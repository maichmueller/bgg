//
// Created by michael on 13.08.19.
//

#pragma once

#include <vector>
#include <string>
#include "array"
#include <sstream>

// forward declare class and operators in conjunction with number types
template <typename Length, int N>
class Position;

template <typename Number, typename Length, int N>
Position<Length, N> operator*(const Number & n, const Position<Length, N> & pos);

template <typename Number, typename Length, int N>
Position<Length, N> operator/(const Number & n, const Position<Length, N> & pos);

// actual class definition
template <typename LengthType, int N>
class Position {
    using CoordType = std::array<LengthType, N>;
    using Iterator = typename CoordType::iterator;
    using ConstIterator = typename CoordType::const_iterator;
    CoordType coordinates;
    static constexpr int dim = N;
public:
    template <typename ...TT>
    explicit Position(TT ...args) : coordinates(args...) {}
    explicit Position(CoordType coords) : coordinates(std::move(coords)) {};
    explicit Position(CoordType & coords) : coordinates() {
        for (int i = 0; i < N; ++i) {
            coordinates[i] = coords[i];
        }
    };

    const LengthType & operator[](unsigned int index) const {return coordinates[index];}
    LengthType & operator[](unsigned int index) {return coordinates[index];}

    Iterator begin() {return coordinates.begin();}
    ConstIterator begin() const {return coordinates.begin();}
    Iterator end() {return coordinates.end();}
    ConstIterator end() const {return coordinates.end();}

    Position<LengthType, N> operator+(const Position<LengthType, N> & pos) const;
    Position<LengthType, N> operator-(const Position<LengthType, N> &  pos) const;
    Position<LengthType, N> operator*(const Position<LengthType, N> & pos) const;
    Position<LengthType, N> operator/(const Position<LengthType, N> & pos) const;
    template <typename Number>
    Position<LengthType, N> operator+(const Number & n) const;
    template <typename Number>
    Position<LengthType, N> operator-(const Number & n) const;
    template <typename Number>
    Position<LengthType, N> operator*(const Number & n) const;
    template <typename Number>
    Position<LengthType, N> operator/(const Number & n) const;

    bool operator==(const Position & other) const;
    bool operator!=(const Position & other) const;
    bool operator<(const Position & other) const;
    bool operator>(const Position & other) const;

    std::string to_string();
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

template <typename LengthType, int N>
Position<LengthType, N> Position<LengthType, N>::operator+(const Position<LengthType, N> & pos) const {
    Position<LengthType, N> p(*this);
    for(int i = 0; i < N; ++i) {
        p[i] += pos[i];
    }
}

template <typename LengthType, int N>
Position<LengthType, N> Position<LengthType, N>::operator-(const Position<LengthType, N> &  pos) const {return *this + (-1 * pos);}

template <typename LengthType, int N>
Position<LengthType, N> Position<LengthType, N>::operator*(const Position<LengthType, N> & pos) const {
    Position<LengthType, N> p(*this);
    for(int i = 0; i < coordinates.size(); ++i) {
        p[i] *= pos[i];
    }
}

template <typename LengthType, int N>
Position<LengthType, N> Position<LengthType, N>::operator/(const Position<LengthType, N> & pos) const {return *this * (1 / pos);}

template <typename LengthType, int N>
template <typename Number>
Position<LengthType, N> Position<LengthType, N>::operator+(const Number & n) const {
    Position<LengthType, N> p(*this);
    for(int i = 0; i < N; ++i) {
        p[i] += n;
    }
}
template <typename LengthType, int N>
template <typename Number>
Position<LengthType, N> Position<LengthType, N>::operator-(const Number & n) const {return *this + (-n);}

template <typename LengthType, int N>
template <typename Number>
Position<LengthType, N> Position<LengthType, N>::operator*(const Number & n) const {
    Position<LengthType, N> p(*this);
    for(int i = 0; i < N; ++i) {
        p[i] *= n;
    }
}
template <typename LengthType, int N>
template <typename Number>
Position<LengthType, N> Position<LengthType, N>::operator/(const Number & n) const {return *this * (1 / n);}

template <typename LengthType, int N>
bool Position<LengthType, N>::operator==(const Position & other) const {
    for(int i = 0; i < N; ++i) {
        if((*this)[i] != other[i])
            return false;
    }
    return true;
}
template <typename LengthType, int N>
bool Position<LengthType, N>::operator!=(const Position & other) const {
    return !(*this == other);
}
template <typename LengthType, int N>
bool Position<LengthType, N>::operator<(const Position & other) const {
    for(int i = 0; i < N; ++i) {
        if((*this)[i] < other[i])
            return true;
        else if((*this)[i] > other[i])
            return false;
        // the else case is the == case for which we simply continue onto the next dimension
    }
    return false;
}
template <typename LengthType, int N>
bool Position<LengthType, N>::operator>(const Position & other) const {
    for(int i = 0; i < N; ++i) {
        if((*this)[i] > other[i])
            return true;
        else if((*this)[i] < other[i])
            return false;
        // the else case is the == case for which we simply continue onto the next dimension
    }
    return false;
}
template <typename LengthType, int N>
std::string Position<LengthType, N>::to_string() {
    std::stringstream ss;
    ss << "(";
    for(int i = 0; i < N-1; ++i) {
        ss << std::to_string(coordinates[i]) << ", ";
    }
    ss << std::to_string(*coordinates.back()) << ")";
    return ss.str();
}