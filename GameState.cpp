//
// Created by Michael on 21/02/2019.
//

#include "GameState.h"


GameState::GameState(int game_len)
: board(game_len), terminal(404), terminal_checked(true),
  move_count(0), canonical_teams(true), rounds_without_fight(0),
  move_equals_prev_move(0), move_history(0)
{
    map<int, int> team0_dead;
    map<int, int> team1_dead;
    dead_pieces = {team0_dead, team1_dead};
}

GameState::GameState(const Board &board, int move_count)
: board(board), move_count(move_count), terminal(404), terminal_checked(false),
  canonical_teams(true), rounds_without_fight(0), move_equals_prev_move(0),
  move_history(0)
{
    map<int, int> team_0_dead;
    map<int, int> team_1_dead;
    dead_pieces = array<map<int, int>, 2> {team_0_dead, team_1_dead};
    int len = board.get_board_len();
    vector<int> avail_types;
    // copy the available types
    avail_types = ActionRep::get_available_types(len);
    for(auto type: avail_types) {
        team_0_dead[type] += 1;
        team_1_dead[type] += 1;
    }
    for(const auto& piece : board) {
        if( !(piece.second->is_null()) ) {
            if(piece.second->get_team() == 0)
                team_0_dead[piece.second->get_type()] -= 1;
            else
                team_1_dead[piece.second->get_type()] -= 1;
        }
    }
}

GameState::GameState(const Board& board, array<map<int, int>, 2>& dead_pieces, int move_count)
: board(board), dead_pieces(dead_pieces), move_count(move_count),
  terminal_checked(false), terminal(404), canonical_teams(true), rounds_without_fight(0),
  move_equals_prev_move(0), move_history(0)
{
}

GameState::GameState(int len, const std::map<pos_type, int>& setup_0, const std::map<pos_type, int>& setup_1)
: board(len, setup_0, setup_1), dead_pieces(), move_count(0),
  terminal_checked(false), terminal(404), canonical_teams(true), rounds_without_fight(0),
  move_equals_prev_move(0), move_history(0)
{
    map<int, int> team0_dead;
    map<int, int> team1_dead;
    dead_pieces = {team0_dead, team1_dead};
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
        auto move_end = move_equals_prev_move.end();
        // check if the moves of the one player are equal for the past 3 rounds
        bool all_equal_0 = *(move_end - 2) && *(move_end - 4) && *(move_end - 6);
        // now check it for the other player
        bool all_equal_1 = *(move_end - 1) && *(move_end - 3)&& *(move_end - 5);
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

int GameState::is_terminal(bool force_check, int turn) {
    if(!terminal_checked || force_check)
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
        int len = board.get_board_len();
        pos_type pos = piece.get_position();
        pos[0] = len-1-pos[0];
        pos[1] = len-1-pos[1];
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

    // save the access to the pieces in question
    // (removes redundant searching in board later)
    shared_ptr<Piece> piece_from = board[from];
    shared_ptr<Piece> piece_to = board[to];
    piece_from->set_flag_has_moved();

    // save all info to the history
    if(move_equals_prev_move.empty())
        move_equals_prev_move.push_back(false);
    else {
        auto& last_move = move_history.back();
        move_equals_prev_move.push_back((move[0] == last_move[0]) && (move[1] == last_move[1]));
    }
    move_history.push_back(move);
    // copying the pieces here, bc this way they can be fully restored (even with altered flags further on)
    // later on (needed e.g. in undoing last rounds)
    piece_history.push_back({std::make_shared<Piece>(*piece_from), std::make_shared<Piece>(*piece_to)});

    // enact the move
    if(!piece_to->is_null()) {
        // unhide participant pieces
        piece_from->set_flag_unhidden();
        piece_to->set_flag_unhidden();

        rounds_without_fight = 0;

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

        rounds_without_fight += 1;
    }
    move_count += 1;
    terminal_checked = false;
    return fight_outcome;
}


void GameState::undo_last_n_rounds(int n) {
    for(int i = 0; i < n; ++i) {
        vector<pos_type > move = *move_history.end();
        auto move_pieces = *piece_history.end();

        move_history.pop_back();
        piece_history.pop_back();
        move_equals_prev_move.pop_back();

        pos_type from = move[0];
        pos_type to = move[1];
        board[from] = move_pieces[0];
        board[to] = move_pieces[1];
    }
    move_count -= n;
}

void GameState::restore_to_round(int round) {
    undo_last_n_rounds(move_count - round);
}