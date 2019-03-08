//
// Created by Michael on 08/03/2019.
//

#ifndef STRATEGO_CPP_UTILS_H
#define STRATEGO_CPP_UTILS_H

#include "string"
#include "iostream"
#include "iomanip"
#include <sstream>

struct utils {

    static std::string center(const std::string &str, int width, const char* fillchar);
    template <typename Board>
    static void print_board(Board& board);
};


#endif //STRATEGO_CPP_UTILS_H
