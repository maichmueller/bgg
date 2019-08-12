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

// forward declare class and operators in conjunction with number types
template <typename len_t, int N>
struct Position;

template <typename number_t, typename len_t, int N>
Position<len_t, N> operator*(number_t n, Position<len_t, N> pos);

template <typename number_t, typename len_t, int N>
Position<len_t, N> operator/(number_t n, Position<len_t, N> pos);

// actual class definition
template <typename len_t, int N>
class Position {
    std::array<len_t, N> coordinates;
public:
    template <typename ...TT>
    explicit Position(TT ...args) : coordinates(args...) {}
    explicit Position(std::array<len_t, N> coords) : coordinates(std::move(coords)) {};
    explicit Position(std::vector<len_t> coords) : coordinates() {
        if(coords.size() > N)
            throw std::out_of_range("Vector passed to array is longer than Position dimension " + std::to_string(N));
        for (int i = 0; i < N; ++i) {
            coordinates[i] = coords[i];
        }
    };

    len_t & operator[](unsigned int index) {len_t * val_ptr = coordinates[index]; return val_ptr;}

    Position<len_t, N> operator+(const Position<len_t, N> & pos) const {
        Position<len_t, N> p(*this);
        for(int i = 0; i < N; ++i) {
            p[i] += pos[i];
        }
    }
    Position<len_t, N> operator-(const Position<len_t, N> &  pos) const {return *this + (-1*pos);}
    Position<len_t, N> operator*(const Position<len_t, N> & pos) const {
        Position<len_t, N> p(*this);
        for(int i = 0; i < coordinates.size(); ++i) {
            p[i] *= pos[i];
        }
    }
    Position<len_t, N> operator/(const Position<len_t, N> & pos) const {return *this * (1/pos);}

    template <typename number_t>
    Position<len_t, N> operator+(const number_t & n) const {
        Position<len_t, N> p(*this);
        for(int i = 0; i < N; ++i) {
            p[i] += n;
        }
    }
    template <typename number_t>
    Position<len_t, N> operator-(const number_t & n) const {return *this + (-n);}
    template <typename number_t>
    Position<len_t, N> operator*(const number_t & n) const {
        Position<len_t, N> p(*this);
        for(int i = 0; i < N; ++i) {
            p[i] *= n;
        }
    }
    template <typename number_t>
    Position<len_t, N> operator/(const number_t & n) const {return *this * (1/n);}
};

template <typename number_t, typename len_t, int N>
Position<len_t, N> operator*(const number_t & n, const Position<len_t, N> & pos) {return pos * n;}

template <typename number_t, typename len_t, int N>
Position<len_t, N> operator/(const number_t & n, const Position<len_t, N> & pos) {
    Position<len_t, N> p(pos);
    for(int i = 0; i < N; ++i) {
        p[i] /= n;
    }
}

template <typename len_t, int N>
class Move {
    Position<len_t, N> pos_before;
    Position<len_t, N> pos_after;
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