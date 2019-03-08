//
// Created by Michael on 08/03/2019.
//

#include "utils.h"


std::string utils::center(const std::string &str, int width, const char* fillchar) {

    int len = str.length();
    if(width < len) { return str; }

    int diff = width - len;
    int pad1 = diff/2;
    int pad2 = diff - pad1;
    return std::string(pad1, *fillchar) + str + std::string(pad2, *fillchar);
}

template <typename Board>
void utils::print_board(Board &board) {

    int H_SIZE_PER_PIECE = 7;
    int V_SIZE_PER_PIECE = 7;

    int dim = board.get_board_len();
    unsigned short multiplier = 2;
    if(dim == 7)
        multiplier = 3;
    else if(dim == 10)
        multiplier = 4;
    else
        throw std::invalid_argument("Board dimension not supported.");

    std::stringstream board_print;
    // column width for the row index plus vertical dash
    board_print << std::string(4, ' ');
    // print the column index rows
    for(int i = 0; i < dim * multiplier; ++i) {
        board_print << center(std::to_string(i), SIZE_PER_PIECE, " ");
    }
    board_print << "\n" << std::string(dim * SIZE_PER_PIECE, '-')  << "\n";


    std::string output = board_print.str();
    std::cout << output << std::endl;
}