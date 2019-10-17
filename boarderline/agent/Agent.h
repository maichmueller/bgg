//
// Created by Michael on 04/03/2019.
//

#ifndef STRATEGO_CPP_AGENT_H
#define STRATEGO_CPP_AGENT_H

#include "../game/GameUtilsStratego.h"
#include "../logic/LogicStratego.h"
#include "../board/Board.h"
#include "../utils/RandomSelector.h"
#include "vector"


class Agent {
protected:
    int m_team;
    bool m_is_learner;

public:
    explicit Agent(int team, bool learner=false)
    : m_team(team), m_is_learner(learner)
    {
    }

    virtual strat_move_t decide_move(const Board& board) = 0;
    virtual void install_board(const Board& board) {}

};

template <typename RDev = std::random_device>
class RandomAgent : public Agent {

    RDev dev;
    std::mt19937 rng;


public:

    explicit RandomAgent(int team)
    : Agent(team), rng(dev())
    {}

    strat_move_t decide_move(const Board &board) override {
        std::vector<strat_move_t> poss_moves = LogicStratego::get_poss_moves(board, m_team);

        std::uniform_int_distribution<std::mt19937::result_type> dist(0, poss_moves.size()-1);

        return poss_moves[dist(rng)];
    }
};



#endif //STRATEGO_CPP_AGENT_H
