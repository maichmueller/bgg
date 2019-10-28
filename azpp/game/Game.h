//
// Created by Michael on 06/03/2019.
//

#pragma once


#include <random>
#include <algorithm>
#include <iterator>
#include <functional>

#include "GameState.h"
#include "../agent/Agent.h"
#include "../utils/utils.h"


template<class GameState, class Derived>
class Game {
public:
    using state_type = GameState;

    using piece_type = typename state_type::piece_type;
    using kin_type = typename state_type::kin_type;
    using position_type = typename state_type::position_type;
    using move_type = typename state_type::move_type;
    using board_type = typename state_type::board_type;

    using sptr_piece_type = std::shared_ptr<piece_type>;

protected:

    state_type m_game_state;

    std::shared_ptr<Agent<board_type>> m_agent_0;
    std::shared_ptr<Agent<board_type>> m_agent_1;

    std::array<std::vector<piece_type>, 2> m_setups;

    bool m_fixed_setups;

    std::vector<piece_type> extract_pieces_from_setup(const std::vector<sptr_piece_type> &setup);

    std::vector<piece_type> extract_pieces_from_setup(const std::map<position_type, kin_type> &setup, int team);

public:

    Game(board_type &&board,
         std::shared_ptr<Agent<board_type>> ag0,
         std::shared_ptr<Agent<board_type>> ag1,
         bool fixed_setups = false,
         int move_count = 0);

    Game(const board_type &board,
         std::shared_ptr<Agent<board_type>> ag0,
         std::shared_ptr<Agent<board_type>> ag1,
         bool fixed_setups = false,
         int move_count = 0);

    template<size_t dim>
    Game(const std::array<int, dim> &shape,
         const std::array<int, dim> &board_starts,
         const std::map<position_type, kin_type> &setup_0,
         const std::map<position_type, kin_type> &setup_1,
         std::shared_ptr<Agent<board_type>> ag0,
         std::shared_ptr<Agent<board_type>> ag1,
         bool fixed_setups = false);

    template<size_t dim>
    Game(const std::array<int, dim> &shape,
         const std::array<int, dim> &board_starts,
         const std::map<position_type, sptr_piece_type> &setup0,
         const std::map<position_type, sptr_piece_type> &setup1,
         std::shared_ptr<Agent<board_type>> ag0,
         std::shared_ptr<Agent<board_type>> ag1,
         bool fixed_setups = false);

    template<size_t dim>
    Game(const std::array<int, dim> &shape,
         const std::array<int, dim> &board_starts,
         std::shared_ptr<Agent<board_type>> ag0,
         std::shared_ptr<Agent<board_type>> ag1,
         bool fixed_setups = false);

    void reset();

    int run_game(bool show);

    int run_step();

    void set_flag_fixed_setups(bool f_s) { m_fixed_setups = f_s; }

    void set_setup(const std::vector<sptr_piece_type> &setup, int team) {
        m_setups[team] = extract_pieces_from_setup(setup);
    }

    std::shared_ptr<Agent<board_type>> get_agent_0() { return m_agent_0; }

    std::shared_ptr<Agent<board_type>> get_agent_1() { return m_agent_1; }

    const state_type * get_gamestate() const { return &m_game_state; }

    std::map<position_type, sptr_piece_type> draw_setup(int team) {
        return static_cast<Derived*>(this)->draw_setup(team);
    }
};


template<class GameState, class Derived>
Game<GameState, Derived>::Game(board_type &&board,
                               std::shared_ptr<Agent<board_type>> ag0,
                               std::shared_ptr<Agent<board_type>> ag1,
                               bool fixed_setups,
                               int move_count)
        : m_game_state(board, move_count),
          m_agent_0(ag0),
          m_agent_1(ag1),
          m_setups{extract_pieces_from_setup(m_game_state.get_board()->get_setup(0)),
                   extract_pieces_from_setup(m_game_state.get_board()->get_setup(1))},
          m_fixed_setups(fixed_setups) {}


template<class GameState, class Derived>
template<size_t dim>
Game<GameState, Derived>::Game(const std::array<int, dim> &shape,
                               const std::array<int, dim> &board_starts,
                               std::shared_ptr<Agent<board_type>> ag0,
                               std::shared_ptr<Agent<board_type>> ag1,
                               bool fixed_setups)
        : Game(board_type(shape,
                          board_starts,
                          draw_setup(0),
                          draw_setup(1)),
               ag0,
               ag1,
               fixed_setups) {}


template<class GameState, class Derived>
Game<GameState, Derived>::Game(const board_type &board,
                               std::shared_ptr<Agent<board_type>> ag0,
                               std::shared_ptr<Agent<board_type>> ag1,
                               bool fixed_setups,
                               int move_count)
        : Game(board_type(board),
               ag0,
               ag1,
               fixed_setups,
               move_count) {}


template<class GameState, class Derived>
template<size_t dim>
Game<GameState, Derived>::Game(const std::array<int, dim> &shape,
                               const std::array<int, dim> &board_starts,
                               const std::map<position_type, kin_type> &setup_0,
                               const std::map<position_type, kin_type> &setup_1,
                               std::shared_ptr<Agent<board_type>> ag0,
                               std::shared_ptr<Agent<board_type>> ag1,
                               bool fixed_setups)
        : Game(board_type(shape,
                          board_starts,
                          setup_0,
                          setup_1),
               ag0,
               ag1,
               fixed_setups) {}


template<class GameState, class Derived>
std::vector<typename Game<GameState, Derived>::piece_type>
Game<GameState, Derived>::extract_pieces_from_setup(const std::vector<sptr_piece_type> &setup) {
    std::vector<piece_type> pc_vec(setup.size());
    std::transform(
            setup.begin(),
            setup.end(),
            pc_vec.begin(),
            [](const sptr_piece_type &piece_ptr) -> piece_type { return *piece_ptr; }
    );
    return pc_vec;
}


template<class GameState, class Derived>
std::vector<typename Game<GameState, Derived>::piece_type>
Game<GameState, Derived>::extract_pieces_from_setup(const std::map<position_type, kin_type> &setup,
                                                    int team) {
    using val_type = typename std::map<position_type, kin_type>::value_type;
    std::vector<piece_type> pc_vec(setup.size());
    std::transform(
            setup.begin(),
            setup.end(),
            pc_vec.begin(),
            [&](const val_type &pos_kin) -> sptr_piece_type {
                return std::make_shared<piece_type>(pos_kin.first, pos_kin.second, team);
            }
    );
}


template<class GameState, class Derived>
void Game<GameState, Derived>::reset() {
    auto curr_board_ptr = m_game_state->get_board();
    auto &setup0 = m_setups[0];
    auto &setup1 = m_setups[1];
    if (!m_fixed_setups) {
        auto &setup0 = draw_setup(0);
        auto &setup1 = draw_setup(1);
    }
    m_game_state = state_type(
            board_type(curr_board_ptr->get_shape(),
                       curr_board_ptr->get_starts(),
                       setup0,
                       setup1)
    );
}


template<class GameState, class Derived>
int Game<GameState, Derived>::run_step() {
    size_t turn = (m_game_state.get_move_count() + 1) % 2;
    int outcome;
    if (turn) {
        outcome = m_game_state->do_move(m_agent_1->decide_move());
    } else {
        outcome = m_game_state->do_move(m_agent_0->decide_move());
    }
    return outcome;
}


template<class GameState, class Derived>
int Game<GameState, Derived>::run_game(bool show) {

    while (true) {
        if (show)
            std::cout << m_game_state->get_board->print_board();

        // test for terminal
        int outcome = m_game_state.is_terminal();

        std::cout << "Status: " << outcome << std::endl;

        if (outcome != 404)
            break;
        else
            outcome = run_step();

        return outcome;
    }
};