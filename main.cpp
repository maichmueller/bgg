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
#include "ActionRepresentation.h"

#include <iostream>
#include <vector>
#include <set>
#include <type_traits>
#include "torch_utils.h"
#include "AgentReinforce.h"


int main(int argc, char const *argv[])
{
    printf("\n");
    printf("\x1B[41m\x1B[30mTexting\033[0m\t\t");
    printf("\x1B[32mTexting\033[0m\t\t");
    printf("\x1B[33mTexting\033[0m\t\t");
    printf("\x1B[34mTexting\033[0m\t\t");
    printf("\x1B[35mTexting\033[0m\n");

    printf("\x1B[36mTexting\033[0m\t\t");
    printf("\x1B[36mTexting\033[0m\t\t");
    printf("\x1B[36mTexting\033[0m\t\t");
    printf("\x1B[37mTexting\033[0m\t\t");
    printf("\x1B[93mTexting\033[0m\n");

    printf("\033[3;42;30mTexting\033[0m\t\t");
    printf("\033[3;43;30mTexting\033[0m\t\t");
    printf("\033[3;44;30mTexting\033[0m\t\t");
    printf("\033[3;104;30mTexting\033[0m\t\t");
    printf("\033[3;100;30mTexting\033[0m\n");

    printf("\033[3;47;35mTexting\033[0m\t\t");
    printf("\033[2;47;35mTexting\033[0m\t\t");
    printf("\033[1;47;35mTexting\033[0m\t\t");
    printf("\t\t");
    printf("\n");
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
    std::map<pos_t, int > setup0;
    std::map<pos_t, int > setup1;
    setup0[{0,0}] = 0;
    setup0[{0,1}] = 1;
    setup0[{0,2}] = 2;
    setup0[{0,3}] = 2;
    setup0[{0,4}] = 3;
    setup0[{1,0}] = 11;
    setup0[{1,1}] = 10;
    setup0[{1,2}] = 2;
    setup0[{1,3}] = 11;
    setup0[{1,4}] = 3;
    setup1[{3,0}] = 2;
    setup1[{3,1}] = 2;
    setup1[{3,2}] = 11;
    setup1[{3,3}] = 2;
    setup1[{3,4}] = 0;
    setup1[{4,0}] = 3;
    setup1[{4,1}] = 1;
    setup1[{4,2}] = 11;
    setup1[{4,3}] = 3;
    setup1[{4,4}] = 10;

    auto game = std::make_shared<Game>(5, agent_0, agent_1, setup0, setup1);
//    auto * board = game->get_gamestate()->get_board();
//    std::cout  << utils::board_str_rep<Board, Piece>(*board, false, false) << "\n";
//    auto valids = StrategoLogic::get_action_mask(
//            *board,
//            ActionRep::get_act_rep(board->get_board_len()),
//            ActionRep::get_act_map(board->get_board_len()),
//            0);
//    for(int i = 0; i < valids.size(); ++i) {
//        move_t move = game->get_gamestate()->action_to_move(i, 0);
//        std::cout << "(" << move[0][0] << ", " << move[0][1] << ") -> (" << move[1][0] << ", " << move[1][1] << ") \t valid: " << valids[i] << "\n";
//    }
//    auto action_mask = StrategoLogic::get_action_mask(
//            *board,
//            ActionRep::get_act_rep(board->get_board_len()),
//            ActionRep::get_act_map(board->get_board_len()),
//            0);

    Coach coach(game, network_0, network_1);
    coach.teach(false, false, false, false);

    return 0;
}