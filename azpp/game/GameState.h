//
// Created by Michael on 21/02/2019.
//

#pragma once

#include "../board/Board.h"
#include "../board/Piece.h"
#include "../board/Move.h"
#include "../board/Position.h"
#include "GameUtilsStratego.h"
//#include "../logic/LogicStratego.h"
#include "unordered_map"


template<class Board>
class GameState {

public:
    using board_type = Board;
    using piece_type = typename Board::piece_type;
    using kin_type = typename Board::kin_type;
    using position_type = typename Board::position_type;
    using move_type = Move<position_type>;

protected:
    board_type m_board;

    using dead_pieces_type = std::array<std::vector<typename piece_type::kin_type>, 2>;
    dead_pieces_type m_dead_pieces;

    int m_terminal;
    bool m_terminal_checked;

    int m_move_count;

    std::vector<move_type> m_move_history;
    std::vector<std::array<std::shared_ptr<piece_type>, 2>> m_piece_history;
    std::vector<bool> m_move_equals_prev_move;
    unsigned int m_rounds_without_fight;

    bool m_canonical_teams;

public:
    template<size_t dim>
    explicit GameState(const std::array<size_t, dim> &shape,
                       const std::array<int, dim> &board_starts);

    explicit GameState(board_type &&board,
                       int move_count = 0);

    explicit GameState(const board_type &board,
                       int move_count = 0);

    template<size_t dim>
    GameState(const std::array<size_t, dim> &shape,
              const std::array<int, dim> &board_starts,
              const std::map<position_type, typename piece_type::kin_type> &setup_0,
              const std::map<position_type, typename piece_type::kin_type> &setup_1);

    int is_terminal(bool force_check = false);

    virtual void check_terminal() = 0;

    virtual int do_move(const move_type &move) = 0;

    void restore_to_round(int round);

    void undo_last_rounds(int n = 1);

    int get_canonical_team(piece_type &piece);

    position_type get_canonical_pos(piece_type &piece);

    int get_move_count() { return m_move_count; }

    bool is_canonical() { return m_canonical_teams; }

    void canonical_board(int player);

    void set_board(board_type brd) { this->m_board = std::move(brd); }

    board_type const *get_board() const { return &m_board; }
};

template<class Board>
GameState<Board>::GameState(board_type &&board,
                            int move_count)
        : m_board(std::move(board)),
          m_dead_pieces(),
          m_move_count(move_count),
          m_terminal_checked(false),
          m_terminal(404),
          m_canonical_teams(true),
          m_rounds_without_fight(0),
          m_move_equals_prev_move(0),
          m_move_history(0) {
    check_terminal();
}

template<class Board>
GameState<Board>::GameState(const board_type &board,
                            int move_count)
        : GameState(board_type(board), move_count) {}

template<class Board>
template<size_t dim>
GameState<Board>::GameState(const std::array<size_t, dim> &shape,
                            const std::array<int, dim> &board_starts)
        : GameState(board_type(shape, board_starts)) {}

template<class Board>
template<size_t dim>
GameState<Board>::GameState(const std::array<size_t, dim> &shape,
                            const std::array<int, dim> &board_starts,
                            const std::map<position_type, typename piece_type::kin_type> &setup_0,
                            const std::map<position_type, typename piece_type::kin_type> &setup_1)
        : GameState(board_type(shape, board_starts, setup_0, setup_1)) {}

template<class Board>
int GameState<Board>::is_terminal(bool force_check) {
    if (!m_terminal_checked || force_check)
        check_terminal(false);
    return m_terminal;
}

template<class Board>
void GameState<Board>::canonical_board(int player) {
    // if the 0 player is team 1, then canonical is false,
    // if it is 0 otherwise, then the teams are canonical
    m_canonical_teams = bool(1 - player);
}

template<class Board>
int GameState<Board>::get_canonical_team(piece_type &piece) {
    if (m_canonical_teams) {
        return piece.get_team();
    } else {
        return 1 - piece.get_team();
    }
}

template<class Board>
typename GameState<Board>::position_type
GameState<Board>::get_canonical_pos(piece_type &piece) {
    if (m_canonical_teams) {
        return piece.get_position();
    } else {
        int len = m_board.get_shape();
        position_type pos = piece.get_position();
        pos[0] = len - 1 - pos[0];
        pos[1] = len - 1 - pos[1];
        return pos;
    }
}

template<class Board>
void GameState<Board>::undo_last_rounds(int n) {
    for (int i = 0; i < n; ++i) {
        move_type move = m_move_history.back();
        auto move_pieces = m_piece_history.back();

        m_move_history.pop_back();
        m_piece_history.pop_back();
        m_move_equals_prev_move.pop_back();

        position_type from = move[0];
        position_type to = move[1];
        m_board.update_board(from, move_pieces[0]);
        m_board.update_board(to, move_pieces[1]);
    }
    m_move_count -= n;
}

template<class Board>
void GameState<Board>::restore_to_round(int round) {
    undo_last_rounds(m_move_count - round);
}



