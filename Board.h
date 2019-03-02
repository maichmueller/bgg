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


using namespace std;
using namespace GameTypes;

class Board
{
public:

private:
    // comparator for 2-tuple keys
    typedef typename map<pos_type, shared_ptr<Piece>, key_comp>::iterator iterator;
    int board_dim;
    vector<pos_type> keys;
    map<pos_type, shared_ptr<Piece>, key_comp> board_map;

    bool check_pos_integrity(pos_type pos);

public:
    explicit Board(int board_dim);
    Board(int dim, vector<shared_ptr<Piece>> setup_0, vector<shared_ptr<Piece>> setup_1);
    Board(int dim, std::map<pos_type, int> setup_0, std::map<pos_type, int> setup_1);
    shared_ptr<Piece>& operator[] (pos_type a);
    shared_ptr<Piece> operator[] (const pos_type a) const;
    iterator begin();
    iterator end();
    int get_board_dim_size() {return board_dim;}
    map<pos_type, shared_ptr<Piece>, key_comp>& get_board_map() {return board_map;}
    vector<pos_type>& get_keys() {return keys;}
    void update_board(pos_type& pos, shared_ptr<Piece> pc);

};

#endif //STRATEGO_CPP_BOARD_H
