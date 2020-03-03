//
// Created by Michael on 06/03/2019.
//

#pragma once

#include <algorithm>
#include <functional>
#include <iterator>
#include <random>

#include "State.h"
#include "aze/agent/Agent.h"
#include "aze/utils/logging_macros.h"
#include "aze/utils/utils.h"

template < class StateType, class LogicType, class Derived >
class Game {
  public:
   using state_type = StateType;
   using logic_type = LogicType;
   using piece_type = typename state_type::piece_type;
   using kin_type = typename state_type::kin_type;
   using position_type = typename state_type::position_type;
   using move_type = typename state_type::move_type;
   using board_type = typename state_type::board_type;
   using agent_type = Agent< state_type >;

   using sptr_piece_type = std::shared_ptr< piece_type >;

   virtual ~Game() = default;

  protected:
   state_type m_game_state;

   std::array< std::shared_ptr< Agent< state_type > >, 2 > m_agents;

   std::array< std::vector< sptr_piece_type >, 2 > m_setups;

   std::vector< sptr_piece_type > extract_pieces_from_setup(
      const std::map< position_type, kin_type > &setup, int team);

   std::vector< sptr_piece_type > extract_pieces_from_setup(
      const std::map< position_type, sptr_piece_type > &setup, int team);

  public:
   Game(
      board_type &&board,
      std::shared_ptr< Agent< state_type > > ag0,
      std::shared_ptr< Agent< state_type > > ag1,
      int move_count = 0);

   Game(
      const board_type &board,
      std::shared_ptr< Agent< state_type > > ag0,
      std::shared_ptr< Agent< state_type > > ag1,
      int move_count = 0);

   template < size_t dim >
   Game(
      const std::array< size_t, dim > &shape,
      const std::array< int, dim > &board_starts,
      const std::map< position_type, kin_type > &setup_0,
      const std::map< position_type, kin_type > &setup_1,
      std::shared_ptr< Agent< state_type > > ag0,
      std::shared_ptr< Agent< state_type > > ag1);

   template < size_t dim >
   Game(
      const std::array< size_t, dim > &shape,
      const std::array< int, dim > &board_starts,
      const std::map< position_type, sptr_piece_type > &setup0,
      const std::map< position_type, sptr_piece_type > &setup1,
      std::shared_ptr< Agent< state_type > > ag0,
      std::shared_ptr< Agent< state_type > > ag1);

   template < size_t dim >
   Game(
      const std::array< size_t, dim > &shape,
      const std::array< int, dim > &board_starts,
      std::shared_ptr< Agent< state_type > > ag0,
      std::shared_ptr< Agent< state_type > > ag1);

   Game(
      const state_type &state,
      std::shared_ptr< Agent< state_type > > ag0,
      std::shared_ptr< Agent< state_type > > ag1);

   Game(
      state_type &&state,
      std::shared_ptr< Agent< state_type > > ag0,
      std::shared_ptr< Agent< state_type > > ag1);

   void reset(bool fixed_setups = false);

   int run_game(bool show);

   int run_step();

   void set_setup(const std::vector< sptr_piece_type > &setup, int team)
   {
      m_setups[team] = setup;
   }

   std::array< std::shared_ptr< Agent< state_type > >, 2 > get_agents()
   {
      return m_agents;
   }

   std::shared_ptr< Agent< state_type > > get_agent_0() { return m_agents[0]; }

   std::shared_ptr< Agent< state_type > > get_agent_1() { return m_agents[1]; }

   state_type *get_gamestate() { return &m_game_state; }

   std::map< position_type, sptr_piece_type > draw_setup(int team)
   {
      return static_cast< Derived * >(this)->draw_setup_(team);
   }
};

template < class StateType, class LogicType, class Derived >
Game< StateType, LogicType, Derived >::Game(
   board_type &&board,
   std::shared_ptr< Agent< state_type > > ag0,
   std::shared_ptr< Agent< state_type > > ag1,
   int move_count)
    : m_game_state(board, move_count),
      m_agents{ag0, ag1},
      m_setups{
         extract_pieces_from_setup(m_game_state.get_board()->get_setup(0)),
         extract_pieces_from_setup(m_game_state.get_board()->get_setup(1))}
{
}

template < class StateType, class LogicType, class Derived >
template < size_t dim >
Game< StateType, LogicType, Derived >::Game(
   const std::array< size_t, dim > &shape,
   const std::array< int, dim > &board_starts,
   std::shared_ptr< Agent< state_type > > ag0,
   std::shared_ptr< Agent< state_type > > ag1)
    : Game(
       board_type(shape, board_starts, draw_setup(0), draw_setup(1)), ag0, ag1)
{
}

template < class StateType, class LogicType, class Derived >
Game< StateType, LogicType, Derived >::Game(
   const board_type &board,
   std::shared_ptr< Agent< state_type > > ag0,
   std::shared_ptr< Agent< state_type > > ag1,
   int move_count)
    : Game(board_type(std::move(*board.clone())), ag0, ag1, move_count)
{
}

template < class StateType, class LogicType, class Derived >
template < size_t dim >
Game< StateType, LogicType, Derived >::Game(
   const std::array< size_t, dim > &shape,
   const std::array< int, dim > &board_starts,
   const std::map< position_type, kin_type > &setup_0,
   const std::map< position_type, kin_type > &setup_1,
   std::shared_ptr< Agent< state_type > > ag0,
   std::shared_ptr< Agent< state_type > > ag1)
    : Game(board_type(shape, board_starts, setup_0, setup_1), ag0, ag1)
{
}

template < class StateType, class LogicType, class Derived >
Game< StateType, LogicType, Derived >::Game(
   const state_type &state,
   std::shared_ptr< Agent< state_type > > ag0,
   std::shared_ptr< Agent< state_type > > ag1)
    : m_game_state(state), m_agents{ag0, ag1}
{
}

template < class StateType, class LogicType, class Derived >
Game< StateType, LogicType, Derived >::Game(
   state_type &&state,
   std::shared_ptr< Agent< state_type > > ag0,
   std::shared_ptr< Agent< state_type > > ag1)
    : m_game_state(std::move(state)), m_agents{ag0, ag1}
{
}

template < class StateType, class LogicType, class Derived >
std::vector< typename Game< StateType, LogicType, Derived >::sptr_piece_type >
Game< StateType, LogicType, Derived >::extract_pieces_from_setup(
   const std::map< position_type, kin_type > &setup, int team)
{
   using val_type = typename std::map< position_type, kin_type >::value_type;
   std::vector< sptr_piece_type > pc_vec;
   pc_vec.reserve(setup.size());
   std::transform(
      setup.begin(),
      setup.end(),
      std::back_inserter(pc_vec),
      [&](const val_type &pos_kin) -> piece_type {
         return std::make_shared< piece_type >(
            pos_kin.first, pos_kin.second, team);
      });
   return pc_vec;
}

template < class StateType, class LogicType, class Derived >
std::vector< typename Game< StateType, LogicType, Derived >::sptr_piece_type >
Game< StateType, LogicType, Derived >::extract_pieces_from_setup(
   const std::map< position_type, sptr_piece_type > &setup, int team)
{
   using val_type = typename std::map< position_type, sptr_piece_type >::
      value_type;
   std::vector< sptr_piece_type > pc_vec;
   pc_vec.reserve(setup.size());
   std::transform(
      setup.begin(),
      setup.end(),
      std::back_inserter(pc_vec),
      [&](const val_type &pos_piecesptr) -> sptr_piece_type {
         auto piece_sptr = pos_piecesptr.second;
         if(piece_sptr->get_team() != team)
            throw std::logic_error(
               "Pieces of team " + std::to_string(team)
               + " were expected, but received piece of team "
               + std::to_string(piece_sptr->get_team()));
         return piece_sptr;
      });
   return pc_vec;
}

template < class StateType, class LogicType, class Derived >
void Game< StateType, LogicType, Derived >::reset(bool fixed_setups)
{
   auto curr_board_ptr = m_game_state.get_board();
   if(! fixed_setups) {
      m_setups[0] = extract_pieces_from_setup(draw_setup(0), 0);
      m_setups[1] = extract_pieces_from_setup(draw_setup(1), 1);
   }
   m_game_state = state_type(std::make_shared< board_type >(
      curr_board_ptr->get_shape(),
      curr_board_ptr->get_starts(),
      m_setups[0],
      m_setups[1]));
}

template < class StateType, class LogicType, class Derived >
int Game< StateType, LogicType, Derived >::run_step()
{
   size_t turn = (m_game_state.get_turn_count() + 1) % 2;
   auto move = m_agents[turn]->decide_move(
      m_game_state,
      logic_type::get_legal_moves(*m_game_state.get_board(), turn));
   LOGD2(
      "Possible Moves",
      logic_type::get_legal_moves(*m_game_state.get_board(), turn));
   LOGD2("Selected Move by player " + std::to_string(turn), move);

   int outcome = m_game_state.do_move(move);

   return outcome;
}

template < class StateType, class LogicType, class Derived >
int Game< StateType, LogicType, Derived >::run_game(bool show)
{
   while(true) {
      if(show)
         std::cout << m_game_state.get_board()->print_board(false, true);

      // test for game end status
      int outcome = m_game_state.is_terminal();

      LOGD(
         std::string("\n")
         + m_game_state.get_board()->print_board(false, false));
      LOGD2("Status", outcome);

      if(outcome != 404)
         return outcome;
      else
         run_step();
   }
}
