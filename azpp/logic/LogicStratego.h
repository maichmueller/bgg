//
// Created by Michael on 28/02/2019.
//

#pragma once

#include "../board/Board.h"
#include "../board/Move.h"
#include "Logic.h"
#include "../board/BoardStratego.h"
#include "BattleMatrix.h"
#include "../../test/BoardTest.h"

#include <functional>

template <class Board>
struct LogicStratego : public Logic<Board, LogicStratego<Board>>{
    using base_type = Logic<Board, LogicStratego<Board>>;
    using board_type = typename base_type::board_type;
    using move_type = typename base_type::move_type;
    using position_type = typename base_type::position_type;
    using kin_type = typename board_type::kin_type;

    static bool is_legal_move(const board_type & board, const move_type & move);

    static std::vector<move_type> get_legal_moves(
            const board_type & board,
            int player,
            bool flip_board=false
    );

    static bool has_legal_moves(const board_type & board, int player);

    static auto get_battle_matrix() {return BattleMatrix::battle_matrix;}
    static int fight_outcome(piece_type attacker, piece_type defender) {
        return fight_outcome(std::array{attacker.get_kin()[0], defender.get_kin()[0]});
    }
    static int fight_outcome(std::array<int, 2> att_def) {
        return BattleMatrix::fight_outcome(att_def);
    }


};



template<typename Board>
bool LogicStratego<Board>::is_legal_move(const board_type &board, const move_type &move) {
    int shape_x = board.get_shape()[0];
    int shape_y = board.get_shape()[1];
    int starts_x = board.get_starts()[0];
    int starts_y = board.get_starts()[1];
    using piece_type = typename board_type::piece_type;
    const auto & [pos_before, pos_after] = move.get_positions();

    if(pos_before[0] < starts_x || pos_before[0] > starts_x + shape_x)
        return false;
    if(pos_before[1] < starts_y || pos_before[1] > starts_y + shape_y)
        return false;
    if(pos_after[0] < starts_x || pos_after[0] > starts_x + shape_x)
        return false;
    if(pos_after[1] < starts_y || pos_after[1] > starts_y + shape_y)
        return false;

    std::shared_ptr<piece_type > p_b = board[pos_before];
    std::shared_ptr<piece_type > p_a = board[pos_after];

    if(p_b->is_null())
        return false;
    if(!p_a->is_null()) {
        if(p_a->get_team() == p_b->get_team())
            return false; // cant fight pieces of own team
        if(p_a->get_kin()[0] == 99)
            return false; // cant fight obstacle
    }

    int move_dist = abs(pos_after[1] - pos_before[1]) + abs(pos_after[0] - pos_before[0]);
    if(move_dist > 1) {
        if(p_b->get_kin()[0] != 2)
            return false;  // not of type 2 , but is supposed to go far

        if(pos_after[0] == pos_before[0]) {
            int dist = pos_after[1] - pos_before[1];
            int sign = (dist >= 0) ? 1 : -1;
            for(int i = 1; i < std::abs(dist); ++i) {
                position_type pos{pos_before[0], pos_before[1] + sign * i};
                if(!board[pos]->is_null())
                    return false;
            }
        }

        else if(pos_after[1] == pos_before[1]) {
            int dist = pos_after[0] - pos_before[0];
            int sign = (dist >= 0) ? 1 : -1;
            for(int i = 1; i < std::abs(dist); ++i) {
                position_type pos = {pos_before[0] + sign * i, pos_before[1]};
                if(!board[pos]->is_null())
                    return false;
            }
        }

        else
            return false;  // diagonal moves not allowed
    }
    return true;
}

template <class Board>
std::vector<typename LogicStratego<Board>::move_type>
LogicStratego<Board>::get_legal_moves(const board_type &board, int player, bool flip_board) {
    using move_type = typename board_type::move_type;
    using position_type = typename board_type::position_type;
    using piece_type = typename board_type::piece_type;

    int shape_x = board.get_shape()[0];
    int shape_y = board.get_shape()[1];
    int starts_x = board.get_starts()[0];
    int starts_y = board.get_starts()[1];
    std::vector<move_type > moves_possible;
    for( auto elem = board.begin(); elem != board.end(); ++elem) {
        std::shared_ptr<piece_type> piece = elem->second;
        if(!piece->is_null() && piece->get_team() == player && piece->get_flag_can_move()) {
            // the position we are dealing with
            Position pos = piece->get_position();

            if(piece->get_kin() == 2) {
                // all possible moves to the right until board ends
                for(int i = 1; i < starts_x + shape_x - pos[0]; ++i) {
                    position_type pos_to{pos[0] + i, pos[1]};
                    move_type move{pos, pos_to};
                    if(is_legal_move(board, move)) {
                        moves_possible.push_back(move);
                    }
                }
                // all possible moves to the top until board ends
                for(int i = 1; i < starts_y + shape_y - pos[1]; ++i) {
                    position_type  pos_to{pos[0], pos[1] + i};
                    move_type move{pos, pos_to};
                    if(is_legal_move(board, move)) {
                        moves_possible.push_back(move);
                    }
                }
                // all possible moves to the left until board ends
                for(int i = 1; i < starts_x + pos[0] + 1; ++i) {
                    position_type  pos_to{pos[0] - i, pos[1]};
                    move_type move{pos, pos_to};
                    if(is_legal_move(board, move)) {
                        moves_possible.push_back(move);
                    }

                }
                // all possible moves to the bottom until board ends
                for(int i = 1; i < starts_y + pos[1] + 1; ++i) {
                    position_type  pos_to{pos[0], pos[1] - i};
                    move_type move{pos, pos_to};
                    if(is_legal_move(board, move)) {
                        moves_possible.push_back(move);
                    }
                }
            }
            else {
                // all moves are 1 step to left, right, top, or bottom
                std::vector<position_type > pos_tos = {{pos[0] + 1, pos[1]},
                                              {pos[0]  , pos[1]+1},
                                              {pos[0]-1,   pos[1]},
                                              {pos[0]  , pos[1]-1}};
                for(auto& pos_to : pos_tos) {
                    move_type move{pos, pos_to};
                    if(is_legal_move(board, move)) {
                        moves_possible.push_back(move);
                    }
                }
            }
        }
    }
    if(flip_board) {
        auto shape = board.get_shape();
        auto starts = board.get_starts();
        for(auto & move : moves_possible) {
            move = move.invert(starts, shape);
        }
    }
    return moves_possible;
}

template <class Board>
bool LogicStratego<Board>::has_legal_moves(const board_type &board, int player) {
    using move_type = typename board_type::move_type;
    using position_type = typename board_type::position_type;
    using piece_type = typename board_type::piece_type;

    int shape_x = board.get_shape()[0];
    int shape_y = board.get_shape()[1];
    int starts_x = board.get_starts()[0];
    int starts_y = board.get_starts()[1];
    for( auto elem = board.begin(); elem != board.end(); ++elem) {
        std::shared_ptr<piece_type> piece = elem->second;
        if(int essential_kin = piece->get_kin()[0];
        !piece->is_null() && piece->get_team() == player && essential_kin != 0 && essential_kin != 11) {
            // the position we are dealing with
            Position pos = piece->get_position();

            if(piece->get_kin()[0] == 2) {
                // all possible moves to the right until board ends
                for(int i = 1; i < starts_x + shape_x - pos[0]; ++i) {
                    position_type pos_to{pos[0] + i, pos[1]};
                    move_type move{pos, pos_to};
                    if(is_legal_move(board, move)) {
                        return true;
                    }
                }
                // all possible moves to the top until board ends
                for(int i = 1; i < starts_y + shape_y - pos[1]; ++i) {
                    position_type  pos_to{pos[0], pos[1] + i};
                    move_type move{pos, pos_to};
                    if(is_legal_move(board, move)) {
                        return true;
                    }
                }
                // all possible moves to the left until board ends
                for(int i = 1; i < starts_x + pos[0] + 1; ++i) {
                    position_type  pos_to{pos[0] - i, pos[1]};
                    move_type move{pos, pos_to};
                    if(is_legal_move(board, move)) {
                        return true;
                    }

                }
                // all possible moves to the bottom until board ends
                for(int i = 1; i < starts_y + pos[1] + 1; ++i) {
                    position_type  pos_to{pos[0], pos[1] - i};
                    move_type move{pos, pos_to};
                    if(is_legal_move(board, move)) {
                        return true;
                    }
                }
            }
            else {
                // all moves are 1 step to left, right, top, or bottom
                std::vector<position_type > pos_tos = {{pos[0] + 1, pos[1]    },
                                                       {pos[0]    , pos[1] + 1},
                                                       {pos[0] - 1, pos[1]    },
                                                       {pos[0]    , pos[1] - 1}};
                for(const auto& pos_to : pos_tos) {
                    move_type move{pos, pos_to};
                    if(is_legal_move(board, move)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
