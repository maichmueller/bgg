
#pragma once

#include "StateStratego.h"
#include "aze/game.h"

class GameStratego:
    public Game< StateStratego, LogicStratego< BoardStratego >, GameStratego > {
  public:
   using base_type = Game<
      StateStratego,
      LogicStratego< BoardStratego >,
      GameStratego >;
   using base_type::base_type;

   GameStratego(
      const std::array< size_t, 2 > &shape,
      const std::map< position_type, int > &setup_0,
      const std::map< position_type, int > &setup_1,
      const std::shared_ptr< Agent< state_type > > &ag0,
      const std::shared_ptr< Agent< state_type > > &ag1,
      bool fixed_setups = false);

   GameStratego(
      size_t shape,
      const std::map< position_type, int > &setup_0,
      const std::map< position_type, int > &setup_1,
      const std::shared_ptr< Agent< state_type > > &ag0,
      const std::shared_ptr< Agent< state_type > > &ag1,
      bool fixed_setups = false);

   GameStratego(
      const std::array< size_t, 2 > &shape,
      const std::map< position_type, kin_type > &setup_0,
      const std::map< position_type, kin_type > &setup_1,
      const std::shared_ptr< Agent< state_type > > &ag0,
      const std::shared_ptr< Agent< state_type > > &ag1,
      bool fixed_setups = false);

   GameStratego(
      size_t shape,
      const std::map< position_type, kin_type > &setup_0,
      const std::map< position_type, kin_type > &setup_1,
      const std::shared_ptr< Agent< state_type > > &ag0,
      const std::shared_ptr< Agent< state_type > > &ag1,
      bool fixed_setups = false);

   std::map< position_type, sptr_piece_type > draw_setup_(int team);
};