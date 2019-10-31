//
// Created by Michael on 04/03/2019.
//

#pragma once

#include "logic/LogicStratego.h"
#include "board/Board.h"
#include "vector"


template <class Board>
class Agent {
public:
    using board_type = Board;
    using move_type = typename board_type::move_type;

protected:
    int m_team;
    bool m_is_learner;

public:
    explicit Agent(int team, bool learner=false)
    : m_team(team), m_is_learner(learner)
    {
    }
    virtual move_type decide_move(const board_type * board) = 0;
    virtual void install_board(const Board& board) {}

};

template <class Board>
class RandomAgent : public Agent<Board> {
    using base_type = Agent<Board>;
    using base_type::base_type;
    using board_type = Board;
    using move_type = typename base_type::move_type;

public:

    explicit RandomAgent(int team)
    : base_type(team)
    {}

    move_type decide_move(const board_type &board) override {
        std::vector<move_type> poss_moves = LogicStratego<board_type >::get_poss_moves(board, base_type::m_team);
        std::array<move_type, 1> selected_move;
        std::sample(
                poss_moves.begin(), poss_moves.end(),
                selected_move.begin(),
                1,
                std::mt19937{std::random_device{}()});

        return selected_move[0];
    }
};



