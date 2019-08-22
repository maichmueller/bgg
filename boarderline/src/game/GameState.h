//
// Created by Michael on 21/02/2019.
//

#ifndef STRATEGO_CPP_GAMESTATE_H
#define STRATEGO_CPP_GAMESTATE_H

#include "../board/Board.h"
#include "../board/Piece.h"
#include "GameDeclarations.h"
#include "../logic/StrategoLogic.h"
#include "unordered_map"
#include "torch/torch.h"

using cond_type = std::tuple<int, int, int, bool>;

class GameState {

    Board board;
    std::array<std::map<int, int>, 2> dead_pieces;

    int terminal;
    bool terminal_checked;

    int move_count;

    using ii_key = std::tuple<int, int>;
    using ii_hasher = hash_tuple::hash<ii_key >;
    using ii_eq_comp = eqcomp_tuple::eqcomp<ii_key >;
    using ii_ptr_map = std::unordered_map<ii_key, std::shared_ptr<Piece>, ii_hasher, ii_eq_comp>;
    std::array<ii_ptr_map, 2> actors{ii_ptr_map{}, ii_ptr_map{}};

    std::vector<cond_type> conditions_torch_rep;
    bool conditions_set = false;

    std::vector<strat_move_t> move_history;
    std::vector<std::array<std::shared_ptr<Piece>, 2>> piece_history;
    std::vector<bool> move_equals_prev_move;
    unsigned int rounds_without_fight;

    bool canonical_teams;

    void assign_actors(const Board& board);

public:
    explicit GameState(int game_len);
    explicit GameState(Board board, int move_count=0);
    GameState(Board board, std::array<std::map<int, int>, 2>& dead_pieces, int move_count);
    GameState(int len, const std::map<Position, int>& setup_0, const std::map<Position, int>& setup_1);
    void check_terminal(bool flag_only=false, int turn=0);
    int do_move(strat_move_t& move);
    int fight(Piece& attacker, Piece& defender);
    int is_terminal(bool force_check=false, int turn=0);

    torch::Tensor torch_represent(int player);
    strat_move_t action_to_move(int actionl, int player) const;

    void restore_to_round(int round);
    void undo_last_rounds(int n=1);

    int get_canonical_team(Piece& piece);
    Position get_canonical_pos(Piece& piece);
    int get_move_count() {return move_count;}
    bool is_canonical() {return canonical_teams;}

    void canonical_board(int player);
    void set_board(Board brd) {this->board = std::move(brd);}
    Board const * get_board() const {return &board;}
};


#endif //STRATEGO_CPP_GAMESTATE_H
