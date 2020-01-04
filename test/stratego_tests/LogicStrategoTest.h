#pragma once

#include <azpp/azpp.h>
#include "board/PieceStratego.h"
#include "board/BoardStratego.h"
#include "game/StateStratego.h"
#include "logic/LogicStratego.h"
#include "gtest/gtest.h"
#include "../../stratego/board/PieceStratego.h"


namespace {
    using piece_type = PieceStratego;
    using position_type = typename piece_type::position_type;
    using kin_type = typename piece_type::kin_type;

    using state_type = StateStratego;
    using move_type = typename state_type::move_type;
    using logic_type = LogicStratego<BoardStratego>;
}


class LogicStrategoTest : public ::testing::Test {

protected:
    std::map<position_type, kin_type> setup_start_0;
    std::map<position_type, kin_type> setup_start_1;
    std::map<position_type, kin_type> setup_mid_0;
    std::map<position_type, kin_type> setup_mid_1;

    void SetUp() override {

        setup_start_0[{0, 0}] = {0, 0};
        setup_start_0[{0, 1}] = {1, 0};
        setup_start_0[{0, 2}] = {2, 0};
        setup_start_0[{0, 3}] = {2, 1};
        setup_start_0[{0, 4}] = {3, 0};
        setup_start_0[{1, 0}] = {11, 0};
        setup_start_0[{1, 1}] = {10, 0};
        setup_start_0[{1, 2}] = {2, 2};
        setup_start_0[{1, 3}] = {11, 1};
        setup_start_0[{1, 4}] = {3, 1};
        setup_start_1[{3, 0}] = {2, 0};
        setup_start_1[{3, 1}] = {2, 1};
        setup_start_1[{3, 2}] = {11, 0};
        setup_start_1[{3, 3}] = {2, 2};
        setup_start_1[{3, 4}] = {0, 0};
        setup_start_1[{4, 0}] = {3, 0};
        setup_start_1[{4, 1}] = {1, 0};
        setup_start_1[{4, 2}] = {11, 1};
        setup_start_1[{4, 3}] = {3, 1};
        setup_start_1[{4, 4}] = {10, 0};

        setup_mid_0[{0, 0}] = {0, 0};
        setup_mid_0[{0, 2}] = {2, 0};
        setup_mid_0[{0, 4}] = {3, 0};
        setup_mid_0[{1, 0}] = {11, 0};
        setup_mid_0[{1, 1}] = {10, 0};
        setup_mid_0[{2, 4}] = {3, 1};
        setup_mid_1[{3, 0}] = {2, 0};
        setup_mid_1[{3, 1}] = {2, 1};
        setup_mid_1[{2, 3}] = {2, 2};
        setup_mid_1[{3, 4}] = {0, 0};
        setup_mid_1[{4, 0}] = {3, 0};
        setup_mid_1[{4, 1}] = {1, 0};
        setup_mid_1[{4, 2}] = {11, 1};

    }
};