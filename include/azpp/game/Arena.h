//
// Created by michael on 07.06.19.
//

#ifndef STRATEGO_CPP_ARENA_H
#define STRATEGO_CPP_ARENA_H

#include "unordered_map"
#include "vector"
#include "memory"

#include "agent/Agent.h"
#include "Game.h"

struct StatTrack {
    int wins;
    int draws;
    int losses;
    std::unordered_map<std::string, int> win_reasons;
    std::vector<int> match_counts;

    StatTrack()
    : wins(0), draws(0), losses(0), win_reasons(), match_counts(0)
    {}
    inline void add_win(const std::string& reason, int count=0) {
        wins += 1;
        win_reasons[reason] += 1;
        match_counts.emplace_back(count);
    }
    inline void add_draw() {
        draws += 1;
    }
};


struct Arena {

    static void print_round_results(int round, int num_rounds,
                                    const Agent & agent_0,
                                    const Agent & agent_1,
                                    const StatTrack & stats0, const StatTrack &stats1);

    static std::tuple<StatTrack, StatTrack> pit(Game& game,
                                                int num_sims,
                                                bool show_game=false,
                                                bool save_results=false);


};


#endif //STRATEGO_CPP_ARENA_H
