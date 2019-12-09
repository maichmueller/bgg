//
// Created by Michael on 21/02/2019.
//

#pragma once

#include "azpp/game.h"
#include "board/BoardStratego.h"
#include "logic/LogicStratego.h"


class StateStratego : public State<BoardStratego> {
public:
    using base_type = State<BoardStratego>;
    using base_type::base_type;

protected:
    static int fight(piece_type &attacker, piece_type &defender);

public:
    explicit StateStratego(size_t shape_x, size_t shape_y);

    explicit StateStratego(size_t shape=5);

    StateStratego(size_t shape,
                  const std::map<position_type, kin_type> &setup_0,
                  const std::map<position_type, kin_type> &setup_1);

    StateStratego(std::array<size_t, 2> shape,
                  const std::map<position_type, kin_type> &setup_0,
                  const std::map<position_type, kin_type> &setup_1);

    StateStratego(size_t shape,
                  const std::map<position_type, int> &setup_0,
                  const std::map<position_type, int> &setup_1);

    StateStratego(std::array<size_t, 2> shape,
                  const std::map<position_type, int> &setup_0,
                  const std::map<position_type, int> &setup_1);

    void check_terminal() override;

    int _do_move(const move_type &move) override;

};
