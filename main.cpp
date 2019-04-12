#include <iostream>
#include "Board.h"
#include "Piece.h"
#include "GameState.h"
#include "Game.h"
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

    // Board board_from_setups(board_len, setup_0, setup_1);

    std::cout << "Initialized boards." << endl;

    std::random_device r;
    for(int i=0; i < 50; ++i) {
        std::cout<< r() << endl;
    }

//
//    auto ag0 = std::make_shared<RandomAgent<>> (0);
//    auto ag1 = std::make_shared<RandomAgent<>> (1);
//    Game game(board_len, ag0, ag1);
//    for(int i = 0; i < 50; ++i) {
//        std::cout << "Game: " << i << std::endl;
//        game.run_game(false);
//        game.reset();
//        //utils::print_board<Board, Piece>(*game.get_gamestate().get_board());
//    }

    return 0;
}