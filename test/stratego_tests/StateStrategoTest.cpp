#include "StateStrategoTest.h"


TEST_F(StateStrategoTest, StateStrategoTest_do_move) {
    move_type move1{{1, 2}, {3, 2}};
    state.do_move(move1);
    ASSERT_EQ((state[{3,2}]->get_kin()), kin_type(11, 0));
    ASSERT_EQ((state[{1,2}]->get_kin()), kin_type(404, 404));

    move_type move2{{3, 1}, {1, 1}};
    state.do_move(move2);
    ASSERT_EQ((state[{1,1}]->get_kin()), kin_type(10, 0));
    ASSERT_EQ((state[{3,1}]->get_kin()), kin_type(404, 404));

    move_type move3{{0, 2}, {3, 2}};
    state.do_move(move3);
    ASSERT_EQ((state[{3,2}]->get_kin()), kin_type(11, 0));
    ASSERT_EQ((state[{0,2}]->get_kin()), kin_type(404, 404));

    move_type move4{{3, 3}, {2, 3}};
    state.do_move(move4);
    ASSERT_EQ((state[{2,3}]->get_kin()), kin_type(2, 2));
    ASSERT_EQ((state[{3,3}]->get_kin()), kin_type(404, 404));

    move_type move5{{1, 4}, {2, 4}};
    state.do_move(move5);
    ASSERT_EQ((state[{2,4}]->get_kin()), kin_type(3, 1));
    ASSERT_EQ((state[{1,4}]->get_kin()), kin_type(404, 404));
}
