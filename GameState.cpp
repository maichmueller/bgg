//
// Created by Michael on 21/02/2019.
//

#include "GameState.h"


GameState::GameState(int game_dim)
: board(game_dim), terminal(404), terminal_checked(true),
  move_count(0), canonical_teams(true)
{
    map<int, int> team0_dead;
    map<int, int> team1_dead;
    dead_pieces = {team0_dead, team1_dead};
}

GameState::GameState(Board &board, int move_count)
: board(board), move_count(move_count), terminal(404), terminal_checked(false),
    canonical_teams(true)
{
    map<int, int> team_0_dead;
    map<int, int> team_1_dead;
    dead_pieces = array<map<int, int>, 2> {team_0_dead, team_1_dead};
    int dim = board.get_board_dim_size();
    vector<int> avail_types;
    // copy the available types
    avail_types = ActionRep::get_available_types(dim);
    for(auto type: avail_types) {
        team_0_dead[type] += 1;
        team_1_dead[type] += 1;
    }
    for(auto& piece : board) {
        if( !(piece.second->is_null()) ) {
            if(piece.second->get_team() == 0)
                team_0_dead[piece.second->get_type()] -= 1;
            else
                team_1_dead[piece.second->get_type()] -= 1;
        }
    }
    check_terminal();
}

GameState::GameState(Board& board, array<map<int, int>, 2>& dead_pieces, int move_count)
: board(board), dead_pieces(dead_pieces), move_count(move_count),
    terminal_checked(false), terminal(404), canonical_teams(true)
{
}

void GameState::check_terminal(bool flag_only, int turn) {
    if(dead_pieces[0][0] == 1) {
        terminal = -1;
        return;
    }
    else if(dead_pieces[1][0] == 1) {
        terminal = 1;
        return;
    }

    if(!flag_only) {
        if (!StrategoLogic::has_poss_moves(board, turn)) {
            terminal = ((turn == 1) ? 1 : -1) * 2;
            return;
        } else if (!StrategoLogic::has_poss_moves(board, (turn + 1) % 2)) {
            terminal = ((turn == 0) ? 1 : -1) * 2;
            return;
        }
    }
    // committing draw rules here
    // Rule 1: If the moves of both players have been repeated 3 times.
    if(move_history.size() >= 6) {
        auto last_move_0 = move_equals_prev_move.end();
        bool last_move_1 = *(last_move_0 - 1);
        bool last_move_2 = *(last_move_0 - 2);
        bool last_move_3 = *(last_move_0 - 3);
        bool last_move_4 = *(last_move_0 - 4);
        bool last_move_5 = *(last_move_0 - 5);
        // check if the moves of the one player are equal for the past 3 rounds
        bool all_equal_0 = *last_move_0 && last_move_2 && last_move_4;
        // now check it for the other player
        bool all_equal_1 = last_move_1 && last_move_3 && last_move_5;
        if(all_equal_0 && all_equal_1)
            // players simply repeated their last 3 moves -> draw
            terminal = 0;
        }
    // Rule 2: If no fight has happened for 50 rounds in a row.
    if(rounds_without_fight > 49) {
        terminal = 0;
    }
    terminal_checked = true;
}

int GameState::is_terminal(bool force_reload, int turn) {
    if(!terminal_checked)
        check_terminal(false, turn);
    return terminal;
}


void GameState::canonical_board(int player) {
    // if the 0 player is team 1, then canonical is false,
    // if it is 0 otherwise, then the teams are canonical
    canonical_teams = bool(1 - player);
}

int GameState::get_canonical_team(Piece& piece){
    if(canonical_teams) {
        return piece.get_team();
    }
    else {
        return 1 - piece.get_team();
    }
}

pos_type GameState::get_canonical_pos(Piece& piece){
    if(canonical_teams) {
        return piece.get_position();
    }
    else {
        int dim = board.get_board_dim_size();
        pos_type pos = piece.get_position();
        pos[0] = dim-1-pos[0];
        pos[1] = dim-1-pos[1];
        return pos;
    }
}

int GameState::fight(Piece &attacker, Piece &defender) {
    return StrategoLogic::fight_outcome(attacker.get_type(), defender.get_type());
}

int GameState::do_move(vector<pos_type> &move) {
    // preliminaries
    pos_type from = move[0];
    pos_type to = move[1];
    int fight_outcome = 404;

    shared_ptr<Piece> piece_from = board[from];
    shared_ptr<Piece> piece_to = board[to];
    piece_from->set_flag_has_moved();

    // save all info to the history
    move_history.push_back(move);
    vector<shared_ptr<Piece>> pieces_in_move = {piece_from, piece_to};
    piece_history.push_back(pieces_in_move);

    if(!piece_to->is_null()) {
        // unhide participant pieces
        piece_from->set_flag_unhidden();
        piece_to->set_flag_unhidden();

        // engage in fight, since piece_to is not a null piece
        fight_outcome = fight(*piece_from, *piece_to);
        if(fight_outcome == 1) {
            // 1 means attacker won, defender died
            board.update_board(to, piece_from);
            auto null_piece = std::make_shared<Piece> (from);
            board.update_board(from, null_piece);

            dead_pieces[piece_to->get_team()][piece_to->get_type()] += 1;
        }
        else if(fight_outcome == 0) {
            // 0 means draw of fight, both die
            auto null_piece_from = std::make_shared<Piece> (from);
            board.update_board(from, null_piece_from);
            auto null_piece_to = std::make_shared<Piece> (to);
            board.update_board(to, null_piece_to);

            dead_pieces[piece_from->get_team()][piece_from->get_type()] += 1;
            dead_pieces[piece_to->get_team()][piece_to->get_type()] += 1;
        }
        else {
            // -1 means defender won, attacker died
            auto null_piece = std::make_shared<Piece> (from);
            board.update_board(from, null_piece);

            dead_pieces[piece_from->get_team()][piece_from->get_type()] += 1;
        }
    }
    else {
        // no fight happened, simply move piece_from onto new postion
        auto null_piece = std::make_shared<Piece> (from);
        board.update_board(from, null_piece);
        board.update_board(to, piece_from);
    }
    move_count += 1;
    return fight_outcome;
}

