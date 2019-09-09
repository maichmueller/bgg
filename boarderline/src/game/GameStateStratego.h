//
// Created by Michael on 21/02/2019.
//

#pragma once

#include "../board/BoardStratego.h"
#include "GameState.h"


class GameStateStratego : public GameState<BoardStratego> {
public:
    using base_type = GameState<BoardStratego>;
    using board_type = base_type::board_type;
    using position_type = base_type::position_type;
    using move_type = base_type::move_type;
    using piece_type = base_type::piece_type;

protected:
    using dead_pieces_type = base_type::dead_pieces_type;
    int fight(piece_type& attacker, piece_type& defender);

public:
    explicit GameStateStratego(int game_dim);
    explicit GameStateStratego(board_type board,
                               int move_count=0);
    GameStateStratego(board_type board,
                      dead_pieces_type & dead_pieces,
                      int move_count);
    GameStateStratego(int game_len,
                      const std::map<position_type, typename piece_type::kin_type>& setup_0,
                      const std::map<position_type, typename piece_type::kin_type>& setup_1);

    void check_terminal() override;
    int do_move(const Move<position_type>& move) override;

};

GameStateStratego::GameStateStratego(int game_len)
        : base_type(std::array<int, 2>{game_len, game_len}, {0, 0})
{}


GameStateStratego::GameStateStratego(int game_len,
                                     const std::map<position_type, typename piece_type::kin_type>& setup_0,
                                     const std::map<position_type, typename piece_type::kin_type>& setup_1)
        : GameState(std::array<int, 2>{game_len, game_len}, {0, 0}, setup_0, setup_1)
{}

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


int GameStateStratego::is_terminal(bool force_check, int turn) {
    if(!terminal_checked || force_check)
        check_terminal(false, turn);
    return m_terminal;
}


void GameStateStratego::canonical_board(int player) {
    // if the 0 player is m_team 1, then canonical is false,
    // if it is 0 otherwise, then the teams are canonical
    canonical_teams = bool(1 - player);
}


int GameStateStratego::get_canonical_team(Piece& piece){
    if(canonical_teams) {
        return piece.get_team();
    }
    else {
        return 1 - piece.get_team();
    }
}


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


int GameStateStratego::fight(Piece &attacker, Piece &defender) {
    return StrategoLogic::fight_outcome(attacker.get_type(), defender.get_type());
}


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


typename GameStateStratego::move_type
GameStateStratego::action_to_move(int action, int player) const {
    int board_len = m_board.get_shape();
    int action_dim = ActionRep::get_act_rep(board_len).size();
    return ActionRep::action_to_move(action, action_dim, board_len, actors.at(player), player);
