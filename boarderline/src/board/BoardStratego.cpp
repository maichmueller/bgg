//
// Created by Michael on 08.09.19.
//

#include "BoardStratego.h"

std::vector<std::shared_ptr<BoardStratego::piece_type>> BoardStratego::adapt_setup(
        const std::map<position_type, int> &setup) {
    auto setup_unloader = [&](std::map<position_type, int> & setup) {
        std::map<position_type, int> seen_pos;
        std::map<int, int> version_count;
        for (auto &elem : setup) {
            position_type pos = elem.first;
            auto kin = elem.second;

            if (seen_pos.find(pos) != seen_pos.end()) {
                //element found
                throw std::invalid_argument("Parameter setup has more than one piece for the "
                                            "same position (position: '" + pos.to_string() + "').");
            }
            seen_pos[pos] = 1;
            // null constructor of map is called on unplaced previous item (creates 0 int)
            // therefore the first time this is called, will get us to version 0, the first
            // piece of its kind. Afterwards it will keep the count correctly for us.
            int version = version_count[kin]++;
            auto piece = std::make_shared<piece_type>(pos, kin_type{kin, version}, 0);
            m_board_map[pos] = std::move(piece);
        }
    };
    setup_unloader(setup_0);
    setup_unloader(setup_1);
}

