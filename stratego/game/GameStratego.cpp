#include "GameStratego.h"


GameStratego::GameStratego(
        const std::array<size_t, 2> &shape,
        const std::map<position_type, int> &setup_0,
        const std::map<position_type, int> &setup_1,
        const std::shared_ptr<Agent<state_type>> & ag0,
        const std::shared_ptr<Agent<state_type>> & ag1,
        bool fixed_setups)
        :
        base_type(
                state_type(
                        shape,
                        setup_0,
                        setup_1),
                ag0,
                ag1,
                fixed_setups) {}

GameStratego::GameStratego(
        const std::array<size_t, 2> &shape,
        const std::map<position_type, kin_type > &setup_0,
        const std::map<position_type, kin_type > &setup_1,
        const std::shared_ptr<Agent<state_type>> & ag0,
        const std::shared_ptr<Agent<state_type>> & ag1,
        bool fixed_setups)
        :
        base_type(
                state_type(
                        shape,
                        setup_0,
                        setup_1),
                ag0,
                ag1,
                fixed_setups) {}


GameStratego::GameStratego(
        size_t shape,
        const std::map<position_type, int> &setup_0,
        const std::map<position_type, int> &setup_1,
        const std::shared_ptr<Agent<state_type>> & ag0,
        const std::shared_ptr<Agent<state_type>> & ag1,
        bool fixed_setups)
        :
        base_type(
                state_type(
                        {shape, shape},
                        setup_0,
                        setup_1),
                ag0,
                ag1,
                fixed_setups) {}

GameStratego::GameStratego(
        size_t shape,
        const std::map<position_type, kin_type > &setup_0,
        const std::map<position_type, kin_type > &setup_1,
        const std::shared_ptr<Agent<state_type>> & ag0,
        const std::shared_ptr<Agent<state_type>> & ag1,
        bool fixed_setups)
        :
        base_type(
                state_type(
                        {shape, shape},
                        setup_0,
                        setup_1),
                ag0,
                ag1,
                fixed_setups) {}


std::map<GameStratego::position_type, GameStratego::sptr_piece_type> GameStratego::draw_setup_(int team) {
    int shape = m_game_state.get_board()->get_shape()[0];
    auto avail_types = LogicStratego<board_type>::get_available_types(shape);

    std::vector<position_type> poss_pos = LogicStratego<board_type>::get_start_positions(shape, team);

    std::map<position_type, sptr_piece_type> setup_out;

    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(poss_pos.begin(), poss_pos.end(), rng);
    std::shuffle(avail_types.begin(), avail_types.end(), rng);

    auto counter = utils::counter(avail_types);

    while (!poss_pos.empty()) {
        auto &pos = poss_pos.back();
        auto &type = avail_types.back();

        setup_out[pos] = std::make_shared<piece_type>(pos,
                                                      typename piece_type::kin_type(type, counter[type] - 1),
                                                      team);

        poss_pos.pop_back();
        avail_types.pop_back();
    }
    return setup_out;
}

