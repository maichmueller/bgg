#include "BoardTest.h"
#include "gtest/gtest.h"

using namespace BoardTest;

TEST(BoardTest, BoardTest_BinaryOperator_Tests_Ints) {
    std::map<position_type, kin_type> setup0;
    std::map<position_type, kin_type> setup1;

    kin_type k{11,11};
    std::cout << "Hash of " << k.to_string() << std::hash<kin_type >()(k) << "\n";
    kin_type k1{22,22};
    std::cout << "Hash of " << k.to_string() << std::hash<kin_type >()(k1) << "\n";
    kin_type k2{33,33};
    std::cout << "Hash of " << k.to_string() << std::hash<kin_type >()(k2) << "\n";
    kin_type k3{44,44};
    std::cout << "Hash of " << k.to_string() << std::hash<kin_type >()(k3) << "\n";
    kin_type k4{55,55};
    std::cout << "Hash of " << k.to_string() << std::hash<kin_type >()(k4) << "\n";

    setup0[{0, 0}] = {11, 11};
    for(auto elem : setup0){
        std::cout<< elem.first.to_string() << elem.second.to_string() << std::endl;
    }
    setup0[{0, 1}] = {22, 22};
    for(auto elem : setup0){
        std::cout<< elem.first.to_string() << elem.second.to_string() << std::endl;
    }
//    setup0[{0, 2}] = {33, 33};
//    for(auto elem : setup0){
//        std::cout<< elem.first.to_string() << elem.second.to_string() << std::endl;
//    }
//    setup0[{1, 0}] = {44, 44};
//    for(auto elem : setup0){
//        std::cout<< elem.first.to_string() << elem.second.to_string() << std::endl;
//    }
//    setup0[{1, 1}] = {55, 55};
//    for(auto elem : setup0){
//        std::cout<< elem.first.to_string() << elem.second.to_string() << std::endl;
//    }
//    setup0[{1, 2}] = {66, 66};
//    for(auto elem : setup0){
//        std::cout<< elem.first.to_string() << elem.second.to_string() << std::endl;
//    }

    kin_type test_kin{20, 1};
    position_type test_pos{3, 2};
    std::shared_ptr<piece_type > test_piece = std::make_shared<piece_type >(test_pos, test_kin, 1);

    setup1[{3, 0}] = {0, 0};
    setup1[{3, 1}] = {5, 0};
    setup1[test_pos] = test_kin;
    setup1[{4, 0}] = {3, 0};
    setup1[{4, 1}] = {3, 2};
    setup1[{4, 2}] = {4, 0};

//    for(auto elem : setup0){
//        std::cout<< elem.first.to_string() << elem.second.to_string() << std::endl;
//    }
//    for(auto elem : setup1){
//        std::cout<< elem.first.to_string() << elem.second.to_string() << std::endl;
//    }

    std::array<size_t, 2> shape{5, 3};
    std::array<int, 2> starts{0, 0};
    planar_board b{shape, starts, setup0, setup1};

    // check access operator
    EXPECT_EQ(*b[test_pos], *test_piece);

    // check get all pieces of one player method
    std::vector<std::shared_ptr<piece_type> > pieces_0 = b.get_pieces(0);
    for(const auto & piece : pieces_0) {

    }

    // check update board method
    kin_type new_test_kin{5, 1};
    std::shared_ptr<piece_type > new_test_piece = std::make_shared<piece_type >(test_pos, new_test_kin, 1);
    b.update_board(test_pos, new_test_piece);

    EXPECT_EQ(*b[test_pos], *new_test_piece);


//    EXPECT_TRUE(m1 == m3);
//    EXPECT_TRUE(m1 == m1);
//    EXPECT_TRUE(m2 != m3);
//    EXPECT_FALSE(m4 == m3);
}