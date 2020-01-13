#pragma once

#include "azpp/board/Board.h"

namespace BoardTest {

using position_type = Position< int, 2 >;
using piece_type = Piece< position_type, 2 >;
using kin_type = typename piece_type::kin_type;

class BoardImplTest: public Board< piece_type > {
  public:
   using base = Board< piece_type >;
   using base::base;
   [[nodiscard]] std::string print_board(
      int player, bool hide_unknowns) const override
   {
      return "";
   }
};
using planar_board = BoardImplTest;
};  // namespace BoardTest
