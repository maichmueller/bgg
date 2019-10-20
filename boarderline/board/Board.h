//
// Created by Michael on 20/02/2019.
//

#pragma once

#include "map"
#include "array"
#include "vector"
#include "memory"

#include "Piece.h"
#include "Move.h"
#include "../game/GameUtilsStratego.h"
#include "../utils/utils.h"


template<typename Piece, typename Position>
class Board {
public:
    using piece_type = Piece;
    using kin_type = typename piece_type::kin_type;
    using position_type = Position;
    using move_type = Move<position_type>;
    using map_type = std::map<Position, std::shared_ptr<Piece>>;
    using iterator = typename map_type::iterator;
    using const_iterator = typename map_type::const_iterator;

    static constexpr size_t m_dim = position_type::dim;

protected:
    std::array<size_t, m_dim> m_shape;
    std::array<int, m_dim> m_board_starts{}; // initializes all entries to 0 this way (for defaulting)
    map_type m_board_map;

    void check_pos_bounds(const position_type &pos) const;

    template<size_t dim>
    void _fill_board_null_pieces(const std::array<size_t, dim> &shape,
                                 const std::array<int, dim> &board_starts,
                                 std::array<int, m_dim> &&position_pres = std::array<int, m_dim>{0});

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
          const std::map<position_type, typename piece_type::kin_type> &setup_0,
          const std::map<position_type, typename piece_type::kin_type> &setup_1);

    Board(const std::array<size_t, m_dim> &shape,
          const std::array<int, m_dim> &board_starts,
          const std::map<position_type, typename piece_type::kin_type> &setup_0,
          const std::map<position_type, typename piece_type::kin_type> &setup_1);

    std::shared_ptr<Piece> &operator[](const position_type &&a);
    const std::shared_ptr<Piece> &operator[](const position_type &&a) const;
    std::shared_ptr<Piece> &operator[](const position_type &a);
    const std::shared_ptr<Piece> &operator[](const position_type &a) const;

    [[nodiscard]] iterator begin() { return m_board_map.begin(); }
    [[nodiscard]] iterator end() { return m_board_map.end(); }
    [[nodiscard]] const_iterator begin() const { return m_board_map.begin(); }
    [[nodiscard]] const_iterator end() const { return m_board_map.end(); }

    [[nodiscard]] auto get_shape() const { return m_shape; }
    [[nodiscard]] auto get_starts() const { return m_board_starts; }
    [[nodiscard]] auto size() const { return m_board_map.size(); }

    map_type const *get_map() const { return m_board_map; }

    std::map<position_type, std::shared_ptr<piece_type> > get_setup(int player);

    void update_board(Position &&pos, std::shared_ptr<piece_type> &pc);
    void update_board(const Position &pos, std::shared_ptr<piece_type> &pc);

    [[nodiscard]] std::string print_board(bool flip_board = false, bool hide_unknowns = false) const;
    [[nodiscard]] std::string to_string_2D(bool flip_board = false, bool hide_unknowns = false) const;

};

template<typename Piece, typename Position>
void Board<Piece, Position>::check_pos_bounds(const position_type &pos) const {
    for (int i = 0; i < m_dim; ++i) {
        if (pos[i] >= m_shape[i] || pos[0] < m_board_starts[i]) {
            std::ostringstream ss;
            std::string val_str = std::to_string(pos[i]);
            std::string bounds_str = std::to_string(m_board_starts[i]) + ", " + std::to_string(m_shape[i]);
            ss << "Index at dimension " << std::to_string(i) << " out of bounds " <<
               "(Value :" << val_str <<
               " Bounds: [" << bounds_str << "])";
            throw std::invalid_argument(ss.str());
        }
    }
}

template<typename Piece, typename Position>
const std::shared_ptr<Piece> &Board<Piece, Position>::operator[](const position_type &&a) const {
    check_pos_bounds(a);
    return m_board_map.find(std::forward(a))->second;
}

template<typename Piece, typename Position>
std::shared_ptr<Piece> &Board<Piece, Position>::operator[](const position_type &&a) {
    return m_board_map.find(std::forward(a))->second;
}

template<typename Piece, typename Position>
const std::shared_ptr<Piece> &Board<Piece, Position>::operator[](const position_type &a) const {
    check_pos_bounds(a);
    return m_board_map.find(std::forward(a))->second;
}

template<typename Piece, typename Position>
std::shared_ptr<Piece> &Board<Piece, Position>::operator[](const position_type &a) {
    return m_board_map.find(std::forward(a))->second;
}

template<typename Piece, typename Position>
void Board<Piece, Position>::update_board(position_type &&pos, std::shared_ptr<piece_type> &pc_ptr) {
    check_pos_bounds(std::forward(pos));
    pc_ptr->set_position(pos);
    (*this)[pos] = pc_ptr;
}

template<typename Piece, typename Position>
void Board<Piece, Position>::update_board(const position_type &pos, std::shared_ptr<piece_type> &pc_ptr) {
    update_board(std::forward(pos));
}


template<typename Piece, typename Position>
template<size_t dim>
void Board<Piece, Position>::_fill_board_null_pieces(const std::array<size_t, dim> &shape,
                                                     const std::array<int, dim> &board_starts,
                                                     std::array<int, m_dim> &&position_pres) {
    if constexpr(dim == m_dim) {
        for (int i = m_board_starts[m_dim - 1];
             i < static_cast<int>(m_board_starts[m_dim - 1] + shape[m_dim - 1]); ++i) {
            position_pres[m_dim - 1] = i;
            _fill_board_null_pieces(shape, board_starts, std::forward<std::array<int, m_dim>>(position_pres));
        }
    } else if constexpr (dim > 0) {
        for (int i = m_board_starts[dim - 1]; i < static_cast<int>(m_board_starts[dim - 1] + shape[dim - 1]); ++i) {
            position_pres[dim - 1] = i;
            _fill_board_null_pieces(shape, board_starts, std::forward<std::array<int, m_dim>>(position_pres));
        }
    } else {
        m_board_map[position_pres] = std::make_shared<piece_type>(position_pres);
    }
}

template<typename Piece, typename Position>
Board<Piece, Position>::Board(const std::array<int, m_dim> &shape)
        : m_shape(shape),
          m_board_map() {
    _fill_board_null_pieces(m_shape, m_board_starts);
}

template<typename Piece, typename Position>
Board<Piece, Position>::Board(const std::array<size_t, m_dim> &shape,
                              const std::array<int, m_dim> &board_starts)
        : m_shape(shape),
          m_board_starts(board_starts),
          m_board_map() {
    _fill_board_null_pieces(m_shape, m_board_starts);
}

template<typename Piece, typename Position>
Board<Piece, Position>::Board(const std::array<size_t, m_dim> &shape,
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
}

template<typename Piece, typename Position>
Board<Piece, Position>::Board(const std::array<size_t, m_dim> &shape,
                              const std::vector<std::shared_ptr<piece_type>> &setup_0,
                              const std::vector<std::shared_ptr<piece_type>> &setup_1)
        : Board(shape, m_board_starts, setup_0, setup_1) {}

template<typename Piece, typename Position>
Board<Piece, Position>::Board(const std::array<size_t, m_dim> &shape,
                              const std::array<int, m_dim> &board_starts,
                              const std::map<position_type, typename piece_type::kin_type> &setup_0,
                              const std::map<position_type, typename piece_type::kin_type> &setup_1)
        : Board(shape, board_starts) {
    auto setup_unwrap = [&](std::map<Position, typename piece_type::kin_type> &setup, int team) {
        std::map<position_type, bool> seen_pos;
        std::map<typename piece_type::chracter_type, bool> seen_char;
        for (auto &elem : setup) {
            position_type pos = elem.first;
            auto character = elem.second;

            if (seen_pos.find(pos) != seen_pos.end()) {
                //element found
                throw std::invalid_argument("Parameter setup has more than one piece for the "
                                            "same position (position: '" + pos.to_string() + "').");
            }
            seen_pos[pos] = true;
            if (seen_char.find(character) != seen_char.end()) {
                throw std::invalid_argument("Parameter setup has more than one piece for the "
                                            "same character (character: '" + character.to_string() + "').");
            }
            seen_char[character] = true;

            auto piece = std::make_shared<piece_type>(pos, character, team);
            m_board_map[pos] = std::move(piece);
        }
    };
    setup_unwrap(setup_0, 0);
    setup_unwrap(setup_1, 1);
}

template<typename Piece, typename Position>
Board<Piece, Position>::Board(const std::array<size_t, m_dim> &shape,
                              const std::map<position_type, typename piece_type::kin_type> &setup_0,
                              const std::map<position_type, typename piece_type::kin_type> &setup_1)
        : Board(shape, m_board_starts, setup_0, setup_1) {}

template<typename Piece, typename Position>
std::string Board<Piece, Position>::to_string_2D(bool flip_board, bool hide_unknowns) const {
    if (m_dim > 2) {
        throw std::logic_error("Board has dimension > 2, thus cannot create 2D representation.");
    }
    int H_SIZE_PER_PIECE = 9;
    int V_SIZE_PER_PIECE = 3;
    // the space needed to assign row indices to the rows and to add a splitting bar "|"
    int row_ind_space = 4;
    int dim_x = m_shape[0];
    int dim_y = m_shape[1];
    int mid = V_SIZE_PER_PIECE / 2;

    // piece string lambda function that returns a str of the kin
    // "-1 \n
    // 10.1 \n
    //   1"
    auto create_piece_str = [&H_SIZE_PER_PIECE, &mid, &flip_board, &hide_unknowns](const Piece &piece, int line) {
        if (piece.is_null())
            return std::string(static_cast<unsigned long> (H_SIZE_PER_PIECE), ' ');
        std::string reset = "\x1B[0m";
        std::string color = "\x1B[44m"; // blue by default (for player 1)
        if (piece.get_team() == 99)
            return "\x1B[30;47m" + utils::center("", H_SIZE_PER_PIECE, " ") + "\x1B[0m";
        else if (piece.get_team(flip_board) == 0) {
            color = "\x1B[41m"; // background red, text "white"
        }
        if (line == mid - 1) {
            // hidden info line
            std::string h = piece.get_flag_hidden() ? "?" : " ";
            return color + utils::center(h, H_SIZE_PER_PIECE, " ") + reset;
        } else if (line == mid) {
            // type and version info line
            if (hide_unknowns && piece.get_flag_hidden() && piece.get_team(flip_board)) {
                return color + std::string(static_cast<unsigned long> (H_SIZE_PER_PIECE), ' ') + reset;
            }
            return color + center(std::to_string(piece.get_type()) + '.' + std::to_string(piece.get_version()),
                                  H_SIZE_PER_PIECE, " ") + reset;
        } else if (line == mid + 1)
            // team info line
            // return color + center(std::to_string(piece.get_team(flip_board)), H_SIZE_PER_PIECE, " ") + reset;
            return color + utils::center("", H_SIZE_PER_PIECE, " ") + reset;
        else
            // empty line
            return std::string(static_cast<unsigned long> (H_SIZE_PER_PIECE), ' ');

    };

    std::stringstream board_print;
    board_print << "\n";
    // column width for the row index plus vertical dash
    board_print << std::string(static_cast<unsigned long> (row_ind_space), ' ');
    // print the column index rows
    for (int i = m_board_starts[0]; i < dim_x; ++i) {
        board_print << utils::center(std::to_string(i), H_SIZE_PER_PIECE + 1, " ");
    }
    board_print << "\n";

    std::string init_space = std::string(static_cast<unsigned long> (row_ind_space), ' ');
    std::string h_border = std::string(static_cast<unsigned long> (dim_x * (H_SIZE_PER_PIECE + 1)), '-');

    board_print << init_space << h_border << "\n";
    std::string init = board_print.str();
    std::shared_ptr<Piece> curr_piece;

    // row means row of the board. not actual rows of console output.
    for (int row = m_board_starts[1]; row < dim_y; ++row) {
        // per piece we have V_SIZE_PER_PIECE many lines to fill consecutively.
        // Iterate over every column and append the new segment to the right line.
        std::vector<std::stringstream> line_streams(static_cast<unsigned int> (V_SIZE_PER_PIECE));

        for (int col = m_board_starts[0]; col < dim_x; ++col) {

            if (flip_board) {
                curr_piece = (*this)[{dim_x - 1 - row, dim_y - 1 - col}];
            } else
                curr_piece = (*this)[{row, col}];

            for (int i = 0; i < V_SIZE_PER_PIECE; ++i) {

                std::stringstream curr_stream;

                if (i == mid - 1 || i == mid + 1) {
                    if (col == 0) {
                        curr_stream << std::string(static_cast<unsigned long> (row_ind_space), ' ');
                    }
                    curr_stream << "|" << create_piece_str(*curr_piece, i);
                } else if (i == mid) {

                    if (col == 0) {
                        if (row < 10)
                            curr_stream << " " << row;
                        else
                            curr_stream << row;

                        curr_stream << std::string(static_cast<unsigned long> (row_ind_space - 2), ' ') << "|";
                    }
                    curr_stream << create_piece_str(*curr_piece, i);
                    if (col != dim_x - 1)
                        curr_stream << "|";
                }
                // extend the current line i by the new information
                line_streams[i] << curr_stream.str();
            }
        }
        for (auto &stream : line_streams) {
            board_print << stream.str() << "|\n";
        }
        board_print << init_space << h_border << "\n";
    }
    return board_print.str();
}

template<typename Piece, typename Position>
std::string Board<Piece, Position>::print_board(bool flip_board, bool hide_unknowns) const {
    if (m_dim == 2) {
        return to_string_2D(flip_board, hide_unknowns);
    }
    return std::string();
}

template<typename Piece, typename Position>
std::map<typename Board<Piece, Position>::position_type,
         std::shared_ptr<typename Board<Piece, Position>::piece_type>>
Board<Piece, Position>::get_setup(int player) {
    std::map<position_type, std::shared_ptr<piece_type>> setup;
    for(auto & pos_piece : m_board_map) {
        position_type pos = pos_piece->first;
        std::shared_ptr<piece_type> piece = pos_piece->second;
        if(!piece->is_null() && piece->get_team() == player) {
            setup[pos] = piece;
        }
    }
    return setup;
}
