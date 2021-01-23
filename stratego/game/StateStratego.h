
#pragma once

#include <unordered_set>

#include "aze/game.h"
#include "aze/game/Defs.h"
#include "board/BoardStratego.h"
#include "logic/LogicStratego.h"

class HistoryStratego {
   using move_type = BoardStratego::move_type;
   using piece_type = BoardStratego::piece_type;

   inline auto get_by_turn(size_t turn) -> std::tuple< Team, move_type, std::array< piece_type, 2 > > {
      return {m_teams[turn], m_moves[turn], m_pieces[turn]};
   }
   inline auto get_by_index(size_t turn) -> std::tuple< Team, move_type, std::array< piece_type, 2 > > {
      auto turn = m_turns[turn];
      return {m_teams[turn], m_moves[turn], m_pieces[turn]};
   }

   void commit_move(const BoardStratego& board, move_type move, size_t turn) {
      auto from = move.
   }

   def commit_move(self, board: Board, move: Move, turn: int):
   """
   Commit the current move to history.
   """
   from_ = move.from_
   to_ = move.to_
   self.move[turn] = move
      self.pieces[turn] = copy.deepcopy(board[from_]), copy.deepcopy(board[to_])
   self.team[turn] = Team(turn % 2)
   self.turns.append(turn)

   def pop_last(self):
   """
   Remove the latest entries from the history. Return the contents, that were removed.
   Returns
   -------
   tuple,
   all removed entries in sequence: turn, team, move, pieces
   """
   turn = self.turns[-1]
   return turn, self.team.pop(turn), self.move.pop(turn), self.pieces.pop(turn)

  private:
   std::vector<size_t> m_turns;
   std::map< size_t, BoardStratego::move_type > m_moves;
   std::map< size_t, Team > m_teams;
   std::map< size_t, std::array< BoardStratego::piece_type, 2 > > m_pieces;
};

class StateStratego: public State< BoardStratego > {
  public:
   using base_type = State< BoardStratego >;

   // just decorate all base constructors with initializing also the dead pieces
   // variable.
   template < typename... Params >
   StateStratego(Params &&...params) : base_type(std::forward< Params >(params)...), m_dead_pieces()
   {
   }

   // also declare some explicit constructors
   explicit StateStratego(size_t shape_x, size_t shape_y);

   explicit StateStratego(size_t shape = 5);

   StateStratego(
      size_t shape,
      const std::map< position_type, token_type > &setup_0,
      const std::map< position_type, token_type > &setup_1);

   StateStratego(
      std::array< size_t, 2 > shape,
      const std::map< position_type, token_type > &setup_0,
      const std::map< position_type, token_type > &setup_1);

   StateStratego(
      size_t shape,
      const std::map< position_type, int > &setup_0,
      const std::map< position_type, int > &setup_1);

   StateStratego(
      std::array< size_t, 2 > shape,
      const std::map< position_type, int > &setup_0,
      const std::map< position_type, int > &setup_1);

   int _do_move(const move_type &move) override;

  protected:
   static int fight(piece_type &attacker, piece_type &defender);

  private:
   using dead_pieces_type = std::array< std::unordered_set< token_type >, 2 >;
   dead_pieces_type m_dead_pieces;

   void _update_dead_pieces(const sptr< piece_type > &piece)
   {
      if(! piece->is_null())
         m_dead_pieces[piece->get_team()].emplace(piece->get_token());
   }

   StateStratego *clone_impl() const override;
};
