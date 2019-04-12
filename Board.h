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


using namespace std;
using namespace GameDeclarations;

class Board
{
public:

private:
    // comparator for 2-tuple keys
    typedef typename map<pos_type, shared_ptr<Piece>, key_comp>::iterator iterator;
    typedef typename map<pos_type, shared_ptr<Piece>, key_comp>::const_iterator const_iterator;
    int board_len;
    vector<pos_type> keys;
    map<pos_type, shared_ptr<Piece>, key_comp> board_map;

    bool check_pos_integrity(pos_type pos);
    void place_obstacles();

public:
    explicit Board(int board_len);
    Board(int len, const vector<shared_ptr<Piece>>& setup_0, const vector<shared_ptr<Piece>>& setup_1);
    Board(int len, const std::map<pos_type, int>& setup_0, const std::map<pos_type, int>& setup_1);
    shared_ptr<Piece>& operator[] (pos_type a);
    shared_ptr<Piece> operator[] (const pos_type a) const;
    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
    int get_board_len() const {return board_len;}
    map<pos_type, shared_ptr<Piece>, key_comp>& get_board_map() {return board_map;}
    vector<pos_type>& get_keys() {return keys;}
    void update_board(pos_type& pos, shared_ptr<Piece> pc);
    void print_board();

};

#endif //STRATEGO_CPP_BOARD_H
