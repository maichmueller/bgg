#include <iostream>
#include "Board.h"
#include "Piece.h"
#include "GameState.h"
#include "vector"
#include "random"

int main() {
    int board_len = 5;

    std::map<pos_type, int> setup_0;
    std::map<pos_type, int> setup_1;

    setup_0[{0,0}] = 0;
    setup_0[{0,1}] = 11;
    setup_0[{0,2}] = 3;
    setup_0[{0,3}] = 10;
    setup_0[{0,4}] = 1;
    setup_0[{1,0}] = 2;
    setup_0[{1,1}] = 2;
    setup_0[{1,2}] = 11;
    setup_0[{1,3}] = 3;
    setup_0[{1,4}] = 2;

    setup_1[{4,0}] = 0;
    setup_1[{4,1}] = 11;
    setup_1[{4,2}] = 3;
    setup_1[{4,3}] = 10;
    setup_1[{4,4}] = 1;
    setup_1[{3,0}] = 2;
    setup_1[{3,1}] = 2;
    setup_1[{3,2}] = 11;
    setup_1[{3,3}] = 3;
    setup_1[{3,4}] = 2;

    Board board_std(board_len);
    Board board_from_setups(board_len, setup_0, setup_1);

    auto p = board_std[{2,1}];
    pos_type new_pos = {1,1};
    board_std.update_board(new_pos, p);
    std::cout << "Initialized boards.";

    return 0;
}