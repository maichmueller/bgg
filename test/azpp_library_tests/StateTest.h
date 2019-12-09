#pragma once

#include <azpp/azpp.h>
#include "gtest/gtest.h"


namespace {
    using position_type = Position<int, 2>;
    using piece_type = Piece<position_type, 2>;
    using kin_type = typename piece_type::kin_type;

    class BoardImplTest : public Board<piece_type > {
    public:
        using base = Board<piece_type >;
        using base::base;
        [[nodiscard]] std::string print_board(bool flip_board, bool hide_unknowns) const override {return "";}
    };
    using planar_board = BoardImplTest;

    class StateImplTest : public State<planar_board> {
    public:
        using base = State<planar_board>;
        using base::base;

        void check_terminal() override {
            bool has_pieces = false;
            if(m_board.size())
                has_pieces = true;
            m_terminal = has_pieces;
        }
    };
    using state_type = StateImplTest;
}


class StateTest : public ::testing::Test {

protected:
    std::map<position_type, kin_type> setup0;
    std::map<position_type, kin_type> setup1;

    void SetUp() override {

        setup0[{0, 0}] = {0, 0};
        setup0[{0, 1}] = {1, 0};
        setup0[{0, 2}] = {2, 0};
//    setup0[{0, 3}] = {2, 1};
//    setup0[{0, 4}] = {3, 0};
        setup0[{1, 0}] = {11, 0};
        setup0[{1, 1}] = {10, 0};
        setup0[{1, 2}] = {2, 2};
//    setup0[{1, 3}] = {11, 1};
//    setup0[{1, 4}] = {3, 1};
        setup1[{3, 0}] = {2, 0};
        setup1[{3, 1}] = {2, 1};
        setup1[{3, 2}] = {11, 0};
//    setup1[{3, 3}] = {2, 2};
//    setup1[{3, 4}] = {0, 0};
        setup1[{4, 0}] = {3, 0};
        setup1[{4, 1}] = {1, 0};
        setup1[{4, 2}] = {11, 1};
//    setup1[{4, 3}] = {3, 1};
//    setup1[{4, 4}] = {10, 0};
    }
};
