#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <cxxabi.h>

#include "utils/utils.h"
#include "agent/Agent.h"
#include "game/Game.h"

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

    template <typename StateType>
    static void print_round_results(int round, int num_rounds,
                                    const std::shared_ptr<Agent<StateType>> * agent_0,
                                    const std::shared_ptr<Agent<StateType>> * agent_1,
                                    const StatTrack & stats0,
                                    const StatTrack & stats1);

    template <typename GameType>
    static std::tuple<StatTrack, StatTrack> pit(GameType & game,
                                                int num_sims,
                                                bool show_game=false,
                                                bool save_results=false);


};


template <typename StateType>
void Arena::print_round_results(
        int round,
        int num_rounds,
        const std::shared_ptr<Agent<StateType>> * agent_0,
        const std::shared_ptr<Agent<StateType>> * agent_1,
        const StatTrack &stats0, const StatTrack &stats1) {

    std::string red("\033[1;31m");
    std::string blue("\033[1;34m");
    std::string reset("\033[0m");
    std::stringstream ss;
    ss << "Game " << round << "/" << num_rounds;
    std::string round_display = ss.str();
    ss.clear();
    ss << "Agent 0 (" << blue << get_typename(agent_0) << reset << ")";
    std::string ag_0_display = ss.str();
    ss.clear();
    ss << "Agent 1 (" << red << get_typename(agent_1) << reset << ")";
    std::string ag_1_display = ss.str();
    ss.clear();
    ss << std::right << blue << stats0.wins << reset << utils::repeat(" ", std::to_string(stats0.wins).size() - 4);
    std::string ag_0_wins = ss.str();
    ss.clear();
    ss << std::left << utils::repeat(" ", std::to_string(stats1.wins).size() - 4) << red << stats1.wins << reset;
    std::string ag_1_wins = ss.str();
    ss.clear();
    ss << "\r" << utils::center(round_display, 10, " ") << " "
       << utils::center(ag_0_display, 30, " ") << "-->"
       << ag_0_wins << " : " << ag_1_wins << "<--"
       << utils::center(ag_1_display, 30, " ")
       << "\t Draws: " << round - stats0.wins - stats1.wins;
}



template<typename T>
char *get_typename(T &object) {
    return abi::__cxa_demangle(typeid(object).name(), 0, 0, 0);
}

template <typename GameType>
std::tuple<StatTrack, StatTrack> Arena::pit(GameType &game, int num_sims, bool show_game, bool save_results) {
    StatTrack stats0;
    StatTrack stats1;

    for (int sim = 1; sim < num_sims; ++sim) {
        game.reset();

        int game_outcome = game.run_game(show_game);
        if (game_outcome == 1)
            stats0.add_win("flag", game.get_gamestate()->get_move_count());
        else if (game_outcome == 2)
            stats0.add_win("moves", game.get_gamestate()->get_move_count());
        else if (game_outcome == -1)
            stats1.add_win("flag", game.get_gamestate()->get_move_count());
        else if (game_outcome == -2)
            stats1.add_win("moves", game.get_gamestate()->get_move_count());
        else {
            stats0.add_draw();
            stats1.add_draw();
        }

        if (sim % 10 == 0)
            print_round_results(sim, num_sims, game.get_agent_0(), game.get_agent_1(), stats0, stats1);
    }
    std::cout << std::endl;
    return std::make_tuple(stats0, stats1);
}