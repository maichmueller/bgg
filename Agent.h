//
// Created by Michael on 04/03/2019.
//

#ifndef STRATEGO_CPP_AGENT_H
#define STRATEGO_CPP_AGENT_H

#include "GameDeclarations.h"
#include "StrategoLogic.h"
#include "Board.h"
#include "RandomSelector.h"
#include "vector"


class Agent {
protected:
    int team;
    bool is_learner;

public:
    explicit Agent(int team, bool learner=false)
    : team(team), is_learner(learner)
    {
    }

    virtual std::vector<pos_type> decide_move(const Board& board) = 0;
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

    std::vector<pos_type> decide_move(const Board &board) {
        std::vector<std::vector<pos_type >> poss_moves = StrategoLogic::get_poss_moves(board, team);

        std::uniform_int_distribution<std::mt19937::result_type> dist(0, poss_moves.size()-1);

        return poss_moves[dist(rng)];
    }
};



#endif //STRATEGO_CPP_AGENT_H
