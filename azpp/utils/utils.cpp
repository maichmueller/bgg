
#include "utils.h"


std::map<std::array<int, 2>, int> utils::BattleMatrix::initialize_battle_matrix() {
    std::map<std::array<int, 2>, int> bm;
    for(int i = 1; i < 11; ++i) {
        bm[{i, i}] = 0;
        for(int j = i+1; j < 11; ++j) {
            bm[{i, j}] = -1;
            bm[{j, i}] = 1;
        }
        bm[{i, 0}] = 1;
        if(i == 3)
            bm[{i, 11}] = 1;
        else
            bm[{i, 11}] = -1;
    }
    bm[{1, 10}] = 1;
    return bm;
}

const std::map<std::array<int, 2>, int> utils::BattleMatrix::battle_matrix = initialize_battle_matrix();

