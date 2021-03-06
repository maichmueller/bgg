#pragma once

#include <aze/aze.h>

#include "gtest/gtest.h"

namespace {
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
   [[nodiscard]] BoardImplTest* clone_impl() const override
   {
      auto* board_copy_ptr = new BoardImplTest(*this);
      for(auto& sptr : *board_copy_ptr) {
         sptr.second = std::make_shared< piece_type >(*sptr.second);
      }
      return board_copy_ptr;
   }
};
using planar_board = BoardImplTest;

enum Status { tie = 0, win_1, win2 };

class StateImplTest: public State< planar_board, Status > {
  public:
   using base = State< planar_board, Status >;
   using base::base;

   void check_terminal() override
   {
      bool has_pieces = false;
      if(m_board->size())
         has_pieces = true;
      m_status = has_pieces;
   }

   [[nodiscard]] StateImplTest* clone_impl() const override
   {
      // copy the shared pointers as of now
      auto piece_history = m_piece_history;
      for(auto& pieces_arr : piece_history) {
         for(auto& piece_sptr : pieces_arr) {
            piece_sptr = std::make_shared< piece_type >(*piece_sptr);
         }
      }
      auto state_clone_ptr = new StateImplTest(
         std::dynamic_pointer_cast< board_type >(m_board->clone()),
         m_status,
         m_status_checked,
         m_turn_count,
         m_move_history,
         m_rounds_without_fight);

      return state_clone_ptr;
   }
};
using state_type = StateImplTest;
}  // namespace

class StateTest: public ::testing::Test {
  protected:
   std::map< position_type, token_type > setup0;
   std::map< position_type, token_type > setup1;

   void SetUp() override
   {
      setup0[{0, 0}] = {0, 0};
      setup0[{0, 1}] = {1, 0};
      setup0[{0, 2}] = {2, 0};
      setup0[{1, 0}] = {11, 0};
      setup0[{1, 1}] = {10, 0};
      setup0[{1, 2}] = {2, 2};
      setup1[{3, 0}] = {2, 0};
      setup1[{3, 1}] = {2, 1};
      setup1[{3, 2}] = {11, 0};
      setup1[{4, 0}] = {3, 0};
      setup1[{4, 1}] = {1, 0};
      setup1[{4, 2}] = {11, 1};
   }
};
