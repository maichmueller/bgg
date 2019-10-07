//
// Created by Michael on 22/02/2019.
//

#pragma once

#include <numeric>
#include "array"
#include "vector"
#include "map"
#include "unordered_map"
#include "algorithm"
#include "memory"

#include "../utils/utils.h"

template <typename Kin, typename Position>
struct GameUtilsStratego {

    std::vector<Kin> available_types;
    std::vector<Position> valid_start_positions;

    constexpr inline std::vector<Position> get_obstacle_positions(int game_len) {
        if (game_len == 5)
            return {{2, 2}};
        else if (game_len == 7)
            return {{3, 1}, {3, 5}};
        else if (game_len == 10)
            return {{4, 2}, {5, 2}, {4, 3}, {5, 3}, {4, 6}, {5, 6}, {4, 7}, {5, 7}};
        else
            throw std::invalid_argument("'game_len' not in [5, 7, 10].");
    }

    inline std::vector<int> get_available_types(int game_len) {
        if (game_len == 5)
            return  {0, 1, 2, 2, 2, 3, 3, 10, 11, 11};
        else if (game_len == 7)
            return {0, 1, 2, 2, 2, 2, 2, 3, 3, 3, 4,
                    4, 4, 5, 5, 6, 10, 11, 11, 11, 11};
        else if (game_len == 10)
            return  {0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3,
                     3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5,
                     6, 6, 6, 6, 7, 7, 7, 8, 8, 9, 10,
                     11, 11, 11, 11, 11, 11};
        else
            throw std::invalid_argument("'game_len' not in [5, 7, 10].");
    }

    inline std::vector<Position> get_start_positions(int game_len, int team) {
        if(team != 0 && team != 1)
            throw std::invalid_argument("'team' not in {0, 1}.");

        if (game_len == 5) {
            if (team == 0)
                return  {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}};
            else
                return {{4, 0}, {4, 1}, {4, 2}, {4, 3}, {4, 4}, {3, 0}, {3, 1}, {3, 2}, {3, 3}, {3, 4}};
        } else if (game_len == 7) {
            if (team == 0)
                return {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6},
                        {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6},
                        {2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}, {2, 5}, {2, 6}};
            else
                return {{4, 0}, {4, 1}, {4, 2}, {4, 3}, {4, 4}, {4, 5}, {4, 6},
                        {5, 0}, {5, 1}, {5, 2}, {5, 3}, {5, 4}, {5, 5}, {5, 6},
                        {6, 0}, {6, 1}, {6, 2}, {6, 3}, {6, 4}, {6, 5}, {6, 6}};

        } else if (game_len == 10) {
            if (team == 0)
                return {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}, {0, 8}, {0, 9},
                        {1, 0}, {1, 1}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6}, {1, 7}, {1, 8}, {1, 9},
                        {2, 0}, {2, 1}, {2, 2}, {2, 3}, {2, 4}, {2, 5}, {2, 6}, {2, 7}, {2, 8}, {2, 9},
                        {3, 0}, {3, 1}, {3, 2}, {3, 3}, {3, 4}, {3, 5}, {3, 6}, {3, 7}, {3, 8}, {3, 9}};

            else
                return {{6, 0}, {6, 1}, {6, 2}, {6, 3}, {6, 4}, {6, 5}, {6, 6}, {6, 7}, {6, 8}, {6, 9},
                        {7, 0}, {7, 1}, {7, 2}, {7, 3}, {7, 4}, {7, 5}, {7, 6}, {7, 7}, {7, 8}, {7, 9},
                        {8, 0}, {8, 1}, {8, 2}, {8, 3}, {8, 4}, {8, 5}, {8, 6}, {8, 7}, {8, 8}, {8, 9},
                        {9, 0}, {9, 1}, {9, 2}, {9, 3}, {9, 4}, {9, 5}, {9, 6}, {9, 7}, {9, 8}, {9, 9}};

        } else
            throw std::invalid_argument("'game_len' not in {5, 7, 10}.");
    }
};
