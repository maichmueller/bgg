#include <iostream>
#include <azpp/azpp.h>

#include "game/StateStratego.h"
#include "torch/torch.h"
#include "board/PieceStratego.h"
#include "game/GameStratego.h"
#include "nn/model/representation/ActionRepresenterStratego.h"
#include <nn/model/modules/AlphazeroStratego.h>

#include<iostream>
#include<memory>
#include <array>


int main(int argc, char const *argv[]) {
    size_t board_size = 5;
    auto action_rep_sptr = std::make_shared<ActionRepStratego>(5);
    std::vector<int> filters{128, 128, 128, 128};
    auto alphazero_net_ptr = std::make_shared<StrategoAlphaZero>(
            board_size * board_size * filters.front(),
            action_rep_sptr->get_actions_vec().size(),
            5,
            10,
            action_rep_sptr->get_conditions().size(),
            filters,
            std::vector<int>{3, 3, 3, 3},
            std::vector<bool>{false, false, false, false},
            std::vector<float>{0.0, 0.0, 0.0, 0.0}
    );

    auto network_0 = std::make_shared<NetworkWrapper>(alphazero_net_ptr);
    auto network_1 = std::make_shared<NetworkWrapper>(*network_0);
    auto agent_0 = std::make_shared<AlphaZeroAgent<StateStratego, ActionRepStratego>>(
            0,
            network_0,
            action_rep_sptr
    );
    auto agent_1 = std::make_shared<AlphaZeroAgent<StateStratego, ActionRepStratego>>(
            1,
            network_1,
            action_rep_sptr
    );
    std::map<BoardStratego::position_type, BoardStratego::kin_type > setup0;
    std::map<BoardStratego::position_type, BoardStratego::kin_type > setup1;

    setup0[{0,0}] = {0, 0};
    setup0[{0,1}] = {1, 0};
    setup0[{0,2}] = {2, 0};
    setup0[{0,3}] = {2, 1};
    setup0[{0,4}] = {3, 0};
    setup0[{1,0}] = {11, 0};
    setup0[{1,1}] = {10, 0};
    setup0[{1,2}] = {2, 2};
    setup0[{1,3}] = {11, 1};
    setup0[{1,4}] = {3, 1};
    setup1[{3,0}] = {2, 0};
    setup1[{3,1}] = {2, 1};
    setup1[{3,2}] = {11, 0};
    setup1[{3,3}] = {2, 2};
    setup1[{3,4}] = {0, 0};
    setup1[{4,0}] = {3, 0};
    setup1[{4,1}] = {1, 0};
    setup1[{4,2}] = {11, 1};
    setup1[{4,3}] = {3, 0};
    setup1[{4,4}] = {10, 0};

    auto g = GameStratego(std::array<size_t, 2>{5, 5},
                          setup0,
                          setup1,
                          agent_0,
                          agent_1,
                          true);
    auto game = std::make_shared<GameStratego>(
            std::array<size_t , 2>{5, 5},
            setup0,
            setup1,
            agent_0,
            agent_1,
            true);


//    Coach coach(game, network_0, network_1);
//    coach.teach(false, false, false, false);

    return 0;
}