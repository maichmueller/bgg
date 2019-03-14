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
    array<map<int, int>, 2> dead_pieces;

    int terminal;
    bool terminal_checked;

    int move_count;

    vector<vector<pos_type >> move_history;
    vector<vector<shared_ptr<Piece>>> piece_history;
    vector<bool> move_equals_prev_move;
    unsigned int rounds_without_fight;

    bool canonical_teams;

public:
    explicit GameState(int game_len);
    GameState(Board& board, int move_count=0);
    GameState(Board& board, array<map<int, int>, 2>& dead_pieces, int move_count);
    void check_terminal(bool flag_only=false, int turn=0);
    int do_move(vector<pos_type>& move);
    int fight(Piece& attacker, Piece& defender);
    int is_terminal(bool force_reload=false, int turn=0);

    void inc_move_count() {move_count += 1;}

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
