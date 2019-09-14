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
#include "../agent/Agent.h"
#include "../utils/utils.h"

template<class GameState, class Agent0, class Agent1>
class Game {
public:
    using game_state_type = GameState;
    using agent_0_type = Agent0;
    using agent_1_type = Agent1;

    using piece_type = typename game_state_type::piece_type;
    using kin_type = typename game_state_type::kin_type;
    using position_type = typename game_state_type::position_type;
    using board_type = typename game_state_type::board_type;
    using action_rep_type = typename game_state_type::action_rep_type;

private:
    using sptr_piece_type = std::shared_ptr<piece_type>;
    using sptr_ag0_type = std::shared_ptr<agent_0_type>;
    using sptr_ag1_type = std::shared_ptr<agent_1_type>;
    using dead_pieces_type = typename game_state_type::dead_pieces_type;

    game_state_type m_game_state;

    std::shared_ptr<agent_0_type> m_agent_0;
    std::shared_ptr<agent_1_type> m_agent_1;

    std::array<std::vector<piece_type>, 2> m_setups;

    bool m_fixed_setups;

    std::map<position_type, piece_type> extract_pieces_from_setup(const std::vector<sptr_piece_type> & setup);
    std::map<position_type, piece_type> extract_pieces_from_setup(const std::map<position_type, kin_type> & setup);
public:

    Game(board_type &&board,
         sptr_ag0_type ag0,
         sptr_ag1_type ag1,
         bool fixed_setups = false,
            // game state necessities
         dead_pieces_type &dead_pieces = dead_pieces_type(),
         int move_count = 0);

    Game(const board_type &board,
         sptr_ag0_type ag0,
         sptr_ag1_type ag1,
         bool fixed_setups = false,
            // game state necessities
         dead_pieces_type &dead_pieces = dead_pieces_type(),
         int move_count = 0);

    template<size_t dim>
    Game(const std::array<int, dim> &shape,
         const std::array<int, dim> &board_starts,
         const std::map<position_type, kin_type> &setup_0,
         const std::map<position_type, kin_type> &setup_1,
         sptr_ag0_type ag0,
         sptr_ag1_type ag1,
         bool fixed_setups = false);

    template<size_t dim>
    Game(const std::array<int, dim> &shape,
         const std::array<int, dim> &board_starts,
         const std::map<position_type, sptr_piece_type> &setup0,
         const std::map<position_type, sptr_piece_type> &setup1,
         sptr_ag0_type ag0,
         sptr_ag1_type ag1,
         bool fixed_setups = false);

    template<size_t dim>
    Game(const std::array<int, dim> &shape,
         const std::array<int, dim> &board_starts,
         sptr_ag0_type ag0,
         sptr_ag1_type ag1,
         bool f_setups = false);

    void reset();

    int run_game(bool show);

    void run_step();

    void set_flag_fixed_setups(bool f_s) { m_fixed_setups = f_s; }

    void set_setup(const std::vector<sptr_piece_type> &setup, int team) {
        m_setups[team] = extract_pieces_from_setup(setup);
    }

    sptr_ag0_type get_agent_0() { return m_agent_0; }

    sptr_ag1_type get_agent_1() { return m_agent_1; }

    const game_state_type * get_gamestate() const { return &m_game_state; }

    virtual std::map<position_type, sptr_piece_type> draw_setup(int team) = 0;
};


template<class GameState, class Agent0, class Agent1>
Game<GameState, Agent0, Agent1>::Game(board_type &&board,
                                      Game::sptr_ag0_type ag0,
                                      Game::sptr_ag1_type ag1,
                                      bool fixed_setups,
                                      dead_pieces_type &dead_pieces,
                                      int move_count)
        : m_game_state(board, dead_pieces, move_count),
          m_agent_0(ag0),
          m_agent_1(ag1),
          m_setups{extract_pieces_from_setup(m_game_state.get_board()->get_setup(0)),
                   extract_pieces_from_setup(m_game_state.get_board()->get_setup(1))},
          m_fixed_setups(fixed_setups)
{}


template<class GameState, class Agent0, class Agent1>
template<size_t dim>
Game<GameState, Agent0, Agent1>::Game(const std::array<int, dim> &shape,
                                      const std::array<int, dim> &board_starts,
                                      Game::sptr_ag0_type ag0,
                                      Game::sptr_ag1_type ag1,
                                      bool fixed_setups)
        : Game(board_type(shape,
                          board_starts,
                          draw_setup(0),
                          draw_setup(1)),
               ag0,
               ag1,
               fixed_setups)
{}


template<class GameState, class Agent0, class Agent1>
Game<GameState, Agent0, Agent1>::Game(const board_type &board,
                                      Game::sptr_ag0_type ag0,
                                      Game::sptr_ag1_type ag1,
                                      bool fixed_setups,
                                      dead_pieces_type &dead_pieces,
                                      int move_count)
        : Game(board_type(board),
               ag0,
               ag1,
               fixed_setups,
               dead_pieces,
               move_count)
{}


template<class GameState, class Agent0, class Agent1>
template<size_t dim>
Game<GameState, Agent0, Agent1>::Game(const std::array<int, dim> &shape,
                                      const std::array<int, dim> &board_starts,
                                      const std::map<position_type, kin_type> &setup_0,
                                      const std::map<position_type, kin_type> &setup_1,
                                      Game::sptr_ag0_type ag0,
                                      Game::sptr_ag1_type ag1,
                                      bool fixed_setups)
        : Game(board_type(shape,
                          board_starts,
                          setup_0,
                          setup_1),
               ag0,
               ag1,
               fixed_setups)
{}

template<class GameState, class Agent0, class Agent1>
std::map<typename Game<GameState, Agent0, Agent1>::position_type,
         typename Game<GameState, Agent0, Agent1>::piece_type>
Game<GameState, Agent0, Agent1>::extract_pieces_from_setup(const std::vector<sptr_piece_type> &setup) {
    std::vector<piece_type> pc_vec(setup.size());
    std::transform(
            setup.begin(),
            setup.end(),
            pc_vec.begin(),
            [](const sptr_piece_type & piece_ptr) -> piece_type { return *piece_ptr; }
    );
    return pc_vec;
}








//class Game {
//
//    int board_len;
//    GameState m_game_state;
//
//    std::shared_ptr<Agent> agent_0;
//    std::shared_ptr<Agent> agent_1;
//
//    std::map<Position, int> setup_0;
//    std::map<Position, int> setup_1;
//
//    bool fixed_setups;
//
//public:
//    Game(int board_l, std::shared_ptr<Agent> ag0, std::shared_ptr<Agent> ag1, bool f_setups=false);
//    Game(int board_l, std::shared_ptr<Agent> ag0, std::shared_ptr<Agent> ag1,
//         const std::map<Position, int>& setup_0,
//         const std::map<Position, int>& setup_1);
//    Game(int board_len, std::shared_ptr<Agent> ag0, std::shared_ptr<Agent> ag1, Board& board, int move_count=0);
//
//    void reset();
//
//    int run_game(bool show);
//    void run_step();
//
//    void set_flag_fixed_setups(bool f_s) {fixed_setups = f_s;}
//
//    void set_setup(const std::map<Position, int>& setup, int team) {
//        if(team == 0) {setup_0 = setup;}
//        else {setup_1 = setup;}
//    }
//
//    std::shared_ptr<Agent> get_agent_0() {return agent_0;}
//    std::shared_ptr<Agent> get_agent_1() {return agent_1;}
//    auto* get_gamestate() {return &m_game_state;}
//
//    std::map<Position, int > draw_setup(int team);
//};


#endif //STRATEGO_CPP_GAME_H
