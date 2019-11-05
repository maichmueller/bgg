//
// Created by michael on 06.10.19.
//


#pragma once

#include "game.h"


class GameStratego : public Game<GameStateStratego, GameStratego> {
public:
    using base_type = Game<GameStateStratego, GameStratego>;
    using base_type::base_type;

    std::map<position_type, sptr_piece_type> draw_setup(int team) {
        int shape = m_game_state.get_board()->get_shape()[0];
            auto avail_types = LogicStratego<board_type>::get_available_types(shape);

            std::vector<position_type> poss_pos = LogicStratego<board_type>::get_start_positions(shape, team);

            std::map<position_type, sptr_piece_type> setup_out;

            std::random_device rd;
            std::mt19937 rng(rd());
            std::shuffle(poss_pos.begin(), poss_pos.end(), rng);
            std::shuffle(avail_types.begin(), avail_types.end(), rng);

            auto counter = utils::counter(avail_types);

            while(!poss_pos.empty()) {
                auto& pos = poss_pos.back();
                auto& type = avail_types.back();

                setup_out[pos] = std::make_shared<piece_type >(pos,
                                                               typename piece_type::kin_type(type, counter[type] - 1),
                                                               team);

                poss_pos.pop_back();
                avail_types.pop_back();
            }
            return setup_out;
    }
};