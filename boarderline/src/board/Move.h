//
// Created by michael on 13.08.19.
//

#ifndef STRATEGO_CPP_MOVE_H
#define STRATEGO_CPP_MOVE_H


#include <vector>
#include "array"

#include "Position.h"

// forward declare class and operators in conjunction with number types
template <typename len_t, int N>
class Move;

template <typename number_t, typename len_t, int N>
Move<len_t, N> operator/(number_t n, Move<len_t, N> pos);

template <typename len_t, int N>
class Move {
    std::array<Position<len_t, N>, 2> positions;
public:

    const len_t & operator[](unsigned int index) const {return positions[index];}
    Position<len_t, N> & operator[](unsigned int index) {return positions[index];}

    Move<len_t, N> operator+(const Move<len_t, N> & other_move) {
        return Move<len_t, N>{this[0] + other_move[0], this[1] + other_move[1]};
    }
    Move<len_t, N> operator*(const Move<len_t, N> & other_move) {
        return Move<len_t, N>{this[0] * other_move[1], this[1] * other_move[1]};
    }
    template <typename number_t>
    Move<len_t, N> operator+(const number_t & n) {
        return Move<len_t, N>{this[0] * n, this[1] * n};
    }
    Move<len_t, N> operator-(const Move<len_t, N> & other_move) {
        return *this + (-1*other_move);
    }
    Move<len_t, N> operator/(const Move<len_t, N> & other_move) {
        return *this * (1/other_move);
    }
};

template <typename number_t, typename len_t, int N>
Move<len_t, N> operator/(const number_t & n, const Move<len_t, N> & pos) {
    Move<len_t, N> m(pos);
    m[0] = 1/m[0];
    m[1] = 1/m[1];
    return m;
}


#endif //STRATEGO_CPP_MOVE_H
