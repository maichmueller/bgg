//
// Created by Michael on 20/02/2019.
//

#pragma once

#include <map>
#include <unordered_map>
#include <array>
#include <vector>
#include <memory>
#include <algorithm>

#include "azpp/board/Piece.h"
#include "azpp/board/Move.h"
#include "azpp/utils/utils.h"


template<typename PieceType>
class Board {
public:
    using piece_type = PieceType;
    using kin_type = typename piece_type::kin_type;
    using position_type = typename piece_type::position_type;
    using move_type = Move<position_type>;
    using map_type = std::map<position_type, std::shared_ptr<PieceType>>;
    using inverse_map_type = std::array<std::unordered_map<kin_type, position_type>, 2>;
    using iterator = typename map_type::iterator;
    using const_iterator = typename map_type::const_iterator;

    static constexpr size_t m_dim = position_type::dim;

protected:
    std::array<size_t, m_dim> m_shape;
    std::array<int, m_dim> m_board_starts{}; // initializes all entries to 0 this way (for defaulting)
    map_type m_board_map;
    inverse_map_type m_board_map_inverse;

    void check_pos_bounds(const position_type &pos) const;

    void _fill_board_null_pieces(size_t dim,
                                 std::array<int, m_dim> &&position_pres = std::array<int, m_dim>{0});

    void _fill_inverse_board();

public:
    explicit Board(const std::array<int, m_dim> &shape);

    Board(const std::array<size_t, m_dim> &shape,
          const std::array<int, m_dim> &board_starts);

    Board(const std::array<size_t, m_dim> &shape,
          const std::vector<std::shared_ptr<piece_type>> &setup_0,
          const std::vector<std::shared_ptr<piece_type>> &setup_1);

    Board(const std::array<size_t, m_dim> &shape,
          const std::array<int, m_dim> &board_starts,
          const std::vector<std::shared_ptr<piece_type>> &setup_0,
          const std::vector<std::shared_ptr<piece_type>> &setup_1);

    Board(const std::array<size_t, m_dim> &shape,
          const std::map<position_type, kin_type> &setup_0,
          const std::map<position_type, kin_type> &setup_1);

    Board(const std::array<size_t, m_dim> &shape,
          const std::array<int, m_dim> &board_starts,
          const std::map<position_type, kin_type> &setup_0,
          const std::map<position_type, kin_type> &setup_1);

    std::shared_ptr<PieceType> &operator[](const position_type &position);

    const std::shared_ptr<PieceType> &operator[](const position_type &position) const;

    [[nodiscard]] iterator begin() { return m_board_map.begin(); }

    [[nodiscard]] iterator end() { return m_board_map.end(); }

    [[nodiscard]] const_iterator begin() const { return m_board_map.begin(); }

    [[nodiscard]] const_iterator end() const { return m_board_map.end(); }

    [[nodiscard]] auto get_shape() const { return m_shape; }

    [[nodiscard]] auto get_starts() const { return m_board_starts; }

    [[nodiscard]] auto size() const { return m_board_map.size(); }

    map_type const * get_map() const { return m_board_map; }

    position_type get_position_of_kin(int team, const kin_type &kin) { return m_board_map_inverse[team][kin]; }

    std::vector<std::shared_ptr<piece_type> > get_pieces(int player);

    void update_board(const position_type &pos, const std::shared_ptr<piece_type> &pc);

    [[nodiscard]] virtual std::string print_board(bool flip_board = false, bool hide_unknowns = false) const = 0;

};


template<typename PieceType>
void Board<PieceType>::check_pos_bounds(const position_type &pos) const {
    for (size_t i = 0; i < m_dim; ++i) {
        if (pos[0] < m_board_starts[i] || (pos[i] > 0 && (size_t) pos[i] >= m_shape[i])) {
            std::ostringstream ss;
            std::string val_str = std::to_string(pos[i]);
            std::string bounds_str = std::to_string(m_board_starts[i]) + ", " + std::to_string(m_shape[i]);
            ss << "Index at dimension " << std::to_string(i) << " out of bounds " <<
               "(Value :" << val_str <<
               ", Bounds: [" << bounds_str << "])";
            throw std::invalid_argument(ss.str());
        }
    }
}

template<typename PieceType>
const std::shared_ptr<PieceType> &Board<PieceType>::operator[](const position_type &position) const {
    return m_board_map.find(position)->second;
}

template<typename PieceType>
std::shared_ptr<PieceType> &Board<PieceType>::operator[](const position_type &position) {
    return m_board_map.find(position)->second;
}

template<typename PieceType>
void Board<PieceType>::update_board(const position_type &pos, const std::shared_ptr<piece_type> &pc_ptr) {
    check_pos_bounds(pos);
    pc_ptr->set_position(pos);
    (*this)[pos] = pc_ptr;
    m_board_map_inverse[pc_ptr->get_team()][pc_ptr->get_kin()] = pos;
}


template<typename PieceType>
void Board<PieceType>::_fill_board_null_pieces(size_t dim,
                                           std::array<int, m_dim> &&position_pres) {
    if (dim == m_dim) {
        for (int i = m_board_starts[m_dim - 1];
             i < static_cast<int>(m_board_starts[m_dim - 1] + m_shape[m_dim - 1]); ++i) {
            position_pres[m_dim - 1] = i;
            _fill_board_null_pieces(dim - 1, std::forward<std::array<int, m_dim>>(position_pres));
        }
    } else if (dim > 0) {
        for (int i = m_board_starts[dim - 1]; i < static_cast<int>(m_board_starts[dim - 1] + m_shape[dim - 1]); ++i) {
            position_pres[dim - 1] = i;
            _fill_board_null_pieces(dim-1, std::forward<std::array<int, m_dim>>(position_pres));
        }
    } else {
        position_type pos(position_pres);
        m_board_map[pos] = std::make_shared<piece_type>(pos);
    }
}

template<typename PieceType>
Board<PieceType>::Board(const std::array<int, m_dim> &shape)
        : m_shape(shape),
          m_board_map(),
          m_board_map_inverse() {
    _fill_board_null_pieces(m_dim);
}

template<typename PieceType>
Board<PieceType>::Board(const std::array<size_t, m_dim> &shape,
                    const std::array<int, m_dim> &board_starts)
        : m_shape(shape),
          m_board_starts(board_starts),
          m_board_map(),
          m_board_map_inverse() {
    _fill_board_null_pieces(m_dim);
}

template<typename PieceType>
Board<PieceType>::Board(const std::array<size_t, m_dim> &shape,
                    const std::array<int, m_dim> &board_starts,
                    const std::vector<std::shared_ptr<piece_type>> &setup_0,
                    const std::vector<std::shared_ptr<piece_type>> &setup_1)
        : Board(shape, board_starts) {

    auto setup_unwrap = [&](const std::vector<std::shared_ptr<piece_type>> &setup) {
        std::map<position_type, int> seen_pos;
        for (const auto &piece : setup) {
            position_type pos = piece->get_position();
            if (seen_pos.find(pos) != seen_pos.end()) {
                //element found
                throw std::invalid_argument("Parameter setup has more than one piece for the "
                                            "same position (position: '" + pos.to_string() + "').");
            }
            seen_pos[pos] = 1;
            m_board_map[pos] = piece;
        }
    };
    setup_unwrap(setup_0);
    setup_unwrap(setup_1);

    _fill_inverse_board();
}

template<typename PieceType>
Board<PieceType>::Board(const std::array<size_t, m_dim> &shape,
                    const std::vector<std::shared_ptr<piece_type>> &setup_0,
                    const std::vector<std::shared_ptr<piece_type>> &setup_1)
        : Board(shape, m_board_starts, setup_0, setup_1) {}

template<typename PieceType>
Board<PieceType>::Board(const std::array<size_t, m_dim> &shape,
                    const std::array<int, m_dim> &board_starts,
                    const std::map<position_type, kin_type> &setup_0,
                    const std::map<position_type, kin_type> &setup_1)
        : Board(shape, board_starts) {
    auto setup_unwrap = [&](const std::map<position_type, kin_type> &setup, int team) {
        // because of the short length of the vectors they might be faster than using a map (fit in cache)
        std::vector<position_type> seen_pos;
        std::vector<kin_type> seen_kin;
        for (auto &elem : setup) {
            position_type pos = elem.first;
            auto character = elem.second;

            if (std::find(seen_pos.begin(), seen_pos.end(), pos) != seen_pos.end()) {
                //element found
                throw std::invalid_argument(std::string("Parameter setup has more than one piece for the ") +
                                            std::string("same position (position: '") +
                                            pos.to_string() + std::string("')."));
            }
            seen_pos.emplace_back(pos);
            if (std::find(seen_kin.begin(), seen_kin.end(), character) != seen_kin.end()) {
                throw std::invalid_argument(std::string("Parameter setup has more than one piece for the ") +
                                            std::string("same character (character: '") +
                                            character.to_string() + std::string("')."));
            }
            seen_kin.emplace_back(character);

            auto piece = std::make_shared<piece_type>(pos, character, team);
            m_board_map[pos] = std::move(piece);
        }
    };
    setup_unwrap(setup_0, 0);
    setup_unwrap(setup_1, 1);

    _fill_inverse_board();
}

template<typename PieceType>
Board<PieceType>::Board(const std::array<size_t, m_dim> &shape,
                    const std::map<position_type, typename piece_type::kin_type> &setup_0,
                    const std::map<position_type, typename piece_type::kin_type> &setup_1)
        : Board(shape, m_board_starts, setup_0, setup_1) {}

template<typename PieceType>
std::vector<std::shared_ptr<typename Board<PieceType>::piece_type> >
Board<PieceType>::get_pieces(int player) {
    std::vector<std::shared_ptr<piece_type> > pieces;
    for (auto &pos_piece : m_board_map) {
        std::shared_ptr<piece_type> piece = pos_piece.second;
        if (!piece->is_null() && piece->get_team() == player) {
            pieces.emplace_back(piece);
        }
    }
    return pieces;
}

template<class PieceType>
void Board<PieceType>::_fill_inverse_board() {
    for (const auto &piece_ptr : m_board_map) {
        auto & piece = piece_ptr.second;
        if (!piece->is_null()) {
            m_board_map_inverse[piece->get_team()][piece->get_kin()] = piece->get_position();
        }
    }
}