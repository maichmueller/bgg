//
// Created by michael on 13.08.19.
//

#pragma once

#include <vector>
#include "array"

#include "board/Position.h"

// forward declare class and operators in conjunction with number types
template <class Position>
class Move;

template <typename Number, typename Position>
Move<Position> operator/(Number n, Move<Position> pos);

template <class Position>
class Move {
public:
    using position_type = Position;
    using move_container = std::array<Position, 2>;
    using iterator = typename move_container::iterator;
    using const_iterator = typename move_container::const_iterator;
private:
    move_container from_to;
public:
    Move(const position_type & pos_from, const position_type & pos_to) : from_to{pos_from, pos_to} {}
    Move(position_type && pos_from, position_type && pos_to) : from_to{pos_from, pos_to} {}

    const Position & operator[](unsigned int index) const {return from_to[index];}
    Position & operator[](unsigned int index) {return from_to[index];}

    iterator begin() {return from_to.begin();}
    const_iterator begin() const {return from_to.begin();}
    iterator end() {return from_to.end();}
    const_iterator end() const {return from_to.end();}

    auto get_positions() const {return from_to;}

    Move operator+(const Move & other_move) {
        return Move{this[0] + other_move[0], this[1] + other_move[1]};
    }
    Move operator*(const Move & other_move) {
        return Move{this[0] * other_move[1], this[1] * other_move[1]};
    }
    template <typename Number>
    Move operator+(const Number & n) {
        return Move{this[0] * n, this[1] * n};
    }
    Move operator-(const Move & other_move) {
        return *this + (-1*other_move);
    }
    Move operator/(const Move & other_move) {
        return *this * (1/other_move);
    }
    bool operator==(const Move & other) const {
        return (*this)[0] == other[0] && (*this)[1] == other[1];
    }
    bool operator!=(const Move & other) const {
        return !(*this == other);
    }
    
    template <typename container>
    Move<Position> invert(const container & starts, const container & ends) {
        for(auto & pos : from_to) {
            pos = pos.invert(starts, ends);
        }
    };
};

template <typename Number, typename Position>
Move<Position> operator/(const Number & n, const Move<Position> & pos) {
    Move<Position> m(pos);
    m[0] = 1/m[0];
    m[1] = 1/m[1];
    return m;
}
