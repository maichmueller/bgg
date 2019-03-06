//
// Created by Michael on 06/03/2019.
//

#ifndef STRATEGO_CPP_GAME_H
#define STRATEGO_CPP_GAME_H


#include "GameState.h"
#include "Agent.h"

class Game {

    int board_len;
    GameState game_state;

    std::shared_ptr<Agent> agent_0;
    std::shared_ptr<Agent> agent_1;

    std::vector<std::shared_ptr<Piece>> setup_0;
    std::vector<std::shared_ptr<Piece>> setup_1;

    bool fixed_setups;

public:
    Game(int board_len, std::shared_ptr<Agent> ag0, std::shared_ptr<Agent> ag1, bool fixed_setups=false);
    Game(int board_len, std::shared_ptr<Agent> ag0, std::shared_ptr<Agent> ag1,
            std::vector<std::shared_ptr<Piece>> setup_0, std::vector<std::shared_ptr<Piece>> setup_1);

    void reset(bool fixed_setups= false);

    int run_game(bool show);
    int run_step();

    void set_flag_fixed_setups(bool f_s) {fixed_setups = f_s;}

    std::shared_ptr<Agent> get_agent_0() {return agent_0;}
    std::shared_ptr<Agent> get_agent_1() {return agent_1;}



};


#endif //STRATEGO_CPP_GAME_H
