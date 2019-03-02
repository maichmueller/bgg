#include <iostream>
#include "Board.h"
#include "Piece.h"
#include "GameState.h"
#include "vector"
#include "random"

int main() {
    int board_dim = 5;
    //std::cout << "Insert game dimension:";
    //std::cin >> board_dim;

    Board board(board_dim);
    bool a = 3 < 5 < 7;
    std::cout << "Reached here.";

    return 0;
}