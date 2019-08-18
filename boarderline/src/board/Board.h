//
// Created by Michael on 20/02/2019.
//

#ifndef STRATEGO_CPP_BOARD_H
#define STRATEGO_CPP_BOARD_H

#include "map"
#include "array"
#include "vector"
#include "memory"

#include "Piece.h"
#include "../game/GameDeclarations.h"
#include "../utils/utils.h"


using namespace GameDeclarations;

template<typename Piece, typename Position>
class Board {
public:
    // comparator for 2-tuple keys
    using Map_Type = std::map<Position, std::shared_ptr<Piece>, key_comp>;
    using iterator = typename Map_Type::iterator;
    using const_iterator = typename Map_Type::const_iterator;

    static constexpr int m_dim = Position::dim;

private:
    std::array<int, m_dim> m_shape;
    std::array<int, m_dim> board_starts{}; // initializes all entries to 0 this way (for defaulting)
    Map_Type m_board_map;

    bool check_pos_bounds(const Position &pos);

    void place_obstacles();

public:
    Board(const std::array<int, m_dim> &shape);

    Board(const std::array<int, m_dim> &shape,
          const std::array<int, m_dim> &board_starts);

    Board(const std::array<int, m_dim> &shape,
          const std::vector<std::shared_ptr<Piece>> &setup_0,
          const std::vector<std::shared_ptr<Piece>> &setup_1);

    Board(const std::array<int, m_dim> &shape,
          const std::array<int, m_dim> &board_starts,
          const std::vector<std::shared_ptr<Piece>> &setup_0,
          const std::vector<std::shared_ptr<Piece>> &setup_1);

    Board(const std::array<int, m_dim> &shape,
          const std::map<Position, int> &setup_0,
          const std::map<Position, int> &setup_1);

    Board(const std::array<int, m_dim> &shape,
          const std::array<int, m_dim> &board_starts,
          const std::map<Position, int> &setup_0,
          const std::map<Position, int> &setup_1);

    std::shared_ptr<Piece> &operator[](Position a);

    const std::shared_ptr<Piece> &operator[](const Position &a) const;

    [[nodiscard]] iterator begin() { return m_board_map.begin(); }

    [[nodiscard]] iterator end() { return m_board_map.end(); }

    [[nodiscard]] const_iterator begin() const { return m_board_map.begin(); }

    [[nodiscard]] const_iterator end() const { return m_board_map.end(); }

    [[nodiscard]] int get_shape() const { return m_shape; }

    Map_Type &get_map() { return m_board_map; }

    void update_board(Position &pos, std::shared_ptr<Piece> &pc);

    void print_board() const;

    [[nodiscard]] auto size() const { return m_board_map.size(); }

};

template<typename Piece, typename Position>
const std::shared_ptr<Piece> &Board<Piece, Position>::operator[](const Position &a) const {
    for (int i = 0; i < m_dim; ++i) {
        if (a[i] >= m_shape[i] || a[0] <) {
            throw std::invalid_argument("Row index out of bounds.");
        }
        if (a[i] >= m_shape || a[i] < 0) {
            throw std::invalid_argument("Column index out of bounds.");
        }
    }
    return m_board_map.find(a)->second;
}

template<typename Piece, typename Position>
std::shared_ptr<Piece> &Board<Piece, Position>::operator[](Position a) {
    if (a[0] >= m_shape || a[0] < 0) {
        throw std::invalid_argument("Row index out of bounds.");
    }
    if (a[1] >= m_shape || a[1] < 0) {
        throw std::invalid_argument("Column index out of bounds.");
    }
    return m_board_map[a];
}

template<typename Piece, typename Position>
void Board<Piece, Position>::place_obstacles() {
    for (auto obs_pos : GameDeclarations::get_obstacle_pos(m_shape)) {
        auto obs = std::make_shared<Piece>(99, 99, obs_pos, -1);
        m_board_map[obs_pos] = obs;
    }
}

template<typename Piece, typename Position>
bool Board<Piece, Position>::check_pos_bounds(const Position &pos) {
    if (pos[0] < 0 || m_shape <= pos[0]) {
        return false;
    } else return !(pos[1] < 0 || m_shape <= pos[1]);
}

template<typename Piece, typename Position>
void Board<Piece, Position>::update_board(Position & pos, std::shared_ptr<Piece> &pc_ptr) {
    bool valid_pos = check_pos_bounds(pos);
    pc_ptr->set_position(pos);
    if (valid_pos)
        m_board_map[pos] = pc_ptr;
    else
        throw std::invalid_argument("Supplied position out of bounds.");
}

template<typename Piece, typename Position>
void Board<Piece, Position>::print_board() const {
    utils::print_board<Board, Piece>(*this);
}

template<typename Piece, typename Position>
Board<Piece, Position>::Board(const std::array<int, m_dim> &shape)
        : m_shape(shape),
          m_board_map() {
    for (int i = 0; i < shape; ++i) {
        for (int j = 0; j < shape; ++j) {
            Position pos = {i, j};
            m_board_map[pos] = std::make_shared<Piece>(pos);
        }
    }

    place_obstacles();
}

template<typename Piece, typename Position>
Board<Piece, Position>::Board(const std::array<int, m_dim> &shape,
                              const std::array<int, m_dim> &board_starts)
        : m_shape(shape),
          m_board_map() {
    if (len != 5 && len != 7 && len != 10) {
        throw std::invalid_argument("Game length not in [5, 7, 10].");
    }

    for (int i = 0; i < shape; i++) {
        for (int j = 0; j < shape; j++) {
            Position pos = {i, j};
            m_board_map[pos] = std::make_shared<Piece>(pos);
        }
    }

    place_obstacles();
}

template<typename Piece, typename Position>
Board<Piece, Position>::Board(const std::array<int, m_dim> &shape,
                              const std::array<int, m_dim> &board_starts,
                              const std::vector<std::shared_ptr<Piece>> &setup_0,
                              const std::vector<std::shared_ptr<Piece>> &setup_1)
        : Board(shape) {
    std::map<Position, int, key_comp> seen_pos_0;
    for (auto &piece : setup_0) {
        Position pos = piece->get_position();
        auto it = seen_pos_0.find(pos);
        if (it != seen_pos_0.end()) {
            //element found
            throw std::invalid_argument("Parameter setup 0 has duplicate piece from_to.");
        }

        seen_pos_0[pos] = 1;
        m_board_map[pos] = piece;
    }

    std::map<Position, int, key_comp> seen_pos_1;
    for (auto &piece : setup_1) {
        Position pos = piece->get_position();
        auto it = seen_pos_1.find(pos);
        if (it != seen_pos_1.end()) {
            //element found
            throw std::invalid_argument("Parameter setup 1 has duplicate piece from_to.");
        }

        seen_pos_1[pos] = 1;
        m_board_map[pos] = piece;
    }

    place_obstacles();
}

template<typename Piece, typename Position>
Board<Piece, Position>::Board(const std::array<int, m_dim> &shape,
                              const std::array<int, m_dim> &board_starts,
                              const std::map<Position, int> &setup_0,
                              const std::map<Position, int> &setup_1)
        : Board(shape) {
    std::map<Position, int, key_comp> seen_pos_0;
    std::map<int, int> version_count_0;
    for (auto &elem : setup_0) {
        Position pos = elem.first;
        int piece_type = elem.second;

        auto it = seen_pos_0.find(pos);
        if (it != seen_pos_0.end()) {
            //element found
            throw std::invalid_argument("Parameter setup 0 has duplicate piece from_to.");
        }

        seen_pos_0[pos] = 1;
        // null constructor of map is called on unplaced previous item (creates 0 int)
        // therefore the first time this is called, will get us to version 1, the first
        // piece. Afterwards it will keep count correctly for us.
        int version = version_count_0[piece_type];
        version_count_0[piece_type] += 1;
        auto piece = std::make_shared<Piece>(0, piece_type, pos, version);
        m_board_map[pos] = std::move(piece);
    }

    std::map<Position, int, key_comp> seen_pos_1;
    std::map<int, int> version_count_1;
    for (auto &elem : setup_1) {
        Position pos = elem.first;
        int piece_type = elem.second;

        auto it = seen_pos_1.find(pos);
        if (it != seen_pos_1.end()) {
            //element found
            throw std::invalid_argument("Parameter setup 1 has duplicate piece from_to.");
        }

        seen_pos_1[pos] = 1;
        // null constructor of map is called on unplaced previous item (creates 0 int)
        // therefore the first time this is called, will get us to version 0, the first
        // piece. Afterwards it will keep count correctly for us.
        int version = version_count_1[piece_type];
        version_count_1[piece_type] += 1;
        auto piece = std::make_shared<Piece>(1, piece_type, pos, version);
        m_board_map[pos] = std::move(piece);
    }

    place_obstacles();
}


#endif //STRATEGO_CPP_BOARD_H
