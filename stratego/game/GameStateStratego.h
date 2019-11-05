//
// Created by Michael on 21/02/2019.
//

#pragma once

#include "include_header/game.h"
#include "board/BoardStratego.h"
#include "logic/LogicStratego.h"


class GameStateStratego : public GameState<BoardStratego> {
public:
    using base_type = GameState<BoardStratego>;
    using base_type::base_type;

protected:
    static int fight(piece_type &attacker, piece_type &defender);

public:
    explicit GameStateStratego(size_t shape_x, size_t shape_y);

    explicit GameStateStratego(size_t shape=5);

    GameStateStratego(size_t shape,
                      const std::map<position_type, typename piece_type::kin_type> &setup_0,
                      const std::map<position_type, typename piece_type::kin_type> &setup_1);

    void check_terminal() override;

    int do_move(const move_type &move) override;

};
