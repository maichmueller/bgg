//
// Created by Michael on 28/02/2019.
//

#pragma once

#include "../board/Board.h"
#include "../board/Move.h"
#include <functional>


template <typename Position, class Board>
struct Logic {
    using position_type = Position;
    using move_type = Move<position_type>;

    static bool is_legal_move(const Board & board, const move_type & move) = 0;
    static std::vector<move_type> get_legal_moves(const Board & board, int player, bool flip_board=false) = 0;
    static bool has_legal_moves(const Board & board, int player) = 0;
};
