//
// Created by michael on 13.08.19.
//

#pragma once

#include <vector>
#include <string>
#include <array>
#include <sstream>

// forward declare class and operators in conjunction with number types
template <typename Length, size_t N>
class Position;

template <typename Number, typename Length, size_t N>
Position<Length, N> operator*(const Number & n, const Position<Length, N> & pos);

template <typename Number, typename Length, size_t N>
Position<Length, N> operator/(const Number & n, const Position<Length, N> & pos);

// actual class definition
template <typename LengthType, size_t N>
class Position {
public:
    using container_type = std::array<LengthType, N>;
    using Iterator = typename container_type::iterator;
    using ConstIterator = typename container_type::const_iterator;
    static constexpr size_t dim = N;
private:
    container_type coordinates;


    template <size_t ... Indices, typename ... Types>
    Position(std::index_sequence<Indices...>, Types&& ... args)
    {
        // c++17 fold expression
        (static_cast<void>(coordinates[Indices] = args), ...);
    }

public:
    /*
     * Constructor that allows to initialize the position by typing out all N coordinates.
     * The seeming complexity of this method comes from the usage of templates and needing to restrict
     * the number of valid parameters to exactly N.
     */
    template <typename ... Types, typename std::enable_if<sizeof...(Types) == N, int>::type = 0>
    Position(Types&&...args)
    : Position(std::index_sequence_for<Types...>{}, std::forward<Types>(args)...) {}

    Position() : coordinates() {}
    Position(const Position & position) : coordinates(position.get_coordinates()) {}
    explicit Position(container_type coords) : coordinates(std::move(coords)) {}

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
    bool operator<=(const Position & other) const;
    bool operator>(const Position & other) const;
    bool operator>=(const Position & other) const;

    container_type get_coordinates() const {return coordinates;}
    template <typename container_start, typename container_end>
    Position invert(const container_start & starts, const container_end & ends);
    std::string to_string();
};


// free operators for switched call positions

template <typename Number, typename Length, size_t N>
Position<Length, N> operator*(const Number & n, const Position<Length, N> & pos) {return pos * n;}

template <typename Number, typename Length, size_t N>
Position<Length, N> operator/(const Number & n, const Position<Length, N> & pos) {
    Position<Length, N> p(pos);
    for(size_t i = 0; i < N; ++i) {
        p[i] /= n;
    }
}


// method implementations


template <typename LengthType, size_t N>
Position<LengthType, N> Position<LengthType, N>::operator+(const Position<LengthType, N> & pos) const {
    Position<LengthType, N> p(*this);
    for(size_t i = 0; i < N; ++i) {
        p[i] += pos[i];
    }
}

template <typename LengthType, size_t N>
Position<LengthType, N> Position<LengthType, N>::operator-(const Position<LengthType, N> &  pos) const {return *this + (-1 * pos);}
template <typename LengthType, size_t N>
Position<LengthType, N> Position<LengthType, N>::operator*(const Position<LengthType, N> & pos) const {
    Position<LengthType, N> p(*this);
    for(size_t i = 0; i < coordinates.size(); ++i) {
        p[i] *= pos[i];
    }
}
template <typename LengthType, size_t N>
Position<LengthType, N> Position<LengthType, N>::operator/(const Position<LengthType, N> & pos) const {return *this * (1 / pos);}
template <typename LengthType, size_t N>
template <typename Number>
Position<LengthType, N> Position<LengthType, N>::operator+(const Number & n) const {
    Position<LengthType, N> p(*this);
    for(size_t i = 0; i < N; ++i) {
        p[i] += n;
    }
}
template <typename LengthType, size_t N>
template <typename Number>
Position<LengthType, N> Position<LengthType, N>::operator-(const Number & n) const {return *this + (-n);}
template <typename LengthType, size_t N>
template <typename Number>
Position<LengthType, N> Position<LengthType, N>::operator*(const Number & n) const {
    Position<LengthType, N> p(*this);
    for(size_t i = 0; i < N; ++i) {
        p[i] *= n;
    }
}
template <typename LengthType, size_t N>
template <typename Number>
Position<LengthType, N> Position<LengthType, N>::operator/(const Number & n) const {return *this * (1 / n);}
template <typename LengthType, size_t N>
bool Position<LengthType, N>::operator==(const Position & other) const {
    for(size_t i = 0; i < N; ++i) {
        if((*this)[i] != other[i])
            return false;
    }
    return true;
}
template <typename LengthType, size_t N>
bool Position<LengthType, N>::operator!=(const Position & other) const {
    return !(*this == other);
}
template <typename LengthType, size_t N>
bool Position<LengthType, N>::operator<(const Position & other) const {
    for(size_t i = 0; i < N; ++i) {
        if((*this)[i] < other[i])
            return true;
        else if((*this)[i] > other[i])
            return false;
        // the else case is the == case for which we simply continue onto the next dimension
    }
    return false;
}
template <typename LengthType, size_t N>
bool Position<LengthType, N>::operator<=(const Position & other) const {
    for(size_t i = 0; i < N; ++i) {
        if((*this)[i] <= other[i])
            return true;
        else if((*this)[i] > other[i])
            return false;
        // the else case is the == case for which we simply continue onto the next dimension
    }
    return false;
}
template <typename LengthType, size_t N>
bool Position<LengthType, N>::operator>(const Position & other) const {
    for(size_t  i = 0; i < N; ++i) {
        if((*this)[i] > other[i])
            return true;
        else if((*this)[i] < other[i])
            return false;
        // the else case is the == case for which we simply continue onto the next dimension
    }
    return false;
}
template <typename LengthType, size_t N>
bool Position<LengthType, N>::operator>=(const Position & other) const {
    for(size_t  i = 0; i < N; ++i) {
        if((*this)[i] > other[i])
            return true;
        else if((*this)[i] < other[i])
            return false;
        // the else case is the == case for which we simply continue onto the next dimension
    }
    return false;
}
template <typename LengthType, size_t N>
std::string Position<LengthType, N>::to_string() {
    std::stringstream ss;
    ss << "(";
    for(size_t  i = 0; i < N-1; ++i) {
        ss << std::to_string(coordinates[i]) << ", ";
    }
    ss << std::to_string(coordinates.back()) << ")";
    return ss.str();
}

template<typename LengthType, size_t N>
template<typename container_start, typename container_end>
Position<LengthType, N> Position<LengthType, N>::invert(const container_start & starts, const container_end & ends) {
    if constexpr(std::is_floating_point_v<LengthType>) {
        if constexpr(!std::is_floating_point_v<typename container_start::value_type>) {
            throw std::invalid_argument(
                    std::string("Container value_type of 'starts' is not of floating point (") +
                    std::string(typeid(typename container_start::value_type).name()) +
                    std::string("), while 'Position' value type is (") +
                    std::string(typeid(LengthType).name()) +
                    std::string(").")
            );
        }
        if constexpr(!std::is_floating_point_v<typename container_end::value_type>) {
            throw std::invalid_argument(
                    std::string("Container value_type of 'ends' is not of floating point (") +
                    std::string(typeid(typename container_end::value_type).name()) +
                    std::string("), while 'Position' value type is (") +
                    std::string(typeid(LengthType).name()) +
                    std::string(").")
            );
        }
    }

    Position<LengthType, N> inverted;
    for(size_t i = 0; i < N; ++i) {
        inverted[i] = starts[i] + ends[i] - coordinates[i];
    }
    return inverted;
}
