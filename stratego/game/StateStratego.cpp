#include "StateStratego.h"

StateStratego::StateStratego(size_t shape_x, size_t shape_y)
    : base_type(std::array< size_t, 2 >{shape_x, shape_y}, {0, 0}),
      m_dead_pieces()
{
}

StateStratego::StateStratego(size_t shape) : StateStratego(shape, shape) {}

StateStratego::StateStratego(
   std::array< size_t, 2 > shape,
   const std::map< position_type, kin_type > &setup_0,
   const std::map< position_type, kin_type > &setup_1)
    : base_type(shape, {0, 0}, setup_0, setup_1), m_dead_pieces()
{
}

StateStratego::StateStratego(
   size_t shape,
   const std::map< position_type, kin_type > &setup_0,
   const std::map< position_type, kin_type > &setup_1)
    : StateStratego(std::array< size_t, 2 >{shape, shape}, setup_0, setup_1)
{
}

StateStratego::StateStratego(
   std::array< size_t, 2 > shape,
   const std::map< position_type, int > &setup_0,
   const std::map< position_type, int > &setup_1)
    : base_type(board_type(shape, setup_0, setup_1)), m_dead_pieces()
{
}

StateStratego::StateStratego(
   size_t shape,
   const std::map< position_type, int > &setup_0,
   const std::map< position_type, int > &setup_1)
    : StateStratego({shape, shape}, setup_0, setup_1)
{
}

void StateStratego::check_terminal()
{
   if(auto dead_pieces = m_dead_pieces[0];
      std::find(dead_pieces.begin(), dead_pieces.end(), kin_type{0, 0})
      != dead_pieces.end()) {
      // flag of player 0 has been captured (killed), therefore player 0 lost
      m_terminal = -1;
      return;
   } else if(dead_pieces = m_dead_pieces[1];
             std::find(dead_pieces.begin(), dead_pieces.end(), kin_type{0, 0})
             != dead_pieces.end()) {
      // flag of player 1 has been captured (killed), therefore player 1 lost
      m_terminal = 1;
      return;
   }

   if(! LogicStratego< board_type >::has_legal_moves_(m_board, 0)) {
      m_terminal = -2;
      return;
   } else if(! LogicStratego< board_type >::has_legal_moves_(m_board, 1)) {
      m_terminal = 2;
      return;
   }
   // committing draw rules here
   // Rule 1: If the moves of both players have been repeated 3 times.
   if(m_move_history.size() >= 6) {
      auto move_end = m_move_equals_prev_move.end();
      // check if the moves of the one player are equal for the past 3 rounds
      bool all_equal_0 = *(move_end - 2) && *(move_end - 4) && *(move_end - 6);
      // now check it for the other player
      bool all_equal_1 = *(move_end - 1) && *(move_end - 3) && *(move_end - 5);
      if(all_equal_0 && all_equal_1)
         // players simply repeated their last 3 moves -> draw
         m_terminal = 0;
   }
   // Rule 2: If no fight has happened for 50 rounds in a row.
   if(m_rounds_without_fight > 49) {
      m_terminal = 0;
   }
   m_terminal_checked = true;
}

int StateStratego::fight(piece_type &attacker, piece_type &defender)
{
   return LogicStratego< board_type >::fight_outcome(attacker, defender);
}

int StateStratego::_do_move(const move_type &move)
{
   // preliminaries
   const position_type &from = move[0];
   const position_type &to = move[1];
   int fight_outcome = 404;

   // save the access to the pieces in question
   // (removes redundant searching in board later)
   std::shared_ptr< piece_type > piece_from = m_board[from];
   std::shared_ptr< piece_type > piece_to = m_board[to];
   piece_from->set_flag_has_moved();

   // enact the move
   if(! piece_to->is_null()) {
      // uncover participant pieces
      piece_from->set_flag_unhidden();
      piece_to->set_flag_unhidden();

      m_rounds_without_fight = 0;

      // engage in fight, since piece_to is not a null piece
      fight_outcome = fight(*piece_from, *piece_to);
      if(fight_outcome == 1) {
         // 1 means attacker won, defender died
         auto null_piece = std::make_shared< piece_type >(from);
         m_board.update_board(from, null_piece);
         m_board.update_board(to, piece_from);

         _update_dead_pieces(piece_to);
      } else if(fight_outcome == 0) {
         // 0 means stalemate, both die
         auto null_piece_from = std::make_shared< piece_type >(from);
         m_board.update_board(from, null_piece_from);
         auto null_piece_to = std::make_shared< piece_type >(to);
         m_board.update_board(to, null_piece_to);

         _update_dead_pieces(piece_from);
         _update_dead_pieces(piece_to);
      } else {
         // -1 means defender won, attacker died
         auto null_piece = std::make_shared< piece_type >(from);
         m_board.update_board(from, null_piece);

         _update_dead_pieces(piece_from);
      }
   } else {
      // no fight happened, simply move piece_from onto new position
      auto null_piece = std::make_shared< piece_type >(from);
      m_board.update_board(from, null_piece);
      m_board.update_board(to, piece_from);

      m_rounds_without_fight += 1;
   }
   return fight_outcome;
}
