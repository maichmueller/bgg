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
        int V_SIZE_PER_PIECE = 5;
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
            int type = piece.get_type();
            int version = piece.get_version();
            int team = piece.get_team();
            std::string hidden = (piece.get_flag_hidden()) ? "+" : "-";
            std::stringstream piece_str;
            piece_str << hidden << team << ":" << type << "." << version;
            return center(piece_str.str(), H_SIZE_PER_PIECE, " ");
        };

        std::stringstream board_print;
        // column width for the row index plus vertical dash
        board_print << std::string(static_cast<unsigned long> (row_ind_space), ' ');
        // print the column index rows
        for(int i = 0; i < nr_pieces; ++i) {
            board_print << center(std::to_string(i), H_SIZE_PER_PIECE, " ");
        }

        std::string init_space = std::string(static_cast<unsigned long> (row_ind_space), ' ');
        std::string h_border = std::string(static_cast<unsigned long> (nr_pieces * H_SIZE_PER_PIECE), '-');

        board_print << "\n" << init_space << h_border  << "\n";

        std::shared_ptr<Piece> curr_piece;
        int mid = V_SIZE_PER_PIECE / 2 + 1;

        for(int k = 0; k < dim; ++k) {
            for(int i = 0; i < V_SIZE_PER_PIECE; ++i) {

                if (i != mid) {
                    std::string p = "|" + std::string(static_cast<unsigned long> (H_SIZE_PER_PIECE), ' ') + "|";
                    board_print << std::string(static_cast<unsigned long> (row_ind_space), ' ') << p * nr_pieces;
                }
                else {
                    // this happens when i == mid
                    if (i < 10) {
                        board_print << " " << i;
                    } else {
                        board_print << i;
                    }

                    board_print << std::string(static_cast<unsigned long> (row_ind_space - 3), ' ') << "|";

                    // iterate over all pieces of the board to get info and print to str
                    for (int j = 0; j < dim; ++j) {
                        curr_piece = board[{k, j}];
                        board_print << "|" + create_piece_str(*curr_piece) + "|";
                    }
                }
            }
            board_print << "\n" << init_space << h_border  << "\n";
        }

        std::string output = board_print.str();
        std::cout << output << std::endl;
    }
};


#endif //STRATEGO_CPP_UTILS_H
