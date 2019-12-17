//
// Created by Michael on 06/03/2019.
//

#pragma once


#include <random>
#include <algorithm>
#include <iterator>
#include <functional>

#include "State.h"
#include "azpp/agent/Agent.h"
#include "azpp/utils/utils.h"


template<class StateType, class LogicType, class Derived>
class Game {
public:
    using state_type = StateType;
    using logic_type = LogicType;
    using piece_type = typename state_type::piece_type;
    using kin_type = typename state_type::kin_type;
    using position_type = typename state_type::position_type;
    using move_type = typename state_type::move_type;
    using board_type = typename state_type::board_type;
    using agent_type = Agent<state_type>;

    using sptr_piece_type = std::shared_ptr<piece_type>;

protected:

    state_type m_game_state;

    std::array<std::shared_ptr<agent_type>, 2> m_agents;

    std::array<std::vector<piece_type>, 2> m_setups;

    bool m_fixed_setups;

    std::vector<piece_type> extract_pieces_from_setup(const std::vector<sptr_piece_type> &setup);

    std::vector<piece_type> extract_pieces_from_setup(const std::map<position_type, kin_type> &setup, int team);

public:

    Game(board_type &&board,
         std::shared_ptr<agent_type> ag0,
         std::shared_ptr<agent_type> ag1,
         bool fixed_setups = false,
         int move_count = 0);

    Game(const board_type &board,
         std::shared_ptr<agent_type> ag0,
         std::shared_ptr<Agent<state_type>> ag1,
         bool fixed_setups = false,
         int move_count = 0);

    template<size_t dim>
    Game(const std::array<int, dim> &shape,
         const std::array<int, dim> &board_starts,
         const std::map<position_type, kin_type> &setup_0,
         const std::map<position_type, kin_type> &setup_1,
         std::shared_ptr<agent_type> ag0,
         std::shared_ptr<agent_type> ag1,
         bool fixed_setups = false);

    template<size_t dim>
    Game(const std::array<int, dim> &shape,
         const std::array<int, dim> &board_starts,
         const std::map<position_type, sptr_piece_type> &setup0,
         const std::map<position_type, sptr_piece_type> &setup1,
         std::shared_ptr<agent_type> ag0,
         std::shared_ptr<agent_type> ag1,
         bool fixed_setups = false);

    template<size_t dim>
    Game(const std::array<int, dim> &shape,
         const std::array<int, dim> &board_starts,
         std::shared_ptr<agent_type> ag0,
         std::shared_ptr<agent_type> ag1,
         bool fixed_setups = false);

    Game(const state_type &state,
         std::shared_ptr<agent_type> ag0,
         std::shared_ptr<agent_type> ag1,
         bool fixed_setups = false);


    Game(state_type &&state,
         std::shared_ptr<agent_type> ag0,
         std::shared_ptr<agent_type> ag1,
         bool fixed_setups = false);

    void reset();

    int run_game(bool show);

    int run_step();

    void set_flag_fixed_setups(bool f_s) { m_fixed_setups = f_s; }

    void set_setup(const std::vector<sptr_piece_type> &setup, int team) {
        m_setups[team] = extract_pieces_from_setup(setup);
    }

    std::shared_ptr<agent_type> get_agents() { return m_agents; }

    std::shared_ptr<agent_type> get_agent_0() { return m_agents[0]; }

    std::shared_ptr<agent_type> get_agent_1() { return m_agents[1]; }

    const state_type *get_gamestate() const { return &m_game_state; }

    virtual std::map<position_type, sptr_piece_type> draw_setup(int team) {
        return static_cast<Derived *>(this)->draw_setup(team);
    }
};


template<class StateType, class LogicType, class Derived>
Game<StateType, LogicType, Derived>::Game(
        board_type &&board,
        std::shared_ptr<agent_type> ag0,
        std::shared_ptr<agent_type> ag1,
        bool fixed_setups,
        int move_count)
        : m_game_state(board, move_count),
          m_agents{ag0, ag1},
          m_setups{extract_pieces_from_setup(m_game_state.get_board()->get_setup(0)),
                   extract_pieces_from_setup(m_game_state.get_board()->get_setup(1))},
          m_fixed_setups(fixed_setups) {}


template<class StateType, class LogicType, class Derived>
template<size_t dim>
Game<StateType, LogicType, Derived>::Game(const std::array<int, dim> &shape,
                                          const std::array<int, dim> &board_starts,
                                          std::shared_ptr<agent_type> ag0,
                                          std::shared_ptr<agent_type> ag1,
                                          bool fixed_setups)
        : Game(board_type(shape,
                          board_starts,
                          draw_setup(0),
                          draw_setup(1)),
               ag0,
               ag1,
               fixed_setups) {}


template<class StateType, class LogicType, class Derived>
Game<StateType, LogicType, Derived>::Game(const board_type &board,
                                          std::shared_ptr<agent_type> ag0,
                                          std::shared_ptr<agent_type> ag1,
                                          bool fixed_setups,
                                          int move_count)
        : Game(board_type(board),
               ag0,
               ag1,
               fixed_setups,
               move_count) {}


template<class StateType, class LogicType, class Derived>
template<size_t dim>
Game<StateType, LogicType, Derived>::Game(
        const std::array<int, dim> &shape,
        const std::array<int, dim> &board_starts,
        const std::map<position_type, kin_type> &setup_0,
        const std::map<position_type, kin_type> &setup_1,
        std::shared_ptr<agent_type> ag0,
        std::shared_ptr<agent_type> ag1,
        bool fixed_setups)
        :
        Game(board_type(shape,
                        board_starts,
                        setup_0,
                        setup_1),
             ag0,
             ag1,
             fixed_setups) {}


template<class StateType, class LogicType, class Derived>
Game<StateType, LogicType, Derived>::Game(
        const state_type &state,
        std::shared_ptr<agent_type> ag0,
        std::shared_ptr<agent_type> ag1,
        bool fixed_setups)
        :
        m_game_state(state),
        m_agents{ag0, ag1},
        m_fixed_setups(fixed_setups) {}


template<class StateType, class LogicType, class Derived>
Game<StateType, LogicType, Derived>::Game(
        state_type && state,
        std::shared_ptr<agent_type> ag0,
        std::shared_ptr<agent_type> ag1,
        bool fixed_setups)
        :
        m_game_state(std::move(state)),
        m_agents{ag0, ag1},
        m_fixed_setups(fixed_setups) {}


template<class StateType, class LogicType, class Derived>
std::vector<typename Game<StateType, LogicType, Derived>::piece_type>
Game<StateType, LogicType, Derived>::extract_pieces_from_setup(const std::vector<sptr_piece_type> &setup) {
    std::vector<piece_type> pc_vec(setup.size());
    std::transform(
            setup.begin(),
            setup.end(),
            pc_vec.begin(),
            [](const sptr_piece_type &piece_ptr) -> piece_type { return *piece_ptr; }
    );
    return pc_vec;
}


template<class StateType, class LogicType, class Derived>
std::vector<typename Game<StateType, LogicType, Derived>::piece_type>
Game<StateType, LogicType, Derived>::extract_pieces_from_setup(const std::map<position_type, kin_type> &setup,
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


template<class StateType, class LogicType, class Derived>
void Game<StateType, LogicType, Derived>::reset() {
    auto curr_board_ptr = m_game_state.get_board();
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


template<class StateType, class LogicType, class Derived>
int Game<StateType, LogicType, Derived>::run_step() {
    size_t turn = (m_game_state.get_move_count() + 1) % 2;
    int outcome = m_game_state.do_move(
            m_agents[turn]->decide_move(
                    m_game_state,
                    logic_type::get_legal_moves(*m_game_state.get_board(), turn)
            )
    );
    return outcome;
}


template<class StateType, class LogicType, class Derived>
int Game<StateType, LogicType, Derived>::run_game(bool show) {

    while (true) {
        if (show)
            std::cout << m_game_state.get_board()->print_board(false, true);

        // test for game end status
        int outcome = m_game_state.is_terminal();

        std::cout << "Status: " << outcome << std::endl;

        if (outcome != 404)
            return outcome;
        else
            run_step();
    }
}
