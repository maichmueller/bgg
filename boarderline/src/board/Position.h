//
// Created by michael on 13.08.19.
//

#ifndef STRATEGO_CPP_POSITION_H
#define STRATEGO_CPP_POSITION_H

#include <vector>
#include <string>
#include "array"

// forward declare class and operators in conjunction with number types
template <typename Length, int N>
class Position;

template <typename Number, typename Length, int N>
Position<Length, N> operator*(const Number & n, const Position<Length, N> & pos);

template <typename Number, typename Length, int N>
Position<Length, N> operator/(const Number & n, const Position<Length, N> & pos);

// actual class definition
template <typename Length, int N>
class Position {
    using CoordType = std::array<Length, N>;
    using Iterator = typename CoordType::iterator;
    using ConstIterator = typename CoordType::const_iterator;
    CoordType coordinates;
    static constexpr int dim = N;
public:
    template <typename ...TT>
    explicit Position(TT ...args) : coordinates(args...) {}
    explicit Position(CoordType coords) : coordinates(std::move(coords)) {};
    explicit Position(CoordType & coords) : coordinates() {
        if(coords.size() > N)
            throw std::out_of_range("Vector passed to array is longer than Position dimension " + std::to_string(N));
        for (int i = 0; i < N; ++i) {
            coordinates[i] = coords[i];
        }
    };

    const Length & operator[](unsigned int index) const {return coordinates[index];}
    Length & operator[](unsigned int index) {return coordinates[index];}

    Iterator begin() {return coordinates.begin();}
    ConstIterator begin() const {return coordinates.begin();}
    Iterator end() {return coordinates.end();}
    ConstIterator end() const {return coordinates.end();}

    Position<Length, N> operator+(const Position<Length, N> & pos) const {
        Position<Length, N> p(*this);
        for(int i = 0; i < N; ++i) {
            p[i] += pos[i];
        }
    }
    Position<Length, N> operator-(const Position<Length, N> &  pos) const {return *this + (-1*pos);}
    Position<Length, N> operator*(const Position<Length, N> & pos) const {
        Position<Length, N> p(*this);
        for(int i = 0; i < coordinates.size(); ++i) {
            p[i] *= pos[i];
        }
    }
    Position<Length, N> operator/(const Position<Length, N> & pos) const {return *this * (1/pos);}

    template <typename Number>
    Position<Length, N> operator+(const Number & n) const {
        Position<Length, N> p(*this);
        for(int i = 0; i < N; ++i) {
            p[i] += n;
        }
    }
    template <typename Number>
    Position<Length, N> operator-(const Number & n) const {return *this + (-n);}
    template <typename Number>
    Position<Length, N> operator*(const Number & n) const {
        Position<Length, N> p(*this);
        for(int i = 0; i < N; ++i) {
            p[i] *= n;
        }
    }
    template <typename Number>
    Position<Length, N> operator/(const Number & n) const {return *this * (1/n);}
};

template <typename Number, typename Length, int N>
Position<Length, N> operator*(const Number & n, const Position<Length, N> & pos) {return pos * n;}

template <typename Number, typename Length, int N>
Position<Length, N> operator/(const Number & n, const Position<Length, N> & pos) {
    Position<Length, N> p(pos);
    for(int i = 0; i < N; ++i) {
        p[i] /= n;
    }
}

#endif //STRATEGO_CPP_POSITION_H
