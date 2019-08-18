//
// Created by michael on 13.08.19.
//

#ifndef STRATEGO_CPP_MOVE_H
#define STRATEGO_CPP_MOVE_H

#include <vector>
#include "array"

#include "Position.h"

// forward declare class and operators in conjunction with number types
template <typename Length, int N>
class Move;

template <typename Number, typename Length, int N>
Move<Length, N> operator/(Number n, Move<Length, N> pos);

template <typename Length, int N>
class Move {
    using MoveContainer = std::array<Position<Length, N>, 2>;
    using Iterator = typename MoveContainer::iterator;
    using ConstIterator = typename MoveContainer::const_iterator;
    MoveContainer from_to;
public:

    const Position<Length, N> & operator[](unsigned int index) const {return from_to[index];}
    Position<Length, N> & operator[](unsigned int index) {return from_to[index];}

    Iterator begin() {return from_to.begin();}
    ConstIterator begin() const {return from_to.begin();}
    Iterator end() {return from_to.end();}
    ConstIterator end() const {return from_to.end();}

    auto get_positions() {return from_to;}

    Move<Length, N> operator+(const Move<Length, N> & other_move) {
        return Move<Length, N>{this[0] + other_move[0], this[1] + other_move[1]};
    }
    Move<Length, N> operator*(const Move<Length, N> & other_move) {
        return Move<Length, N>{this[0] * other_move[1], this[1] * other_move[1]};
    }
    template <typename Number>
    Move<Length, N> operator+(const Number & n) {
        return Move<Length, N>{this[0] * n, this[1] * n};
    }
    Move<Length, N> operator-(const Move<Length, N> & other_move) {
        return *this + (-1*other_move);
    }
    Move<Length, N> operator/(const Move<Length, N> & other_move) {
        return *this * (1/other_move);
    }
};

template <typename Number, typename Length, int N>
Move<Length, N> operator/(const Number & n, const Move<Length, N> & pos) {
    Move<Length, N> m(pos);
    m[0] = 1/m[0];
    m[1] = 1/m[1];
    return m;
}


#endif //STRATEGO_CPP_MOVE_H
