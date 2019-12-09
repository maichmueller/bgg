//
// Created by Michael on 21/02/2019.
//

#pragma once

#include <unordered_map>

#include "azpp/board/Board.h"
#include "azpp/board/Piece.h"
#include "azpp/board/Move.h"
#include "azpp/board/Position.h"


template<class BoardType>
class State {

public:
    using board_type = BoardType;
    using piece_type = typename BoardType::piece_type;
    using kin_type = typename BoardType::kin_type;
    using position_type = typename BoardType::position_type;
    using move_type = Move<position_type>;

protected:
    board_type m_board;

    using dead_pieces_type = std::array<std::vector<kin_type>, 2>;
    dead_pieces_type m_dead_pieces;

    int m_terminal;
    bool m_terminal_checked;

    int m_move_count;

    std::vector<move_type> m_move_history;
    std::vector<std::array<std::shared_ptr<piece_type>, 2>> m_piece_history;
    std::vector<bool> m_move_equals_prev_move;
    unsigned int m_rounds_without_fight;

    virtual int _do_move(const move_type &move);

public:
    template<size_t dim>
    explicit State(const std::array<size_t, dim> &shape,
                   const std::array<int, dim> &board_starts);

    explicit State(board_type &&board,
                   int move_count = 0);

    explicit State(const board_type &board,
                   int move_count = 0);

    template<size_t dim>
    State(const std::array<size_t, dim> &shape,
          const std::array<int, dim> &board_starts,
          const std::map<position_type, typename piece_type::kin_type> &setup_0,
          const std::map<position_type, typename piece_type::kin_type> &setup_1);

    auto &operator[](const position_type &position) { return m_board[position]; }

    const auto &operator[](const position_type &position) const { return m_board[position]; }

    int is_terminal(bool force_check = false);

    virtual void check_terminal() = 0;

    int do_move(const move_type &move);

    virtual void restore_to_round(int round);

    void undo_last_rounds(int n = 1);

    int get_move_count() { return m_move_count; }

    void set_board(board_type brd) { this->m_board = std::move(brd); }

    board_type const *get_board() const { return &m_board; }
};

template<class BoardType>
int State<BoardType>::_do_move(const State::move_type &move) {
    m_board.update_board(move[1], m_board[move[0]]);
    m_board.update_board(move[0], std::make_shared<piece_type>(move[0]));
    return 0;
}

template<class BoardType>
State<BoardType>::State(board_type &&board,
                        int move_count)
        : m_board(std::move(board)),
          m_dead_pieces(),
          m_terminal(404),
          m_terminal_checked(false),
          m_move_count(move_count),
          m_move_history(),
          m_move_equals_prev_move(0),
          m_rounds_without_fight(0) {}

template<class BoardType>
State<BoardType>::State(const board_type &board,
                        int move_count)
        : State(board_type(board), move_count) {}

template<class BoardType>
template<size_t dim>
State<BoardType>::State(const std::array<size_t, dim> &shape,
                        const std::array<int, dim> &board_starts)
        : State(board_type(shape, board_starts)) {}

template<class BoardType>
template<size_t dim>
State<BoardType>::State(const std::array<size_t, dim> &shape,
                        const std::array<int, dim> &board_starts,
                        const std::map<position_type, typename piece_type::kin_type> &setup_0,
                        const std::map<position_type, typename piece_type::kin_type> &setup_1)
        : State(board_type(shape, board_starts, setup_0, setup_1)) {}

template<class BoardType>
int State<BoardType>::is_terminal(bool force_check) {
    if (!m_terminal_checked || force_check)
        check_terminal();
    return m_terminal;
}

template<class BoardType>
void State<BoardType>::undo_last_rounds(int n) {
    for (int i = 0; i < n; ++i) {
        move_type move = m_move_history.back();
        auto move_pieces = m_piece_history.back();

        m_move_history.pop_back();
        m_piece_history.pop_back();
        m_move_equals_prev_move.pop_back();

        m_board.update_board(move[0], move_pieces[0]);
        m_board.update_board(move[1], move_pieces[1]);
    }
    m_move_count -= n;
}

template<class BoardType>
void State<BoardType>::restore_to_round(int round) {
    undo_last_rounds(m_move_count - round);
}

template<class BoardType>
int State<BoardType>::do_move(const State::move_type &move) {
    // save all info to the history
    std::shared_ptr<piece_type> piece_from = m_board[move[0]];
    std::shared_ptr<piece_type> piece_to = m_board[move[1]];
    if (m_move_equals_prev_move.empty())
        m_move_equals_prev_move.push_back(false);
    else {
        auto &last_move = m_move_history.back();
        m_move_equals_prev_move.push_back((move[0] == last_move[0]) && (move[1] == last_move[1]));
    }
    m_move_history.push_back(move);
    // copying the pieces here, bc this way they can be fully restored later on
    // (especially when flags have been altered - needed in undoing last rounds)
    m_piece_history.push_back({std::make_shared<piece_type>(*piece_from), std::make_shared<piece_type>(*piece_to)});

    m_terminal_checked = false;
    m_move_count += 1;

    return _do_move(move);
}



