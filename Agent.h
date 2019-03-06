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

    int team;
    bool is_learner;

public:
    std::vector<pos_type> decide_move(Board& board) {
        vector<vector<pos_type >> poss_moves = StrategoLogic::get_poss_moves(board, team);
        random_selector<> selector{};
        return selector(poss_moves);
    }

};


#endif //STRATEGO_CPP_AGENT_H
