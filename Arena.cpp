//
// Created by michael on 07.06.19.
//

#include <cxxabi.h>

#include "Arena.h"


template <typename T>
char* get_typename(T& object)
{
    return abi::__cxa_demangle(typeid(object).name(), 0, 0, 0);
}

void Arena::print_round_results(int round, int num_rounds, const Agent &agent_0,
                                const Agent &agent_1,
                                const StatTrack& stats0, const StatTrack & stats1) {

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

std::tuple<StatTrack, StatTrack> Arena::pit(Game& game, int num_sims, bool show_game, bool save_results) {
    StatTrack stats0;
    StatTrack stats1;

    for(int sim = 1; sim < num_sims; ++sim) {
        game.reset();

        int game_outcome = game.run_game(show_game);
        if(game_outcome == 1)
            stats0.add_win("flag", game.get_gamestate()->get_move_count());
        else if(game_outcome == 2)
            stats0.add_win("moves", game.get_gamestate()->get_move_count());
        else if(game_outcome == -1)
            stats1.add_win("flag", game.get_gamestate()->get_move_count());
        else if(game_outcome == -2)
            stats1.add_win("moves", game.get_gamestate()->get_move_count());
        else {
            stats0.add_draw();
            stats1.add_draw();
        }

        if(sim % 10 == 0)
            print_round_results(sim, num_sims, *(game.get_agent_0()), *(game.get_agent_1()), stats0, stats1);
    }
    std::cout << std::endl;
    return std::make_tuple(stats0, stats1);
}