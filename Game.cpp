//
// Created by Michael on 06/03/2019.
//

#include "Game.h"


Game::Game(int board_l, const std::shared_ptr<Agent>& ag0, const std::shared_ptr<Agent>& ag1, bool f_setups)
    : board_len(board_l), game_state(board_l),
      agent_0(ag0), agent_1(ag1), fixed_setups(f_setups),
      setup_0(), setup_1()
{
    reset();
}

Game::Game(int board_l, const std::shared_ptr<Agent>& ag0, const std::shared_ptr<Agent>& ag1,
           const std::map<pos_type, int>& setup_0,
           const std::map<pos_type, int>& setup_1)
    : board_len(board_l), game_state(board_l, setup_0, setup_1),
      agent_0(ag0), agent_1(ag1), fixed_setups(true),
      setup_0(setup_0), setup_1(setup_1)
{}

Game::Game(int board_len, const std::shared_ptr<Agent>& ag0, const std::shared_ptr<Agent>& ag1, Board &board)
    : board_len(board_len), agent_0(ag0), agent_1(ag1), game_state(board, 0)
{}

int Game::run_game(bool show=true) {
    int game_over = false;
    int terminal = 404;
    std::function<void(Board&, bool, bool)> print_board = [](Board& board, bool n, bool m) -> void {return;};
    if(show)
        print_board = &utils::print_board<Board, Piece>;

    while(!game_over) {
        print_board(*game_state.get_board(), false, false);

        // test for terminality
        terminal = game_state.is_terminal();

        std::cout << "Status: " << terminal << std::endl;

        if(terminal != 404)
            game_over = true;
        else
            run_step();

    }
    print_board(*game_state.get_board(), false, false);

    std::cout << "Status: " << game_state.is_terminal() << std::endl;
    return terminal;
}

void Game::run_step() {
    int turn = game_state.get_move_count() % 2;  // # player 1 or player 0
    
    std::vector<pos_type > move;
    if(turn == 1)
        move = agent_1->decide_move(*game_state.get_board());
    else
        move = agent_0->decide_move(*game_state.get_board());

    game_state.do_move(move);
    std::cout << "Move: (" << move[0][0] << ", " << move[0][1] << ") -> (" << move[1][0] << ", " << move[1][1] << ")" << std::endl;

}


void Game::reset() {
    auto setup_team_0 = setup_0;
    auto setup_team_1 = setup_1;
    if(!fixed_setups) {
        setup_team_0 = draw_random_setup(0);
        setup_team_1 = draw_random_setup(1);
    }
    game_state = GameState(board_len, setup_team_0, setup_team_1);
}


std::map<pos_type, int> Game::draw_random_setup(int team) {
    auto avail_types = GameDeclarations::get_available_types(board_len);

    std::vector<pos_type > poss_pos = GameDeclarations::get_start_positions(board_len, team);

    std::map<pos_type, int > setup_out;

    std::random_device rd;
    std::mt19937 rng(rd());
    std::shuffle(poss_pos.begin(), poss_pos.end(), rng);
    std::shuffle(avail_types.begin(), avail_types.end(), rng);

    while(!poss_pos.empty()) {
        auto& pos = poss_pos.back();
        auto& type = avail_types.back();

        setup_out[pos] = type;

        poss_pos.pop_back();
        avail_types.pop_back();
    }

    return setup_out;
}
