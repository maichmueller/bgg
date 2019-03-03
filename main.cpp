#include <iostream>
#include "Board.h"
#include "Piece.h"
#include "GameState.h"
#include "vector"
#include "random"

int main() {
    int board_len = 5;
    //std::cout << "Insert game lenension:";
    //std::cin >> board_len;

    Board board(board_len);
    bool a = 3 < 5 < 7;
    std::cout << "Reached here.";

    return 0;
}