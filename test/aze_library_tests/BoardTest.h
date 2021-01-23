#pragma once

#include <array>

#include "aze/board/Board.h"

namespace BoardTest {

using position_type = Position< int, 2 >;
using piece_type = Piece< position_type, 2 >;
using token_type = typename piece_type::token_type;

class BoardImplTest: public Board< piece_type > {
  public:
   using base = Board< piece_type >;
   using base::base;
   [[nodiscard]] std::string print_board(Team team, bool hide_unknowns) const override
   {
      return "";
   }
   BoardImplTest* clone_impl() const override
   {
      return new BoardImplTest(
         std::array< size_t, 2 >{5, 5}, std::array< int, 2 >{0, 0});
   }
};
using planar_board = BoardImplTest;
};  // namespace BoardTest
