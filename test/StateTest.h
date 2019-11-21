#pragma once

#include <azpp/azpp.h>


namespace {
    using position_type = Position<int, 2>;
    using piece_type = Piece<position_type, 2>;
    using kin_type = typename piece_type::kin_type;
    class BoardImplTest : public Board<piece_type > {
    public:
        using base = Board<piece_type >;
        using base::base;
        std::string print_board(bool flip_board, bool hide_unknowns) const override {return "";}
    };
    using planar_board = BoardImplTest;

    class StateImplTest : public State<planar_board> {
    public:
        using base = State<planar_board>;
        using base::base;

        void check_terminal() override {
            bool has_pieces = false;
            for(auto & piece: m_board) {
                has_pieces = true;
                break;
            }
            m_terminal = has_pieces;
        }

        int do_move(const move_type &move) override {
            m_board.update_board(move[1], m_board[move[0]]);
            m_board.update_board(move[0], std::make_shared<piece_type>(move[0]));
        }
    };
    using planar_board = BoardImplTest;

    using state_type = StateImplTest;
};
