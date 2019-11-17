//
// Created by Michael on 04/03/2019.
//

#pragma once

#include "logic/Logic.h"
#include "board/Board.h"
#include "vector"


template<class StateType>
class Agent {
public:
    using state_type = StateType;
    using move_type = typename state_type::move_type;

protected:
    int m_team;
    bool m_is_learner;

public:
    explicit Agent(int team, bool learner = false)
            : m_team(team), m_is_learner(learner) {
    }

    virtual move_type decide_move(const state_type & state, const std::vector<move_type> & poss_moves) = 0;

};

template<class StateType>
class RandomAgent : public Agent<StateType> {
    using state_type = StateType;
    using base_type = Agent<state_type>;
    using base_type::base_type;
    using board_type = typename base_type::board_type;
    using move_type = typename base_type::move_type;

public:

    explicit RandomAgent(int team)
            : base_type(team) {}

    move_type decide_move(const state_type & state, const std::vector<move_type> & poss_moves) override {
        std::array<move_type, 1> selected_move;
        std::sample(
                poss_moves.begin(), poss_moves.end(),
                selected_move.begin(),
                1,
                std::mt19937{std::random_device{}()});

        return selected_move[0];
    }
};



