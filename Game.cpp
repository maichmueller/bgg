//
// Created by Michael on 06/03/2019.
//

#include "Game.h"


Game::Game(int board_l, const std::shared_ptr<Agent>& ag0, const std::shared_ptr<Agent>& ag1, bool f_setups)
    : board_len(board_l), game_state(board_l),
      agent_0(ag0), agent_1(ag1), fixed_setups(f_setups),
      setup_0(0), setup_1(0)
{}

Game::Game(int board_l, const std::shared_ptr<Agent>& ag0, const std::shared_ptr<Agent>& ag1,
           const std::vector<std::shared_ptr<Piece>>& setup_0,
           const std::vector<std::shared_ptr<Piece>>& setup_1)
    : board_len(board_l), game_state(board_l),
      agent_0(ag0), agent_1(ag1), fixed_setups(true),
      setup_0(setup_0), setup_1(setup_1)
{
    Board board(board_len);

    for(auto const & piece : setup_0)
        board[piece->get_position()] = piece;

    for(auto const & piece : setup_1)
        board[piece->get_position()] = piece;

    game_state = GameState(board);
}

Game::Game(int board_len, const std::shared_ptr<Agent>& ag0, const std::shared_ptr<Agent>& ag1, Board &board)
    : board_len(board_len), agent_0(ag0), agent_1(ag1), game_state(board, 0)

{}

int Game::run_game(bool show=true) {
    int game_over = false;
    int rewards = 404;
    std::function<void(Board&)> print_board = [](Board& board) -> void {return;};
    if(show)
        print_board = &utils::print_board<Board, Piece>;

    while(!game_over) {
        print_board(*game_state.get_board());
        rewards = run_step();
        if(rewards != 404)
            game_over = true;
    }
    print_board(*game_state.get_board());
    return rewards;
}

int Game::run_step() {
    int turn = game_state.get_move_count() % 2;  // # player 1 or player 0
    
    vector<pos_type > move;
    if(turn == 1)
        move = agent_1->decide_move(*game_state.get_board());
    else
        move = agent_0->decide_move(*game_state.get_board());

    game_state.do_move(move);
    // test for terminality
    int terminal = game_state.is_terminal();
    return terminal;
}
