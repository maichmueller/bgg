//
// Created by Michael on 26/02/2019.
//

#include "Board.h"

void Board::place_obstacles() {
    for(auto op : GameDeclarations::get_obstacle_pos(board_len)) {
        auto obs = std::make_shared<Piece>(99, 99, op, -1, true, false, true);
        board_map[op] = obs;
    }
}


Board::Board(int len)
        : board_len(len),
          board_map()
{
    if(len != 5 && len != 7 && len != 10) {
        throw std::invalid_argument("Game lenension not in [5, 7, 10].");
    }

    for(int i = 0; i < board_len; i++) {
        for(int j = 0; j < board_len; j++) {
            pos_type pos = {i, j};
            board_map[pos] = std::make_shared<Piece> (pos);
        }
    }

    place_obstacles();
}

Board::Board(int len, const std::vector<std::shared_ptr<Piece>>& setup_0, const std::vector<std::shared_ptr<Piece>>& setup_1)
        : Board(len)
{
    std::map<pos_type, int, key_comp> seen_pos_0;
    for (auto& piece : setup_0) {
        pos_type pos = piece->get_position();
        auto it = seen_pos_0.find(pos);
        if(it != seen_pos_0.end())
        {
            //element found
            throw std::invalid_argument("Parameter setup 0 has duplicate piece positions.");
        }

        seen_pos_0[pos] = 1;
        board_map[pos] = piece;
    }

    std::map<pos_type, int, key_comp> seen_pos_1;
    for (auto& piece : setup_1) {
        pos_type pos = piece->get_position();
        auto it = seen_pos_1.find(pos);
        if(it != seen_pos_1.end())
        {
            //element found
            throw std::invalid_argument("Parameter setup 1 has duplicate piece positions.");
        }

        seen_pos_1[pos] = 1;
        board_map[pos] = piece;
    }

    place_obstacles();
}

Board::Board(int len, const std::map<pos_type, int>& setup_0, const std::map<pos_type, int>& setup_1)
        : Board(len)
{
    std::map<pos_type, int, key_comp> seen_pos_0;
    std::map<int, int> version_count_0;
    for (auto& elem : setup_0) {
        pos_type pos = elem.first;
        int piece_type = elem.second;

        auto it = seen_pos_0.find(pos);
        if(it != seen_pos_0.end())
        {
            //element found
            throw std::invalid_argument("Parameter setup 0 has duplicate piece positions.");
        }

        seen_pos_0[pos] = 1;
        // null constructor of map is called on unplaced previous item (creates 0 int)
        // therefore the first time this is called, will get us to version 1, the first
        // piece. Afterwards it will keep count correctly for us.
        int version = version_count_0[piece_type];
        version_count_0[piece_type] += 1;
        auto piece = std::make_shared<Piece> (0, piece_type, pos, version);
        board_map[pos] = std::move(piece);
    }

    std::map<pos_type, int, key_comp> seen_pos_1;
    std::map<int, int> version_count_1;
    for (auto& elem : setup_1) {
        pos_type pos = elem.first;
        int piece_type = elem.second;

        auto it = seen_pos_1.find(pos);
        if(it != seen_pos_1.end())
        {
            //element found
            throw std::invalid_argument("Parameter setup 1 has duplicate piece positions.");
        }

        seen_pos_1[pos] = 1;
        // null constructor of map is called on unplaced previous item (creates 0 int)
        // therefore the first time this is called, will get us to version 0, the first
        // piece. Afterwards it will keep count correctly for us.
        int version = version_count_1[piece_type];
        version_count_1[piece_type] += 1;
        auto piece = std::make_shared<Piece> (1, piece_type, pos, version);
        board_map[pos] = std::move(piece);
    }

    place_obstacles();
}

std::shared_ptr<Piece>& Board::operator[] (pos_type a)
{
    if(a[0] >= board_len) {
        throw std::invalid_argument( "Row index out of bounds." );
    }
    if(a[1] >= board_len) {
        throw std::invalid_argument( "Column index out of bounds." );
    }
    return board_map[a];
}


std::shared_ptr<Piece> Board::operator[] (const pos_type a) const
{
    if(a[0] >= board_len) {
        throw std::invalid_argument( "Row index out of bounds." );
    }
    if(a[1] >= board_len) {
        throw std::invalid_argument( "Column index out of bounds." );
    }
    return board_map.find(a)->second;
}

typename Board::iterator Board::begin()
{
    return board_map.begin();
}


typename Board::iterator Board::end()
{
    return board_map.end();
}

typename Board::const_iterator Board::begin() const
{
    return board_map.begin();
}

typename Board::const_iterator Board::end() const
{
    return board_map.end();
}

bool Board::check_pos_integrity(pos_type pos)
{
    if(pos[0] < 0 || board_len <= pos[0]) {
        return false;
    }
    else if(pos[1] < 0 || board_len <= pos[1]) {
        return false;
    }
    else {
        return true;
    }
}

void Board::update_board(pos_type& pos, std::shared_ptr<Piece> pc_ptr)
{
    bool valid_pos = check_pos_integrity(pos);
    pc_ptr->set_position(pos);
    if(valid_pos)
        board_map[pos] = pc_ptr;
    else
        throw std::invalid_argument("Supplied position out of bounds.");
}

void Board::print_board() {
    utils::print_board<Board, Piece>(*this);
}