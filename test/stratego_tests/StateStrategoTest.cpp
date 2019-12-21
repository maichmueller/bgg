#include "StateStrategoTest.h"


TEST_F(StateTest, StateStrategoTest_do_move) {
    move_type move1{{1, 2}, {3, 2}};
    move_type move2{{1, 2}, {3, 2}};
    move_type move3{{1, 2}, {3, 2}};
    move_type move4{{1, 2}, {3, 2}};
    move_type move5{{1, 2}, {3, 2}};

}

TEST_F(StateTest, StateTest_DO_and_UNDO_Move_Ints_2D) {
    size_t board_size_x = 5;
    size_t board_size_y = 3;

    auto state = state_type(
            std::array<size_t, 2>{board_size_x, board_size_y},
            std::array<int, 2>{0, 0},
            setup0,
            setup1
    );

    EXPECT_EQ(*(state[{4, 0}]), (piece_type({4, 0}, kin_type{3, 0}, 1)));

    state.do_move({position_type(3, 0), position_type(3, 2)});
    state.undo_last_rounds(1);
    auto piece_30 = *(state[{3, 0}]);
    EXPECT_EQ(*(state[{3, 0}]), piece_30);
    auto piece_00 = *(state[{0, 0}]);
    auto piece_12 = *(state[{1, 2}]);
    // round 0
    state.do_move({position_type(3, 0), position_type(3, 2)});
    // round 1
    state.do_move({position_type(0, 0), position_type(3, 2)});
    // round 2
    state.do_move({position_type(1, 2), position_type(3, 2)});
    // round 3
    // ...
    state.restore_to_round(2);
    EXPECT_EQ(*(state[{1, 2}]), piece_12);
    EXPECT_NE(*(state[{0, 0}]), piece_00);
    EXPECT_NE(*(state[{3, 0}]), piece_30);
    state.restore_to_round(1);
    EXPECT_EQ(*(state[{0, 0}]), piece_00);
    EXPECT_NE(*(state[{3, 0}]), piece_30);
    state.restore_to_round(0);
    EXPECT_EQ(*(state[{3, 0}]), piece_30);
}