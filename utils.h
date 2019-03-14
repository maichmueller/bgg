//
// Created by Michael on 08/03/2019.
//

#ifndef STRATEGO_CPP_UTILS_H
#define STRATEGO_CPP_UTILS_H

#include "GameDeclarations.h"
#include "string"
#include "memory"
#include "iostream"
#include "iomanip"
#include <sstream>


namespace utils {

    std::string center(const std::string &str, int width, const char* fillchar);

    std::string repeat(std::string str, const std::size_t n);

    inline std::string operator*(std::string str, std::size_t n)
    {
        return repeat(std::move(str), n);
    }


    template <typename Board, typename Piece>
    void print_board(Board &board) {

        int H_SIZE_PER_PIECE = 9;
        int V_SIZE_PER_PIECE = 3;
        // the space needed to assign row indices to the rows and add a splitting bar "|"
        int row_ind_space = 4;

        int dim = board.get_board_len();
        unsigned short multiplier = 2;
        if(dim == 5)
            ;
        else if(dim == 7)
            multiplier = 3;
        else if(dim == 10)
            multiplier = 4;
        else
            throw std::invalid_argument("Board dimension not supported.");

        int nr_pieces = dim * multiplier;

        // piece string lambda function that throws out a str of the sort "-1:10.1"
        auto create_piece_str = [&H_SIZE_PER_PIECE] (Piece& piece) {
            if(piece.is_null())
                return std::string(static_cast<unsigned long> (H_SIZE_PER_PIECE), ' ');
            int type = piece.get_type();
            int version = piece.get_version();
            int team = piece.get_team();
            std::string hidden = (piece.get_flag_hidden()) ? "?" : "!";
            std::stringstream piece_str;
            piece_str << hidden << " " << team << ":" << type << "." << version;
            return center(piece_str.str(), H_SIZE_PER_PIECE, " ");
        };

        std::stringstream board_print;
        board_print << "\n";
        // column width for the row index plus vertical dash
        board_print << std::string(static_cast<unsigned long> (row_ind_space), ' ');
        // print the column index rows
        for(int i = 0; i < dim; ++i) {
            board_print << center(std::to_string(i), H_SIZE_PER_PIECE + 1, " ");
        }
        board_print << "\n";

        std::string init_space = std::string(static_cast<unsigned long> (row_ind_space), ' ');
        std::string h_border = std::string(static_cast<unsigned long> (dim * (H_SIZE_PER_PIECE+1)), '-');

        board_print << init_space << h_border << "\n";
        std::string init = board_print.str();
        std::shared_ptr<Piece> curr_piece;
        int mid = V_SIZE_PER_PIECE / 2;

        // row means row of the board. not actual rows of console output.
        for(int row = 0; row < dim; ++row) {
            // per piece we have V_SIZE_PER_PIECE many lines to fill consecutively.
            // Iterate over every column and append the new segment to the right line.
            std::vector<std::stringstream> line_streams(static_cast<unsigned int> (V_SIZE_PER_PIECE));

            for (int col = 0; col < dim; ++col) {

                curr_piece = board[{row, col}];

                for(int i = 0; i < V_SIZE_PER_PIECE; ++i) {

                    std::stringstream curr_stream;

                    if (i != mid) {
                        if(col == 0) {
                            curr_stream << std::string(static_cast<unsigned long> (row_ind_space), ' ');
                        }
                        std::string p = "|" +  std::string(static_cast<unsigned long> (H_SIZE_PER_PIECE), ' ');
                        curr_stream << p;
                    }
                    else {
                        // i == mid
                        if(col == 0) {
                            if (row < 10)
                                curr_stream << " " << row;
                            else
                                curr_stream << row;

                            curr_stream << std::string(static_cast<unsigned long> (row_ind_space - 2), ' ') << "|";
                        }
                        curr_stream << create_piece_str(*curr_piece);
                        if(col != dim -1)
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

        std::string output = board_print.str();
        std::cout << output << std::endl;
        int p = 3;
    }
};


#endif //STRATEGO_CPP_UTILS_H
