//
// Created by Michael on 22/02/2019.
//

#ifndef STRATEGO_CPP_GAMEDECLARATIONS_H
#define STRATEGO_CPP_GAMEDECLARATIONS_H

#include <numeric>
#include "array"
#include "vector"
#include "map"
#include "unordered_map"
#include "algorithm"
#include "memory"

#include "utils.h"


template <typename len_t, int N>
struct Position {
    std::array<len_t, N> coordinates;
    template <typename ...TT>
    Position(TT args) : m_x(x), m_y(y) {}
    len_t operator[](unsigned int index) {
        if(index > N-1) throw std::out_of_range("Index" + std::to_string(index) +
                                              " out of bounds (has to be <= 0 index <= 1).");
        return coordinates[index];
    }
    Position<len_t> operator+(Position<len_t> pos) {return Position<len_t>(m_x + pos[0], m_y + pos[1]);}
    Position<len_t> operator-(Position<len_t> pos) {return Position<len_t>(m_x - pos[0], m_y - pos[1]);}
    Position<len_t> operator*(Position<len_t> pos) {return Position<len_t>(m_x * pos[0], m_y * pos[1]);}
    Position<len_t> operator/(Position<len_t> pos) {return Position<len_t>(m_x / pos[0], m_y / pos[1]);}
    template <typename number_t>
    Position<len_t> operator+(number_t n) {return Position<len_t>(n + this->x, n + this->y);}
    template <typename number_t>
    Position<len_t> operator-(number_t n) {return *this + (-n);}
};

template <typename number_t, typename len_t>
Position<len_t> operator*(number_t n, Position<len_t> pos) {return Position<len_t>(n * pos[0], n * pos[1]);}
template <typename number_t, typename len_t>
Position<len_t> operator*(Position<len_t> pos, number_t n) {return n * pos;}
template <typename number_t, typename len_t>
Position<len_t> operator/(Position<len_t> pos, number_t n) {return 1/n * pos;}

template <typename len_t>
class Move {
    Position<len_t> x;
    len_t y;
public:

};


namespace GameDeclarations {
    // should be of length 2 only!
    using strat_pos_t = std::array<int, 2>;
    using strat_move_base_t = strat_pos_t;
    using strat_move_t = std::array<strat_pos_t, 2>;

    const std::vector<int> available_types_s = {0, 1, 2, 2, 2, 3, 3, 10, 11, 11};
    const std::vector<int> available_types_m = {0, 1, 2, 2, 2, 2, 2, 3, 3, 3, 4,
                                                4, 4, 5, 5, 6, 10, 11, 11, 11, 11};
    const std::vector<int> available_types_l = {0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3,
                                                3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5,
                                                6, 6, 6, 6, 7, 7, 7, 8, 8, 9, 10,
                                                11, 11, 11, 11, 11, 11};

    const std::vector<strat_pos_t> start_pos_0_s = {{0, 0},
                                                    {0, 1},
                                                    {0, 2},
                                                    {0, 3},
                                                    {0, 4},
                                                    {1, 0},
                                                    {1, 1},
                                                    {1, 2},
                                                    {1, 3},
                                                    {1, 4}};
    const std::vector<strat_pos_t> start_pos_1_s = {{4, 0},
                                                    {4, 1},
                                                    {4, 2},
                                                    {4, 3},
                                                    {4, 4},
                                                    {3, 0},
                                                    {3, 1},
                                                    {3, 2},
                                                    {3, 3},
                                                    {3, 4}};

    const std::vector<strat_pos_t> start_pos_0_m = {{0, 0},
                                                    {0, 1},
                                                    {0, 2},
                                                    {0, 3},
                                                    {0, 4},
                                                    {0, 5},
                                                    {0, 6},
                                                    {1, 0},
                                                    {1, 1},
                                                    {1, 2},
                                                    {1, 3},
                                                    {1, 4},
                                                    {1, 5},
                                                    {1, 6},
                                                    {2, 0},
                                                    {2, 1},
                                                    {2, 2},
                                                    {2, 3},
                                                    {2, 4},
                                                    {2, 5},
                                                    {2, 6}};
    const std::vector<strat_pos_t> start_pos_1_m = {{4, 0},
                                                    {4, 1},
                                                    {4, 2},
                                                    {4, 3},
                                                    {4, 4},
                                                    {4, 5},
                                                    {4, 6},
                                                    {5, 0},
                                                    {5, 1},
                                                    {5, 2},
                                                    {5, 3},
                                                    {5, 4},
                                                    {5, 5},
                                                    {5, 6},
                                                    {6, 0},
                                                    {6, 1},
                                                    {6, 2},
                                                    {6, 3},
                                                    {6, 4},
                                                    {6, 5},
                                                    {6, 6}};

    const std::vector<strat_pos_t> start_pos_0_l = {{0, 0},
                                                    {0, 1},
                                                    {0, 2},
                                                    {0, 3},
                                                    {0, 4},
                                                    {0, 5},
                                                    {0, 6},
                                                    {0, 7},
                                                    {0, 8},
                                                    {0, 9},
                                                    {1, 0},
                                                    {1, 1},
                                                    {1, 2},
                                                    {1, 3},
                                                    {1, 4},
                                                    {1, 5},
                                                    {1, 6},
                                                    {1, 7},
                                                    {1, 8},
                                                    {1, 9},
                                                    {2, 0},
                                                    {2, 1},
                                                    {2, 2},
                                                    {2, 3},
                                                    {2, 4},
                                                    {2, 5},
                                                    {2, 6},
                                                    {2, 7},
                                                    {2, 8},
                                                    {2, 9},
                                                    {3, 0},
                                                    {3, 1},
                                                    {3, 2},
                                                    {3, 3},
                                                    {3, 4},
                                                    {3, 5},
                                                    {3, 6},
                                                    {3, 7},
                                                    {3, 8},
                                                    {3, 9}};

    const std::vector<strat_pos_t> start_pos_1_l = {{6, 0},
                                                    {6, 1},
                                                    {6, 2},
                                                    {6, 3},
                                                    {6, 4},
                                                    {6, 5},
                                                    {6, 6},
                                                    {6, 7},
                                                    {6, 8},
                                                    {6, 9},
                                                    {7, 0},
                                                    {7, 1},
                                                    {7, 2},
                                                    {7, 3},
                                                    {7, 4},
                                                    {7, 5},
                                                    {7, 6},
                                                    {7, 7},
                                                    {7, 8},
                                                    {7, 9},
                                                    {8, 0},
                                                    {8, 1},
                                                    {8, 2},
                                                    {8, 3},
                                                    {8, 4},
                                                    {8, 5},
                                                    {8, 6},
                                                    {8, 7},
                                                    {8, 8},
                                                    {8, 9},
                                                    {9, 0},
                                                    {9, 1},
                                                    {9, 2},
                                                    {9, 3},
                                                    {9, 4},
                                                    {9, 5},
                                                    {9, 6},
                                                    {9, 7},
                                                    {9, 8},
                                                    {9, 9}};


    const std::vector<strat_pos_t> obstacle_pos_s = {{2, 2}};
    const std::vector<strat_pos_t> obstacle_pos_m = {{3, 1},
                                                     {3, 5}};
    const std::vector<strat_pos_t> obstacle_pos_l = {{4, 2},
                                                     {5, 2},
                                                     {4, 3},
                                                     {5, 3},
                                                     {4, 6},
                                                     {5, 6},
                                                     {4, 7},
                                                     {5, 7}};

    constexpr inline auto const &get_obstacle_pos(int game_len) {
        if (game_len == 5)
            return obstacle_pos_s;
        else if (game_len == 7)
            return obstacle_pos_m;
        else if (game_len == 10)
            return obstacle_pos_l;
        else
            throw std::invalid_argument("'game_len' not in [5, 7, 10].");
    }

    constexpr inline auto const &get_available_types(int game_len) {
        if (game_len == 5)
            return available_types_s;
        else if (game_len == 7)
            return available_types_m;
        else if (game_len == 10)
            return available_types_l;
        else
            throw std::invalid_argument("'game_len' not in [5, 7, 10].");
    }

    constexpr inline auto const &get_start_positions(int game_len, int team) {
        if (game_len == 5) {
            if (team == 0)
                return start_pos_0_s;
            else if (team == 1)
                return start_pos_1_s;
            else
                throw std::invalid_argument("'team' not in [0, 1].");
        } else if (game_len == 7) {
            if (team == 0)
                return start_pos_0_m;
            else if (team == 1)
                return start_pos_1_m;
            else
                throw std::invalid_argument("'team' not in [0, 1].");
        } else if (game_len == 10) {
            if (team == 0)
                return start_pos_0_s;
            else if (team == 1)
                return start_pos_1_s;
            else
                throw std::invalid_argument("'team' not in [0, 1].");
        } else
            throw std::invalid_argument("'game_len' not in [5, 7, 10].");
    }
}

using namespace GameDeclarations;

struct key_comp {
    bool operator()(const strat_pos_t &lhs, const strat_pos_t &rhs) const {
        if (lhs[0] < rhs[0]) {
            return true;
        } else if (lhs[0] == rhs[0]) {
            if (lhs[1] < rhs[1]) {
                return true;
            } else {
                // on equivalence == or greater > case it has to return false
                return false;
            }
        } else {
            return false;
        }
    }
};



#endif //STRATEGO_CPP_GAMEDECLARATIONS_H