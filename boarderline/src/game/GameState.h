//
// Created by Michael on 21/02/2019.
//

#pragma once

#include "../board/Board.h"
#include "../board/Piece.h"
#include "../board/Move.h"
#include "GameUtilsStratego.h"
#include "../logic/StrategoLogic.h"
#include "unordered_map"
#include "torch/torch.h"


template <class Board>
class GameState {

public:
    using board_type = Board;
    using piece_type = typename Board::piece_type;
    using position_type = typename Board::position_type;
    using move_type = Move<position_type >;
    using cond_type = std::tuple<int, int, int, bool>;

protected:
    board_type board;
    std::array<std::map<int, int>, 2> dead_pieces;

    int terminal;
    bool terminal_checked;

    int move_count;

    using ii_key = std::tuple<int, int>;
    using ii_hasher = hash_tuple::hash<ii_key >;
    using ii_eq_comp = eqcomp_tuple::eqcomp<ii_key >;
    using ii_ptr_map = std::unordered_map<ii_key, std::shared_ptr<piece_type>, ii_hasher, ii_eq_comp>;
    std::array<ii_ptr_map, 2> actors{ii_ptr_map{}, ii_ptr_map{}};

    std::vector<cond_type> conditions_torch_rep;
    bool conditions_set = false;

    std::vector<move_type> move_history;
    std::vector<std::array<std::shared_ptr<piece_type>, 2>> piece_history;
    std::vector<bool> move_equals_prev_move;
    unsigned int rounds_without_fight;

    bool canonical_teams;

    void assign_actors(const board_type & brd);

public:
    explicit GameState(int game_len);
    explicit GameState(const board_type & board, int move_count=0);
    GameState(const board_type & board, std::array<std::map<int, int>, 2>& dead_pieces, int move_count);
    GameState(int len, const std::map<position_type, int>& setup_0, const std::map<position_type, int>& setup_1);
    virtual void check_terminal();
    virtual int do_move(const Move<position_type>& move) = 0;
    int is_terminal(bool force_check=false);

    torch::Tensor torch_represent(int player);
    move_type action_to_move(int action, int player) const;

    void restore_to_round(int round);
    void undo_last_rounds(int n=1);

    int get_canonical_team(piece_type& piece);
    position_type get_canonical_pos(piece_type& piece);
    int get_move_count() {return move_count;}
    bool is_canonical() {return canonical_teams;}

    void canonical_board(int player);
    void set_board(board_type brd) {this->board = std::move(brd);}
    board_type const * get_board() const {return &board;}
};

template <class Board>
GameState<Board>::GameState(const board_type & board, std::array<std::map<int, int>, 2>& dead_pieces, int move_count)
        : board(board), dead_pieces(dead_pieces), move_count(move_count),
          terminal_checked(false), terminal(404), canonical_teams(true), rounds_without_fight(0),
          move_equals_prev_move(0), move_history(0)
{
    assign_actors(this->board);
}

template <class Board>
GameState<Board>::GameState(const board_type & board, int move_count)
        : GameState(board, {std::map<int, int>(), std::map<int, int>()}, move_count)
{}

template <class Board>
GameState<Board>::GameState(int len, const std::map<position_type, int>& setup_0, const std::map<position_type, int>& setup_1)
        : board(len, setup_0, setup_1),
          GameState(board, 0)
{}

template <class Board>
GameState<Board>::GameState(int game_len)
        : board(game_len),
          GameState(board, 0)
{}

template <class Board>
void GameState<Board>::assign_actors(const board_type &brd) {
    for(const auto& entry: brd) {
        const auto& piece = entry.second;
        if(!piece->is_null() && piece->get_type() != 99)
            actors[piece->get_team()][std::make_tuple(piece->get_type(), piece->get_version())] = piece;
    }
}

template <class Board>
int GameState<Board>::is_terminal(bool force_check) {
    if(!terminal_checked || force_check)
        check_terminal(false);
    return terminal;
}

template <class Board>
void GameState<Board>::canonical_board(int player) {
    // if the 0 player is team 1, then canonical is false,
    // if it is 0 otherwise, then the teams are canonical
    canonical_teams = bool(1 - player);
}

template <class Board>
int GameState<Board>::get_canonical_team(piece_type & piece){
    if(canonical_teams) {
        return piece.get_team();
    }
    else {
        return 1 - piece.get_team();
    }
}

// TODO: Update to new position type
template <class Board>
typename GameState<Board>::position_type GameState<Board>::get_canonical_pos(piece_type & piece){
    if(canonical_teams) {
        return piece.get_position();
    }
    else {
        int len = board.get_shape();
        Position pos = piece.get_position();
        pos[0] = len-1-pos[0];
        pos[1] = len-1-pos[1];
        return pos;
    }
}

template <class Board>
void GameState<Board>::undo_last_rounds(int n) {
    for(int i = 0; i < n; ++i) {
        move_type move = move_history.back();
        auto move_pieces = piece_history.back();

        move_history.pop_back();
        piece_history.pop_back();
        move_equals_prev_move.pop_back();

        Position from = move[0];
        Position to = move[1];
        board.update_board(from, move_pieces[0]);
        board.update_board(to, move_pieces[1]);
        for(auto& piece: move_pieces) {
            int type = piece->get_type();
            int version = piece->get_version();
            if(0 < type && type < 11 && version != -1)
                actors[piece->get_team()][{piece->get_type(), piece->get_version()}] = piece;
        }
    }
    move_count -= n;
}

template <class Board>
void GameState<Board>::restore_to_round(int round) {
    undo_last_rounds(move_count - round);
}

template <class Board>
torch::Tensor GameState<Board>::torch_represent(int player) {
    if(!conditions_set) {
        auto type_counter = utils::counter(GameDeclarations::get_available_types(board.get_shape()));
        conditions_torch_rep = StateRepresentation::create_conditions(type_counter, 0);
        conditions_set = true;
    }

    return StateRepresentation::b2s_cond_check(
            board,
            conditions_torch_rep,
            player);
}

template <class Board>
typename GameState<Board>::move_type
GameState<Board>::action_to_move(int action, int player) const {
    int board_len = board.get_shape();
    int action_dim = ActionRep::get_act_rep(board_len).size();
    return ActionRep::action_to_move(action, action_dim, board_len, actors.at(player), player);
