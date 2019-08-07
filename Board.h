//
// Created by Michael on 20/02/2019.
//

#ifndef STRATEGO_CPP_BOARD_H
#define STRATEGO_CPP_BOARD_H

#include "map"
#include "array"
#include "vector"
#include "memory"

#include "Piece.h"
#include "GameDeclarations.h"
#include "utils.h"


using namespace GameDeclarations;

class Board
{
public:

private:
    // comparator for 2-tuple keys
    typedef typename std::map<pos_t, std::shared_ptr<Piece>, key_comp>::iterator iterator;
    typedef typename std::map<pos_t, std::shared_ptr<Piece>, key_comp>::const_iterator const_iterator;
    int board_len;
    std::map<pos_t, std::shared_ptr<Piece>, key_comp> board_map;

    bool check_pos_integrity(pos_t pos);
    void place_obstacles();

public:
    explicit Board(int board_len);
    Board(int len, const std::vector<std::shared_ptr<Piece>>& setup_0, const std::vector<std::shared_ptr<Piece>>& setup_1);
    Board(int len, const std::map<pos_t, int>& setup_0, const std::map<pos_t, int>& setup_1);
    std::shared_ptr<Piece>& operator[] (pos_t a);
    std::shared_ptr<Piece> operator[] (const pos_t a) const;
    iterator begin();
    iterator end();
    [[nodiscard]] const_iterator begin() const;
    [[nodiscard]] const_iterator end() const;
    [[nodiscard]] int get_board_len() const {return board_len;}
    std::map<pos_t, std::shared_ptr<Piece>, key_comp>& get_board_map() {return board_map;}

    void update_board(pos_t& pos, std::shared_ptr<Piece>& pc);
    void print_board();

};

#endif //STRATEGO_CPP_BOARD_H
