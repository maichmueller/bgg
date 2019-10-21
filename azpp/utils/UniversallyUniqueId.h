//
// Created by michael on 04.09.19.
//

#pragma once

#include <numeric>
#include <random>
#include <map>



struct UUID {
    static std::map<unsigned int, bool> used_indices;

    static unsigned int get_unique_id() {
        auto it = used_indices.begin();
        for(; it != used_indices.end(); ++it) {
            if(!(it->second)) {
                // this id is yet unused and can be returned
                break;
            }
        }
        it->second = true;
        return it->first;
    }
    static void free_id(unsigned int id) {
        used_indices[id] = false;
    }
};

