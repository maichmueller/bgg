//
// Created by Michael on 21/02/2019.
//

#ifndef STRATEGO_CPP_GAMESTATE_H
#define STRATEGO_CPP_GAMESTATE_H

#include "Board.h"
#include "Piece.h"
#include "GameDeclarations.h"
#include "StrategoLogic.h"
#include "map"


class GameState {

    Board board;
    std::array<std::map<int, int>, 2> dead_pieces;

    int terminal;
    bool terminal_checked;

    int move_count;

    std::vector<move_type> move_history;
    std::vector<std::array<std::shared_ptr<Piece>, 2>> piece_history;
    std::vector<bool> move_equals_prev_move;
    unsigned int rounds_without_fight;

    bool canonical_teams;

public:
    explicit GameState(int game_len);
    GameState(const Board& board, int move_count=0);
    GameState(const Board& board, std::array<std::map<int, int>, 2>& dead_pieces, int move_count);
    GameState(int len, const std::map<pos_type, int>& setup_0, const std::map<pos_type, int>& setup_1);
    void check_terminal(bool flag_only=false, int turn=0);
    int do_move(move_type& move);
    int fight(Piece& attacker, Piece& defender);
    int is_terminal(bool force_check=false, int turn=0);

    void restore_to_round(int round);
    void undo_last_n_rounds(int n);

    int get_canonical_team(Piece& piece);
    pos_type get_canonical_pos(Piece& piece);
    int get_move_count() {return move_count;}
    bool is_canonical() {return canonical_teams;}

    void canonical_board(int player);
    void set_board(Board board) {this->board = std::move(board);}
    Board* get_board() {return &board;}
};


#endif //STRATEGO_CPP_GAMESTATE_H
