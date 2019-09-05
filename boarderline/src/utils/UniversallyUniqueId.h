//
// Created by michael on 04.09.19.
//

#pragma once

#include <numeric>
#include <random>
#include <array>


struct UUID {
    static std::vector<bool> used_indices{false};
    int last_used_idx;

    static constexpr int get_unique_id() {
        auto it = used_indices.begin();
        int id = 0;
        for(; it != used_indices.end(); ++it, ++id) {
            if(!(*it)) {
                // this id is yet unused and can be returned
                break
            }
        }
        *it = true;
        nr_used_indices += 1;
        return id;
    }
};

