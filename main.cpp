//#include <iostream>
//#include "Board.h"
//#include "Piece.h"
//#include "GameState.h"
//#include "Game.h"
//#include "vector"
//#include "random"
//#include "torch/torch.h"
//
//int main() {
//    int board_len = 5;
//
//    std::map<pos_type, int> setup_0;
//    std::map<pos_type, int> setup_1;
//
//    setup_0[{0,0}] = 0;
//    setup_0[{0,1}] = 11;
//    setup_0[{0,2}] = 3;
//    setup_0[{0,3}] = 10;
//    setup_0[{0,4}] = 1;
//    setup_0[{1,0}] = 2;
//    setup_0[{1,1}] = 2;
//    setup_0[{1,2}] = 11;
//    setup_0[{1,3}] = 3;
//    setup_0[{1,4}] = 2;
//
//    setup_1[{4,0}] = 0;
//    setup_1[{4,1}] = 11;
//    setup_1[{4,2}] = 3;
//    setup_1[{4,3}] = 10;
//    setup_1[{4,4}] = 1;
//    setup_1[{3,0}] = 2;
//    setup_1[{3,1}] = 2;
//    setup_1[{3,2}] = 11;
//    setup_1[{3,3}] = 3;
//    setup_1[{3,4}] = 2;
//    // Board board_from_setups(board_len, setup_0, setup_1);
//
//    std::cout << "Initialized boards." << std::endl;
//
//    auto ag0 = std::make_shared<RandomAgent<>> (0);
//    auto ag1 = std::make_shared<RandomAgent<>> (1);
//    Game game(board_len, ag0, ag1);
//    utils::print_board<Board, Piece>(*game.get_gamestate()->get_board());
//    utils::print_board<Board, Piece>(*game.get_gamestate()->get_board(), true);
////    for(int i = 0; i < 50; ++i) {
////        std::cout << "Game: " << i << std::endl;
////        utils::print_board<Board, Piece>(*game.get_gamestate()->get_board());
////        game.run_game(true);
////        game.reset();
////    }
//
//    return 0;
//
//
//
//}



#include <iostream>
#include <vector>
#include <set>
#include <type_traits>

template<class T1,class T2>
struct compare_template_class : std::false_type {};

template<template<class...> class T,class... Args1, class... Args2>
struct compare_template_class<T<Args1...>,T<Args2...>> : std::true_type {};

template<class T>
struct number_of_nested_classes
{
    static const int value = 0;

    static void sizes(const T&, std::vector<size_t>&) {}
};

// remember that std::vector has two arguments! so you have to deduce the two of them
template<template<class,class...> class TT,class T,class... Args>
struct number_of_nested_classes<TT<T,Args...> >
{
    static const int value_before = number_of_nested_classes<T>::value;
    static const int value = compare_template_class<TT<T,Args...>,T>::value ? 1 + value_before : value_before;

    static void sizes(const TT<T,Args...>& v, std::vector<size_t>& size)
    {
        size.push_back(v.size());
        number_of_nested_classes<T>::sizes(v[0],size);
    }
};


int main(int argc, char const *argv[])
{

    // std::cout << compare_template_class<std::vector<int>,int >::value << std::endl;
    // std::cout << compare_template_class<std::vector<int>,std::vector<std::vector<int> > >::value << std::endl;

    // std::cout << number_of_nested_classes<int >::value << std::endl;
    // std::cout << number_of_nested_classes<std::vector<int > >::value << std::endl;
    // std::cout << number_of_nested_classes<std::vector<std::vector<int > > >::value << std::endl;
    // std::cout << number_of_nested_classes<std::vector<std::vector<std::vector<int> > > >::value << std::endl;
    // std::cout << number_of_nested_classes<std::vector<std::set<std::vector<std::vector<int>> > > >::value << std::endl;

    using V1 = std::vector<int>;
    using V2 = std::vector<V1>;
    using V3 = std::vector<V2>;
    using V4 = std::vector<V3>;
    using V5 = std::vector<V4>;

    std::vector<size_t> size;
    V1 v1(10);
    V2 v2({v1,v1,v1,v1,v1});
    V3 v3({v2,v2,v2});
    V4 v4({v3,v3,v3,v3});
    V5 v5({v4});
    // std::cout << v5.size() << std::endl;
    number_of_nested_classes<V5>::sizes(v5,size);
    for (auto s : size)
    {
        std::cout << s << std::endl;
    }


    return 0;
}