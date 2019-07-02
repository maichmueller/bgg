#include <iostream>
#include "Board.h"
#include "Piece.h"
#include "GameState.h"
#include "Game.h"
#include "NeuralNetwork.h"
#include "Coach.h"
#include "vector"
#include "random"
#include "torch/torch.h"

#include <iostream>
#include <vector>
#include <set>
#include <type_traits>
#include "torch_utils.h"
#include "AgentReinforce.h"


int main(int argc, char const *argv[])
{
    int board_size = 5;
    int action_dim = ActionRep::get_act_rep(board_size).size();
    StateRepresentation::set_state_rep_conditions(board_size);
    std::vector<int> filters{128, 128, 128, 128};
    auto alphazero_net_ptr = std::make_shared<StrategoAlphaZero>(
            /*D_in=*/board_size * board_size * filters.front(),
            /*D_out=*/action_dim,
            /*nr_lin_layers=*/5,
            /*start_exponent=*/10,
            /*channels_in=*/StateRepresentation::state_torch_conv_conditions_0.size(),
            filters,
            std::vector<int> {3, 3, 3, 3},
            std::vector<bool> {false, false, false, false},
            std::vector<float> {0.0, 0.0, 0.0, 0.0});

    auto network_0 = std::make_shared<NetworkWrapper>(alphazero_net_ptr, board_size, action_dim);
    auto network_1 = std::make_shared<NetworkWrapper>(*network_0);
    auto agent_0 = std::make_shared<AlphaZeroAgent>(0, true, network_0);
    auto agent_1 = std::make_shared<AlphaZeroAgent>(1, true, network_1);
    auto game = std::make_shared<Game>(5, agent_0, agent_1);
    Coach coach(game, network_0, network_1);
    coach.teach(false, false, false, false);

    return 0;
}