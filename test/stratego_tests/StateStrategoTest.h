#pragma once

#include <azpp/azpp.h>
#include "board/PieceStratego.h"
#include "board/BoardStratego.h"
#include "game/StateStratego.h"
#include "gtest/gtest.h"
#include "../../stratego/board/PieceStratego.h"


namespace {
    using piece_type = PieceStratego;
    using position_type = typename piece_type::position_type;
    using kin_type = typename piece_type::kin_type;

    using state_type = StateStratego;
    using move_type = typename state_type::move_type;
}


class StateTest : public ::testing::Test {

protected:
    std::map<position_type, kin_type> setup0;
    std::map<position_type, kin_type> setup1;

    void SetUp() override {

        setup0[{0, 0}] = {0, 0};
        setup0[{0, 1}] = {1, 0};
        setup0[{0, 2}] = {2, 0};
        setup0[{0, 3}] = {2, 1};
        setup0[{0, 4}] = {3, 0};
        setup0[{1, 0}] = {11, 0};
        setup0[{1, 1}] = {10, 0};
        setup0[{1, 2}] = {2, 2};
        setup0[{1, 3}] = {11, 1};
        setup0[{1, 4}] = {3, 1};
        setup1[{3, 0}] = {2, 0};
        setup1[{3, 1}] = {2, 1};
        setup1[{3, 2}] = {11, 0};
        setup1[{3, 3}] = {2, 2};
        setup1[{3, 4}] = {0, 0};
        setup1[{4, 0}] = {3, 0};
        setup1[{4, 1}] = {1, 0};
        setup1[{4, 2}] = {11, 1};
        setup1[{4, 3}] = {3, 1};
        setup1[{4, 4}] = {10, 0};

        auto state = state_type(
                std::array<size_t, 2>{5, 5},
                std::array<int, 2>{0, 0},
                setup0,
                setup1
        );
    }
};
