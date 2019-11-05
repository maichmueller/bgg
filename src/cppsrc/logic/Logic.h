//
// Created by Michael on 28/02/2019.
//

#pragma once

#include "board/Board.h"
#include "board/Move.h"
#include <functional>


template <class BoardType, class Derived>
struct Logic {
    using board_type = BoardType;
    using position_type = typename board_type::position_type;
    using move_type = typename board_type::move_type;

    static bool is_legal_move(const board_type & board, const move_type & move) {
        return Derived::is_legal_move(board, move);
    };
    static std::vector<move_type> get_legal_moves(const board_type & board, int player, bool flip_board) {
        return Derived::get_legal_moves(board, player, flip_board);
    }
    static bool has_legal_moves(const board_type & board, int player) {
        return Derived::has_legal_moves(board, player);
    }
};
