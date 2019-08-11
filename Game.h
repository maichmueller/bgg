//
// Created by Michael on 06/03/2019.
//

#ifndef STRATEGO_CPP_GAME_H
#define STRATEGO_CPP_GAME_H


#include <random>
#include <algorithm>
#include <iterator>
#include <functional>

#include "GameState.h"
#include "Agent.h"
#include "utils.h"

class Game {

    int board_len;
    GameState game_state;

    std::shared_ptr<Agent> agent_0;
    std::shared_ptr<Agent> agent_1;

    std::map<strat_pos_t, int> setup_0;
    std::map<strat_pos_t, int> setup_1;

    bool fixed_setups;

public:
    Game(int board_l, std::shared_ptr<Agent> ag0, std::shared_ptr<Agent> ag1, bool f_setups=false);
    Game(int board_l, std::shared_ptr<Agent> ag0, std::shared_ptr<Agent> ag1,
         const std::map<strat_pos_t, int>& setup_0,
         const std::map<strat_pos_t, int>& setup_1);
    Game(int board_len, std::shared_ptr<Agent> ag0, std::shared_ptr<Agent> ag1, Board& board, int move_count=0);

    void reset();

    int run_game(bool show);
    void run_step();

    void set_flag_fixed_setups(bool f_s) {fixed_setups = f_s;}

    void set_setup(const std::map<strat_pos_t, int>& setup, int team) {
        if(team == 0) {setup_0 = setup;}
        else {setup_1 = setup;}
    }

    std::shared_ptr<Agent> get_agent_0() {return agent_0;}
    std::shared_ptr<Agent> get_agent_1() {return agent_1;}
    auto* get_gamestate() {return &game_state;}

    std::map<strat_pos_t, int > draw_random_setup(int team);
};


#endif //STRATEGO_CPP_GAME_H
