//
// Created by Michael on 20/02/2019.
//

#pragma once

#include "map"
#include "array"
#include "vector"
#include "memory"

#include "Piece.h"
#include "../game/GameUtilsStratego.h"
#include "../utils/utils.h"


template<typename Piece, typename Position>
class Board {
public:
    using piece_type = Piece;
    using position_type = Position;
    using MapType = std::map<Position, std::shared_ptr<Piece>>;
    using iterator = typename MapType::iterator;
    using const_iterator = typename MapType::const_iterator;

    static constexpr int m_dim = Position::dim;

protected:
    std::array<int, m_dim> m_shape;
    std::array<int, m_dim> m_board_starts{}; // initializes all entries to 0 this way (for defaulting)
    MapType m_board_map;

    void check_pos_bounds(const Position &pos) const;

    void _fill_board_null_pieces(const std::array<int, m_dim> &shape,
                                 const std::array<int, m_dim> &board_starts);

public:
    explicit Board(const std::array<int, m_dim> &shape);

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

    std::shared_ptr<Piece> &operator[](const Position&& a);
    const std::shared_ptr<Piece> &operator[](const Position &&a) const;

    [[nodiscard]] iterator begin() { return m_board_map.begin(); }
    [[nodiscard]] iterator end() { return m_board_map.end(); }

    [[nodiscard]] const_iterator begin() const { return m_board_map.begin(); }
    [[nodiscard]] const_iterator end() const { return m_board_map.end(); }

    [[nodiscard]] int get_shape() const { return m_shape; }
    MapType& get_map() { return m_board_map; }

    void update_board(Position && pos, std::shared_ptr<Piece> &pc);

    [[nodiscard]] std::string print_board(bool flip_board=false, bool hide_unknowns=false) const;

    [[nodiscard]] std::string to_string_2D(bool flip_board=false, bool hide_unknowns=false) const;

    [[nodiscard]] auto size() const { return m_board_map.size(); }

};

template <typename Piece, typename Position>
void Board<Piece, Position>::check_pos_bounds(const Position &pos) const {
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
const std::shared_ptr<Piece> &Board<Piece, Position>::operator[](const Position &&a) const {
    check_pos_bounds(a);
    return m_board_map.find(std::forward(a))->second;
}

template<typename Piece, typename Position>
std::shared_ptr<Piece> &Board<Piece, Position>::operator[](const Position&& a) {
    return m_board_map.find(std::forward(a))->second;
}

template<typename Piece, typename Position>
void Board<Piece, Position>::update_board(Position && pos, std::shared_ptr<Piece> &pc_ptr) {
    check_pos_bounds(std::forward(pos));
    pc_ptr->set_position(pos);
    (*this)[pos] = pc_ptr;
}

template<typename Piece, typename Position>
void Board<Piece, Position>::_fill_board_null_pieces(const std::array<int, m_dim> &shape,
                                                     const std::array<int, m_dim> &board_starts) {
    for (int i = 0; i < shape; ++i) {
        for (int j = 0; j < shape; ++j) {
            Position pos = {i, j};
            // create null piece at pos
            m_board_map[pos] = std::make_shared<Piece>(pos);
        }
    }
}

template<typename Piece, typename Position>
Board<Piece, Position>::Board(const std::array<int, m_dim> &shape)
        : m_shape(shape),
          m_board_map() {
    _fill_board_null_pieces(m_shape, m_board_starts);
}

template<typename Piece, typename Position>
Board<Piece, Position>::Board(const std::array<int, m_dim> &shape,
                              const std::array<int, m_dim> &board_starts)
        : m_shape(shape),
          m_board_starts(board_starts),
          m_board_map() {
    _fill_board_null_pieces(m_shape, m_board_starts);
}

template<typename Piece, typename Position>
Board<Piece, Position>::Board(const std::array<int, m_dim> &shape,
                              const std::array<int, m_dim> &board_starts,
                              const std::vector<std::shared_ptr<Piece>> &setup_0,
                              const std::vector<std::shared_ptr<Piece>> &setup_1)
        : Board(shape, board_starts) {

    auto setup_unloader = [&](std::vector<std::shared_ptr<Piece>> & setup) {
        std::map<Position, int> seen_pos;
        std::map<typename Piece::type_type, int> version_count;
        for (auto & piece : setup) {
            Position pos = piece->get_position();
            if (seen_pos.find(pos) != seen_pos.end()) {
                //element found
                throw std::invalid_argument("Parameter setup has more than one piece for the "
                                            "same position (position: '" + pos.to_string() + "').");
            }
            seen_pos[pos] = 1;
            m_board_map[pos] = piece;
        }
    };
    setup_unloader(setup_0);
    setup_unloader(setup_1);
}

template<typename Piece, typename Position>
Board<Piece, Position>::Board(const std::array<int, m_dim> &shape,
                              const std::vector<std::shared_ptr<Piece>> &setup_0,
                              const std::vector<std::shared_ptr<Piece>> &setup_1)
        : Board(shape, m_board_starts, setup_0, setup_1) {}

template<typename Piece, typename Position>
Board<Piece, Position>::Board(const std::array<int, m_dim> &shape,
                              const std::array<int, m_dim> &board_starts,
                              const std::map<Position, int> &setup_0,
                              const std::map<Position, int> &setup_1)
        : Board(shape, board_starts) {
    auto setup_unloader = [&](std::map<Position, int> & setup) {
        std::map<Position, int> seen_pos;
        std::map<typename Piece::type_type, int> version_count;
        for (auto &elem : setup) {
            Position pos = elem.first;
            auto piece_type = elem.second;

            if (seen_pos.find(pos) != seen_pos.end()) {
                //element found
                throw std::invalid_argument("Parameter setup has more than one piece for the "
                                            "same position (position: '" + pos.to_string() + "').");
            }
            seen_pos[pos] = 1;
            // null constructor of map is called on unplaced previous item (creates 0 int)
            // therefore the first time this is called, will get us to version 1, the first
            // piece. Afterwards it will keep the count correctly for us.
            int version = version_count[piece_type];
            version_count[piece_type] += 1;
            auto piece = std::make_shared<Piece>(0, piece_type, pos, version);
            m_board_map[pos] = std::move(piece);
        }
    };
    setup_unloader(setup_0);
    setup_unloader(setup_1);
}

template<typename Piece, typename Position>
Board<Piece, Position>::Board(const std::array<int, m_dim> &shape,
                              const std::map<Position, int> &setup_0,
                              const std::map<Position, int> &setup_1)
        : Board(shape, m_board_starts, setup_0, setup_1)
        {}

template<typename Piece, typename Position>
std::string Board<Piece, Position>::to_string_2D(bool flip_board, bool hide_unknowns) const {
    if(m_dim > 2) {
        throw std::logic_error("Board has dimension > 2, thus cannot create 2D representation.");
    }
    int H_SIZE_PER_PIECE = 9;
    int V_SIZE_PER_PIECE = 3;
    // the space needed to assign row indices to the rows and to add a splitting bar "|"
    int row_ind_space = 4;
    int dim_x = m_shape[0];
    int dim_y = m_shape[1];
    int mid = V_SIZE_PER_PIECE / 2;

    // piece string lambda function that returns a str of the sort
    // "-1 \n
    // 10.1 \n
    //   1"
    auto create_piece_str = [&H_SIZE_PER_PIECE, &mid, &flip_board, &hide_unknowns] (const Piece& piece, int line) {
        if(piece.is_null())
            return std::string(static_cast<unsigned long> (H_SIZE_PER_PIECE), ' ');
        std::string reset = "\x1B[0m";
        std::string color = "\x1B[44m"; // blue by default (for player 1)
        if(piece.get_team() == 99)
            return "\x1B[30;47m" + utils::center("", H_SIZE_PER_PIECE, " ") + "\x1B[0m";
        else if(piece.get_team(flip_board) == 0) {
            color = "\x1B[41m"; // background red, text "white"
        }
        if(line == mid-1) {
            // hidden info line
            std::string h = piece.get_flag_hidden() ? "?" : " ";
            return color + utils::center(h, H_SIZE_PER_PIECE, " ") + reset;
        }
        else if(line == mid) {
            // type and version info line
            if(hide_unknowns && piece.get_flag_hidden() && piece.get_team(flip_board)){
                return color + std::string(static_cast<unsigned long> (H_SIZE_PER_PIECE), ' ') + reset;
            }
            return color + center(std::to_string(piece.get_type()) + '.' + std::to_string(piece.get_version()),
                                  H_SIZE_PER_PIECE, " ") + reset ;
        }
        else if(line == mid+1)
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
    for(int i = m_board_starts[0]; i < dim_x; ++i) {
        board_print << utils::center(std::to_string(i), H_SIZE_PER_PIECE + 1, " ");
    }
    board_print << "\n";

    std::string init_space = std::string(static_cast<unsigned long> (row_ind_space), ' ');
    std::string h_border = std::string(static_cast<unsigned long> (dim_x * (H_SIZE_PER_PIECE+1)), '-');

    board_print << init_space << h_border << "\n";
    std::string init = board_print.str();
    std::shared_ptr<Piece> curr_piece;

    // row means row of the board. not actual rows of console output.
    for(int row = m_board_starts[1]; row < dim_y; ++row) {
        // per piece we have V_SIZE_PER_PIECE many lines to fill consecutively.
        // Iterate over every column and append the new segment to the right line.
        std::vector<std::stringstream> line_streams(static_cast<unsigned int> (V_SIZE_PER_PIECE));

        for (int col = m_board_starts[0]; col < dim_x; ++col) {

            if(flip_board) {
                curr_piece = (*this)[{dim_x - 1 - row, dim_y - 1 - col}];
            }
            else
                curr_piece = (*this)[{row, col}];

            for(int i = 0; i < V_SIZE_PER_PIECE; ++i) {

                std::stringstream curr_stream;

                if (i == mid - 1 || i == mid + 1) {
                    if(col == 0) {
                        curr_stream << std::string(static_cast<unsigned long> (row_ind_space), ' ');
                    }
                    curr_stream << "|" << create_piece_str(*curr_piece, i);
                }
                else if(i == mid){

                    if(col == 0) {
                        if (row < 10)
                            curr_stream << " " << row;
                        else
                            curr_stream << row;

                        curr_stream << std::string(static_cast<unsigned long> (row_ind_space - 2), ' ') << "|";
                    }
                    curr_stream << create_piece_str(*curr_piece, i);
                    if(col != dim_x -1)
                        curr_stream << "|";
                }
                // extend the current line i by the new information
                line_streams[i] << curr_stream.str();
            }
        }
        for(auto& stream : line_streams) {
            board_print << stream.str() << "|\n";
        }
        board_print << init_space << h_border  << "\n";
    }
    return board_print.str();
}

template<typename Piece, typename Position>
std::string Board<Piece, Position>::print_board(bool flip_board, bool hide_unknowns) const {
    if(m_dim == 2) {
        return to_string_2D(flip_board, hide_unknowns);
    }
    return std::string();
}
