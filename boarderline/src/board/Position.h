//
// Created by michael on 13.08.19.
//

#ifndef STRATEGO_CPP_POSITION_H
#define STRATEGO_CPP_POSITION_H

#include <vector>
#include "array"

// forward declare class and operators in conjunction with number types
template <typename len_t, int N>
struct Position;

template <typename number_t, typename len_t, int N>
Position<len_t, N> operator*(const number_t & n, const Position<len_t, N> & pos);

template <typename number_t, typename len_t, int N>
Position<len_t, N> operator/(const number_t & n, const Position<len_t, N> & pos);

// actual class definition
template <typename len_t, int N>
class Position {
    std::array<len_t, N> coordinates;
public:
    template <typename ...TT>
    explicit Position(TT ...args) : coordinates(args...) {}
    explicit Position(std::array<len_t, N> coords) : coordinates(std::move(coords)) {};
    explicit Position(const std::vector<len_t> & coords) : coordinates() {
        if(coords.size() > N)
            throw std::out_of_range("Vector passed to array is longer than Position dimension " + std::to_string(N));
        for (int i = 0; i < N; ++i) {
            coordinates[i] = coords[i];
        }
    };

    const len_t & operator[](unsigned int index) const {return coordinates[index];}
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

#endif //STRATEGO_CPP_POSITION_H
