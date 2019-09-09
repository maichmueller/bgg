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

struct ActionRepNull {
    static void enable_representation() {}
};

template <class Board, class ActionRepType = ActionRepNull>
class GameState {

public:
    using board_type = Board;
    using piece_type = typename Board::piece_type;
    using kin_type = typename Board::kin_type;
    using position_type = typename Board::position_type;
    using move_type = Move<position_type >;

protected:
    friend ActionRepType;
    ActionRepType m_action_rep;
    board_type m_board;

    using dead_pieces_type = std::array<std::vector<typename piece_type::R>, 2>;
    dead_pieces_type m_dead_pieces;

    int m_terminal;
    bool terminal_checked;

    int move_count;

    std::vector<move_type> move_history;
    std::vector<std::array<std::shared_ptr<piece_type>, 2>> piece_history;
    std::vector<bool> move_equals_prev_move;
    unsigned int rounds_without_fight;

    bool canonical_teams;

public:
    template <size_t dim>
    explicit GameState(const std::array<int, dim> &shape,
                       const std::array<int, dim> &board_starts);

    explicit GameState(const board_type & board,
                       dead_pieces_type & dead_pieces = dead_pieces_type(),
                       int move_count = 0,
                       ActionRepType && action_rep = ActionRepType());

    template <size_t dim>
    GameState(const std::array<int, dim> &shape,
              const std::array<int, dim> &board_starts,
              const std::map<position_type, typename piece_type::kin_type>& setup_0,
              const std::map<position_type, typename piece_type::kin_type>& setup_1);

    int is_terminal(bool force_check=false);
    virtual void check_terminal() = 0;
    virtual int do_move(const Move<position_type>& move) = 0;

    void register_action_rep(ActionRepType action_rep) {action_rep = std::move(action_rep);}
    torch::Tensor state_representation(int player);
    move_type action_to_move(int action, int player) const;

    void restore_to_round(int round);
    void undo_last_rounds(int n=1);

    int get_canonical_team(piece_type& piece);
    position_type get_canonical_pos(piece_type& piece);
    int get_move_count() {return move_count;}
    bool is_canonical() {return canonical_teams;}

    void canonical_board(int player);
    void set_board(board_type brd) {this->m_board = std::move(brd);}
    board_type const * get_board() const {return &m_board;}
};

template <class Board, class ActionRepType>
GameState<Board, ActionRepType>::GameState(const board_type & board,
                                           dead_pieces_type & dead_pieces,
                                           int move_count,
                                           ActionRepType && action_rep)
       : m_board(board),
         m_dead_pieces(std::move(dead_pieces)),
         move_count(move_count),
         terminal_checked(false),
         m_terminal(404),
         canonical_teams(true),
         rounds_without_fight(0),
         move_equals_prev_move(0),
         move_history(0),
         m_action_rep(std::move(action_rep))
{
    check_terminal();
    m_action_rep.enable_representation(this);
}

template <class Board, class ActionRepType>
template <size_t dim>
GameState<Board, ActionRepType>::GameState(const std::array<int, dim> &shape,
                                           const std::array<int, dim> &board_starts)
        : m_board(shape, board_starts),
          GameState(m_board)
{}

template <class Board, class ActionRepType>
template <size_t dim>
GameState<Board, ActionRepType>::GameState(const std::array<int, dim> &shape,
                                           const std::array<int, dim> &board_starts,
                                           const std::map<position_type, typename piece_type::kin_type>& setup_0,
                                           const std::map<position_type, typename piece_type::kin_type>& setup_1)
        : m_board(shape, board_starts, setup_0, setup_1),
          GameState(m_board)
{}




template <class Board, class ActionRepType>
int GameState<Board, ActionRepType>::is_terminal(bool force_check) {
    if(!terminal_checked || force_check)
        check_terminal(false);
    return m_terminal;
}


template <class Board, class ActionRepType>
void GameState<Board, ActionRepType>::canonical_board(int player) {
    // if the 0 player is team 1, then canonical is false,
    // if it is 0 otherwise, then the teams are canonical
    canonical_teams = bool(1 - player);
}


template <class Board, class ActionRepType>
int GameState<Board, ActionRepType>::get_canonical_team(piece_type & piece){
    if(canonical_teams) {
        return piece.get_team();
    }
    else {
        return 1 - piece.get_team();
    }
}

// TODO: Update to new position type

template <class Board, class ActionRepType>
typename GameState<Board, ActionRepType>::position_type GameState<Board, ActionRepType>::get_canonical_pos(piece_type & piece){
    if(canonical_teams) {
        return piece.get_position();
    }
    else {
        int len = m_board.get_shape();
        Position pos = piece.get_position();
        pos[0] = len-1-pos[0];
        pos[1] = len-1-pos[1];
        return pos;
    }
}


template <class Board, class ActionRepType>
void GameState<Board, ActionRepType>::undo_last_rounds(int n) {
    for(int i = 0; i < n; ++i) {
        move_type move = move_history.back();
        auto move_pieces = piece_history.back();

        move_history.pop_back();
        piece_history.pop_back();
        move_equals_prev_move.pop_back();

        Position from = move[0];
        Position to = move[1];
        m_board.update_board(from, move_pieces[0]);
        m_board.update_board(to, move_pieces[1]);
        for(const auto& piece: move_pieces) {
            m_action_rep.update_actors(piece->get_team(), piece->get_kin());
        }
    }
    move_count -= n;
}


template <class Board, class ActionRepType>
void GameState<Board, ActionRepType>::restore_to_round(int round) {
    undo_last_rounds(move_count - round);
}

template <class Board, class ActionRepType>
torch::Tensor GameState<Board, ActionRepType>::state_representation(int player) {
    return m_action_rep.state_representation(player);
}

template <class Board, class ActionRepType>
typename GameState<Board, ActionRepType>::move_type
GameState<Board, ActionRepType>::action_to_move(int action, int player) const {
    return m_action_rep.action_to_move(action, player);
}

