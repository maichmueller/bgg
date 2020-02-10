//
// Created by Michael on 21/02/2019.
//

#pragma once

#include "azpp/board/Board.h"
#include "azpp/board/Move.h"
#include "azpp/board/Piece.h"
#include "azpp/board/Position.h"

template < class BoardType >
class State {
  public:
   using board_type = BoardType;
   using piece_type = typename BoardType::piece_type;
   using kin_type = typename BoardType::kin_type;
   using position_type = typename BoardType::position_type;
   using move_type = Move< position_type >;

  protected:
   board_type m_board;

   int m_terminal;
   bool m_terminal_checked;

   int m_turn_count;

   std::vector< move_type > m_move_history;
   std::vector< std::array< std::shared_ptr< piece_type >, 2 > >
      m_piece_history;
   std::vector< bool > m_move_equals_prev_move;
   unsigned int m_rounds_without_fight;

   void _recompute_rounds_without_fight();
   virtual int _do_move(const move_type &move);
   virtual State< board_type > *clone_impl() = 0;

  public:
   template < size_t dim >
   explicit State(
      const std::array< size_t, dim > &shape,
      const std::array< int, dim > &board_starts);

   explicit State(board_type &&board, int move_count = 0);

   explicit State(const board_type &board, int move_count = 0);

   template < size_t dim >
   State(
      const std::array< size_t, dim > &shape,
      const std::array< int, dim > &board_starts,
      const std::map< position_type, typename piece_type::kin_type > &setup_0,
      const std::map< position_type, typename piece_type::kin_type > &setup_1);

   virtual ~State() = default;

   auto &operator[](const position_type &position) { return m_board[position]; }

   const auto &operator[](const position_type &position) const
   {
      return m_board[position];
   }

   std::shared_ptr< State< board_type > > clone()
   {
      return std::shared_ptr(clone_impl());
   }

   int is_terminal(bool force_check = false);

   virtual void check_terminal() = 0;

   int do_move(const move_type &move);

   virtual void restore_to_round(int round);

   void undo_last_rounds(int n = 1);

   int get_turn_count() const { return m_turn_count; }

   void set_board(board_type brd) { this->m_board = std::move(brd); }

   const board_type *get_board() const { return &m_board; }

   virtual std::string string_representation(int player, bool hide_unknowns);
};

template < class BoardType >
int State< BoardType >::_do_move(const State::move_type &move)
{
   m_board.update_board(move[1], m_board[move[0]]);
   m_board.update_board(move[0], std::make_shared< piece_type >(move[0]));
   return 0;
}

template < class BoardType >
State< BoardType >::State(board_type &&board, int move_count)
    : m_board(std::move(board)),
      m_terminal(404),
      m_terminal_checked(false),
      m_turn_count(move_count),
      m_move_history(),
      m_move_equals_prev_move(0),
      m_rounds_without_fight(0)
{
}

template < class BoardType >
State< BoardType >::State(const board_type &board, int move_count)
    : State(board_type(board), move_count)
{
}

template < class BoardType >
template < size_t dim >
State< BoardType >::State(
   const std::array< size_t, dim > &shape,
   const std::array< int, dim > &board_starts)
    : State(board_type(shape, board_starts))
{
}

template < class BoardType >
template < size_t dim >
State< BoardType >::State(
   const std::array< size_t, dim > &shape,
   const std::array< int, dim > &board_starts,
   const std::map< position_type, typename piece_type::kin_type > &setup_0,
   const std::map< position_type, typename piece_type::kin_type > &setup_1)
    : State(board_type(shape, board_starts, setup_0, setup_1))
{
}

template < class BoardType >
int State< BoardType >::is_terminal(bool force_check)
{
   if(! m_terminal_checked || force_check)
      check_terminal();
   return m_terminal;
}

template < class BoardType >
void State< BoardType >::undo_last_rounds(int n)
{
   // rwf = rounds without fight
   bool recompute_rwf = false;

   for(int i = 0; i < n; ++i) {
      move_type move = m_move_history.back();
      auto move_pieces = m_piece_history.back();

      if(m_rounds_without_fight > 0)
         m_rounds_without_fight -= 1;
      else
         recompute_rwf = true;

      m_move_history.pop_back();
      m_piece_history.pop_back();
      m_move_equals_prev_move.pop_back();

      m_board.update_board(move[1], move_pieces[1]);
      m_board.update_board(move[0], move_pieces[0]);
   }

   m_turn_count -= n;
   if(recompute_rwf) {
      _recompute_rounds_without_fight();
   }
}

template < class BoardType >
void State< BoardType >::restore_to_round(int round)
{
   undo_last_rounds(m_turn_count - round);
}

template < class BoardType >
int State< BoardType >::do_move(const State::move_type &move)
{
   // save all info to the history
   std::shared_ptr< piece_type > piece_from = m_board[move[0]];
   std::shared_ptr< piece_type > piece_to = m_board[move[1]];
   if(m_move_equals_prev_move.empty())
      m_move_equals_prev_move.push_back(false);
   else {
      auto &last_move = m_move_history.back();
      m_move_equals_prev_move.push_back(
         (move[0] == last_move[0]) && (move[1] == last_move[1]));
   }
   m_move_history.push_back(move);
   // copying the pieces here, bc this way they can be fully restored later on
   // (especially when flags have been altered - needed in undoing last rounds)
   m_piece_history.push_back({std::make_shared< piece_type >(*piece_from),
                              std::make_shared< piece_type >(*piece_to)});

   m_terminal_checked = false;
   m_turn_count += 1;

   return _do_move(move);
}

template < class BoardType >
std::string State< BoardType >::string_representation(
   int player, bool hide_unknowns)
{
   std::stringstream sstream;
   sstream << get_board()->print_board(player, hide_unknowns) << "\n";
   sstream << "turn count" << m_turn_count << "\n";
   //    sstream << "rounds without fight" << m_rounds_without_fight << "\n";
   return sstream.str();
}
template < class BoardType >
void State< BoardType >::_recompute_rounds_without_fight()
{
   m_rounds_without_fight = 0;
   for(auto piece_rev_iter = m_piece_history.rbegin();
       piece_rev_iter != m_piece_history.rend();
       ++piece_rev_iter) {
      if(! (*piece_rev_iter)[1]->is_null())
         // if the defending piece was not a null piece, then there was a fight
         break;
      else
         m_rounds_without_fight += 1;
   }
}
template < class BoardType >
State< BoardType > State< BoardType >::clone()
{
   // copy the shared pointers as of now
   auto piece_history = m_piece_history;
   for(auto &pieces_arr : piece_history) {
      for(auto &piece_sptr : pieces_arr) {
         piece_sptr = std::make_shared< piece_type >(*piece_sptr);
      }
   }
   decltype(*this) state_copy(
      m_board.clone(),
      m_terminal,
      m_terminal_checked,
      m_turn_count,
      m_move_history,
      m_piece_history,
      m_move_equals_prev_move,
      m_rounds_without_fight);

   return state_copy;
}
