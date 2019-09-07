//
// Created by Michael on 21/02/2019.
//

#pragma once

#include "../board/BoardStratego.h"
#include "GameState.h"


class GameStateStratego : public GameState<BoardStratego> {
public:
    using game_state_base = GameState<BoardStratego>;
    using board_type = game_state_base::board_type;
    using position_type = game_state_base::position_type;
    using move_type = game_state_base::move_type;
    using piece_type = game_state_base::piece_type;

private:
    using dead_pieces_type = game_state_base::dead_pieces_type;
    int fight(piece_type& attacker, piece_type& defender);

public:
    explicit GameStateStratego(int game_dim);
    explicit GameStateStratego(board_type board,
                               int move_count=0);
    GameStateStratego(board_type board,
                      std::array<std::map<int, int>, 2>& dead_pieces,
                      int move_count);
    GameStateStratego(int len,
                      const std::map<position_type, int>& setup_0,
                      const std::map<position_type, int>& setup_1);

    void check_terminal() override;
    int do_move(Move<position_type>& move) override;

};

template <class Board>
GameStateStratego::GameStateStratego(int game_len)
        : m_board(game_len), m_terminal(404), terminal_checked(true),
          move_count(0), canonical_teams(true), rounds_without_fight(0),
          move_equals_prev_move(0), move_history(0)
{
    std::map<int, int> team0_dead;
    std::map<int, int> team1_dead;
    m_dead_pieces = {team0_dead, team1_dead};
    assign_actors(this->m_board);
}

template <class Board>
GameStateStratego::GameState(board_type board, int move_count)
        : m_board(std::move(board)), move_count(move_count), m_terminal(404), terminal_checked(false),
          canonical_teams(true), rounds_without_fight(0), move_equals_prev_move(0),
          move_history(0)
{
    std::map<int, int> team_0_dead;
    std::map<int, int> team_1_dead;
    m_dead_pieces = std::array<std::map<int, int>, 2> {team_0_dead, team_1_dead};
    int len = board.get_shape();
    std::vector<int> avail_types;
    // copy the available types
    avail_types = GameDeclarations::get_available_types(len);
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
    assign_actors(this->m_board);
}

template <class Board>
GameStateStratego::GameState(board_type board, std::array<std::map<int, int>, 2>& dead_pieces, int move_count)
        : m_board(std::move(board)), m_dead_pieces(dead_pieces), move_count(move_count),
          terminal_checked(false), m_terminal(404), canonical_teams(true), rounds_without_fight(0),
          move_equals_prev_move(0), move_history(0)
{
    assign_actors(this->m_board);
}

template <class Board>
GameStateStratego::GameState(int len, const std::map<Position, int>& setup_0, const std::map<Position, int>& setup_1)
        : m_board(len, setup_0, setup_1), m_dead_pieces(), move_count(0),
          terminal_checked(false), m_terminal(404), canonical_teams(true), rounds_without_fight(0),
          move_equals_prev_move(0), move_history(0)
{
    std::map<int, int> team0_dead;
    std::map<int, int> team1_dead;
    m_dead_pieces = {team0_dead, team1_dead};
    assign_actors(m_board);
}

template <class Board>
void GameStateStratego::assign_actors(const board_type &board) {
    for(const auto& entry: board) {
        const auto& piece = entry.second;
        if(!piece->is_null() && piece->get_type() != 99)
            actors[piece->get_team()][std::make_tuple(piece->get_type(), piece->get_version())] = piece;
    }
}

template <class Board>
void GameStateStratego::check_terminal() {
    if(m_dead_pieces[0][0] == 1) {
        m_terminal = -1;
        return;
    }
    else if(m_dead_pieces[1][0] == 1) {
        m_terminal = 1;
        return;
    }

    if (!StrategoLogic::has_poss_moves(m_board, 0)) {
        m_terminal = -2;
        return;
    } else if (!StrategoLogic::has_poss_moves(m_board, 1)) {
        m_terminal = 2;
        return;
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
            m_terminal = 0;
    }
    // Rule 2: If no fight has happened for 50 rounds in a row.
    if(rounds_without_fight > 49) {
        m_terminal = 0;
    }
    terminal_checked = true;
}

template <class Board>
int GameStateStratego::is_terminal(bool force_check, int turn) {
    if(!terminal_checked || force_check)
        check_terminal(false, turn);
    return m_terminal;
}

template <class Board>
void GameStateStratego::canonical_board(int player) {
    // if the 0 player is m_team 1, then canonical is false,
    // if it is 0 otherwise, then the teams are canonical
    canonical_teams = bool(1 - player);
}

template <class Board>
int GameStateStratego::get_canonical_team(Piece& piece){
    if(canonical_teams) {
        return piece.get_team();
    }
    else {
        return 1 - piece.get_team();
    }
}

template <class Board>
Position GameStateStratego::get_canonical_pos(Piece& piece){
    if(canonical_teams) {
        return piece.get_position();
    }
    else {
        int len = m_board.get_shape();
        Position pos = piece.get_position();
        pos[0] = len-1-pos[0];
        pos[1] = len-1-pos[1];
        return pos;
    }
}

template <class Board>
int GameStateStratego::fight(Piece &attacker, Piece &defender) {
    return StrategoLogic::fight_outcome(attacker.get_type(), defender.get_type());
}

template <class Board>
int GameStateStratego::do_move(move_type &move) {
    // preliminaries
    Position from = move[0];
    Position to = move[1];
    int fight_outcome = 404;

    // save the access to the pieces in question
    // (removes redundant searching in board later)
    std::shared_ptr<Piece> piece_from = m_board[from];
    std::shared_ptr<Piece> piece_to = m_board[to];
    piece_from->set_flag_has_moved();

    // save all info to the history
    if(move_equals_prev_move.empty())
        move_equals_prev_move.push_back(false);
    else {
        auto& last_move = move_history.back();
        move_equals_prev_move.push_back((move[0] == last_move[0]) && (move[1] == last_move[1]));
    }
    move_history.push_back(move);
    // copying the pieces here, bc this way they can be fully restored later on
    // (especially when flags have been altered - needed e.g. in undoing last rounds)
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
            m_board.update_board(to, piece_from);
            auto null_piece = std::make_shared<Piece> (from);
            m_board.update_board(from, null_piece);

            m_dead_pieces[piece_to->get_team()][piece_to->get_type()] += 1;
        }
        else if(fight_outcome == 0) {
            // 0 means stalemate, both die
            auto null_piece_from = std::make_shared<Piece> (from);
            m_board.update_board(from, null_piece_from);
            auto null_piece_to = std::make_shared<Piece> (to);
            m_board.update_board(to, null_piece_to);

            m_dead_pieces[piece_from->get_team()][piece_from->get_type()] += 1;
            m_dead_pieces[piece_to->get_team()][piece_to->get_type()] += 1;
        }
        else {
            // -1 means defender won, attacker died
            auto null_piece = std::make_shared<Piece> (from);
            m_board.update_board(from, null_piece);

            m_dead_pieces[piece_from->get_team()][piece_from->get_type()] += 1;
        }
    }
    else {
        // no fight happened, simply move piece_from onto new position
        auto null_piece = std::make_shared<Piece> (from);
        m_board.update_board(from, null_piece);
        m_board.update_board(to, piece_from);

        rounds_without_fight += 1;
    }
    move_count += 1;
    terminal_checked = false;
    return fight_outcome;
}

template <class Board>
torch::Tensor GameStateStratego::torch_represent(int player) {
    if(!conditions_set) {
        auto type_counter = utils::counter(GameDeclarations::get_available_types(m_board.get_shape()));
        conditions_torch_rep = StateRepresentation::create_conditions(type_counter, 0);
        conditions_set = true;
    }

    return StateRepresentation::b2s_cond_check(
            m_board,
            conditions_torch_rep,
            player);
}

template <class Board>
typename GameStateStratego::move_type
GameStateStratego::action_to_move(int action, int player) const {
    int board_len = m_board.get_shape();
    int action_dim = ActionRep::get_act_rep(board_len).size();
    return ActionRep::action_to_move(action, action_dim, board_len, actors.at(player), player);
