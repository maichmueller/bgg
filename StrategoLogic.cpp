//
// Created by Michael on 28/02/2019.
//

#include "StrategoLogic.h"


// StrategoLogic


std::map<std::array<int,2>, int> StrategoLogic::initialize_battle_matrix() {
    std::map<std::array<int,2>, int> bm;
    for(int i = 1; i < 11; ++i) {
        bm[{i, i}] = 0;
        for(int j = i+1; j < 11; ++j) {
            bm[{i, j}] = -1;
            bm[{j, i}] = 1;
        }
        bm[{i, 0}] = 1;
        if(i == 3)
            bm[{i, 11}] = 1;
        else
            bm[{i, 11}] = -1;
    }
    bm[{1, 10}] = 1;
    return bm;
}

const std::map<std::array<int, 2>, int> StrategoLogic::battle_matrix = StrategoLogic::initialize_battle_matrix();

pos_type StrategoLogic::pos_ident(int &len, const pos_type& pos) {
    return pos;
}

pos_type StrategoLogic::pos_invert(int &len, const pos_type &pos) {
    pos_type p = {len - pos[0], len - pos[1]};
    return p;
}

move_type StrategoLogic::move_ident(int &len, const move_type &move) {
    return move;
}

move_type StrategoLogic::move_invert(int &len, const move_type &move) {
    move_type m = {StrategoLogic::pos_invert(len, move[0]), StrategoLogic::pos_invert(len, move[1])};
    return m;
}

int StrategoLogic::team_ident(int team) {
    return team;
}

int StrategoLogic::team_invert(int team) {
    return 1 - team;
}



bool StrategoLogic::is_legal_move(const Board &board, const move_type &move_in, bool flip_teams) {

    std::function<pos_type(int&, pos_type&)> canonize_pos = &StrategoLogic::pos_ident;
    std::function<int(int)> canonize_team = &StrategoLogic::team_ident;

    if(flip_teams) {
        canonize_pos = &StrategoLogic::pos_invert;
        canonize_team = &StrategoLogic::team_invert;
    }

    int board_len = board.get_board_len() - 1;

    move_type move(move_in);
    pos_type& pos_before = move[0];
    pos_type& pos_after = move[1];

    if(pos_before[0] < 0 || pos_before[0] > board_len)
        return false;
    if(pos_before[1] < 0 || pos_before[1] > board_len)
        return false;
    if(pos_after[0] < 0 || pos_after[0] > board_len)
        return false;
    if(pos_after[1] < 0 || pos_after[1] > board_len)
        return false;

    std::shared_ptr<Piece> p_b = board[pos_before];
    std::shared_ptr<Piece> p_a = board[pos_after];

    pos_before = canonize_pos(board_len, move[0]);
    pos_after = canonize_pos(board_len, move[1]);

    if(p_b->is_null())
        return false;
    if(!p_a->is_null()) {
        if(p_a->get_team() == p_b->get_team())
            return false; // cant fight pieces of own m_team
        if(p_a->get_type() == 99)
            return false; // cant fight obstacle
    }

    int move_dist = abs(pos_after[1] - pos_before[1]) + abs(pos_after[0] - pos_before[0]);
    if(move_dist > 1) {
        if(p_b->get_type() != 2)
            return false;  // not of type 2 , but is supposed to go far

        if(pos_after[0] == pos_before[0]) {
            int dist = pos_after[1] - pos_before[1];
            int sign = (dist >= 0) ? 1 : -1;
            for(int i = pos_before[1] + sign; i < pos_after[1]; i = i + sign) {
                pos_type pos = {pos_before[0], i};
                if(!board[canonize_pos(board_len, pos)]->is_null())
                    return false;
            }
        }

        else if(pos_after[1] == pos_before[1]) {
            int dist = pos_after[0] - pos_before[0];
            int sign = (dist >= 0) ? 1 : -1;
            for(int i = pos_before[0] + sign; i < pos_after[0]; i = i + sign) {
                pos_type pos = {i, pos_before[1]};
                if(!board[canonize_pos(board_len, pos)]->is_null())
                    return false;
            }
        }

        else
            return false;  // diagonal moves not allowed
    }
    return true;
}


std::vector<move_type> StrategoLogic::get_poss_moves(const Board &board, int player, bool flip_teams) {

    std::function<move_type(int&, move_type&)> canonize_move = &StrategoLogic::move_ident;
    std::function<int(int)> canonize_team = &StrategoLogic::team_ident;

    if(flip_teams) {
        canonize_move = &StrategoLogic::move_invert;
        canonize_team = &StrategoLogic::team_invert;
    }

    int board_len = board.get_board_len();
    std::vector<move_type> moves_possible;
    for( auto elem = board.begin(); elem != board.end(); ++elem) {
        std::shared_ptr<Piece> piece = elem->second;
        if(!piece->is_null() && canonize_team(piece->get_team()) == player && piece->get_flag_can_move()) {
            // the position we are dealing with
            pos_type pos = piece->get_position();

            if(piece->get_type() == 2) {
                // all possible moves to the right until board ends
                for(int i = 1; i < board_len - pos[0]; ++i) {
                    pos_type pos_to = {pos[0] + i, pos[1]};
                    move_type move = {pos, pos_to};
                    if(is_legal_move(board, move, flip_teams))
                        moves_possible.push_back(canonize_move(board_len, move));
                }
                // all possible moves to the top until board ends
                for(int i = 1; i < board_len - pos[1]; ++i) {
                    pos_type pos_to = {pos[0], pos[1] + i};
                    move_type move = {pos, pos_to};
                    if(is_legal_move(board, move))
                        moves_possible.push_back(canonize_move(board_len, move));
                }
                // all possible moves to the left until board ends
                for(int i = 1; i < pos[0] + 1; ++i) {
                    pos_type pos_to = {pos[0] - i, pos[1]};
                    move_type move = {pos, pos_to};
                    if(is_legal_move(board, move))
                        moves_possible.push_back(canonize_move(board_len, move));
                }
                // all possible moves to the bottom until board ends
                for(int i = 1; i < pos[1] + 1; ++i) {
                    pos_type pos_to = {pos[0], pos[1] - i};
                    move_type move = {pos, pos_to};
                    if(is_legal_move(board, move))
                        moves_possible.push_back(canonize_move(board_len, move));
                }
            }
            else {
                // all moves are 1 step to left, right, top, or bottom
                std::vector<pos_type> pos_tos = {{pos[0]+1, pos[1]},
                                                 {pos[0]  , pos[1]+1},
                                                 {pos[0]-1, pos[1]},
                                                 {pos[0]  , pos[1]-1}};
                for(auto& pos_to : pos_tos) {
                    move_type move = {pos, pos_to};
                    if(is_legal_move(board, move))
                        moves_possible.push_back(canonize_move(board_len, move));
                }
            }
        }
    }
    return moves_possible;
}

bool StrategoLogic::has_poss_moves(const Board &board, int player) {

    int board_len = board.get_board_len();
    for( auto elem = board.begin(); elem != board.end(); ++elem) {
        std::shared_ptr<Piece> piece = elem->second;
        if(!piece->is_null() && piece->get_team() == player && piece->get_flag_can_move()) {
            // the position we are dealing with
            pos_type pos = piece->get_position();

            if(piece->get_type() == 2) {
                // all possible moves to the right until board ends
                for(int i = 1; i < board_len - pos[0]; ++i) {
                    pos_type pos_to = {pos[0] + i, pos[1]};
                    move_type move = {pos, pos_to};
                    if(is_legal_move(board, move))
                        return true;
                }
                // all possible moves to the top until board ends
                for(int i = 1; i < board_len - pos[1]; ++i) {
                    pos_type pos_to = {pos[0], pos[1] + i};
                    move_type move = {pos, pos_to};
                    if(is_legal_move(board, move))
                        return true;
                }
                // all possible moves to the left until board ends
                for(int i = 1; i < pos[0] + 1; ++i) {
                    pos_type pos_to = {pos[0] - i, pos[1]};
                    move_type move = {pos, pos_to};
                    if(is_legal_move(board, move))
                        return true;
                }
                // all possible moves to the bottom until board ends
                for(int i = 1; i < pos[1] + 1; ++i) {
                    pos_type pos_to = {pos[0], pos[1] - i};
                    move_type move = {pos, pos_to};
                    if(is_legal_move(board, move))
                        return true;
                }
            }
            else {
                // all moves are 1 step to left, right, top, or bottom
                std::vector<pos_type> pos_tos = {{pos[0]+1, pos[1]},
                                                 {pos[0]  , pos[1]+1},
                                                 {pos[0]-1, pos[1]},
                                                 {pos[0]  , pos[1]-1}};
                for(auto& pos_to : pos_tos) {
                    move_type move = {pos, pos_to};
                    if(is_legal_move(board, move))
                        return true;
                }
            }
        }
    }
    return false;
}

int StrategoLogic::find_action_idx(std::vector<std::vector<int>> &vec_to_search, std::vector<int> &action_to_find) {

    // func to find a specific vector in another (assuming types are all known)

    int idx = 0;
    std::vector<std::vector<int>>::iterator entry;
    for(entry = vec_to_search.begin(); entry != vec_to_search.end(); ++entry) {
        if((*entry)[0] == action_to_find[0] && (*entry)[1] == action_to_find[1]) {
            // element has been found, exit the for-loop
            break;
        }
        // increase idx, as this idx wasnt the right one
        ++idx;
    }
    if(entry == vec_to_search.end())
        return -1;  // vector has not been found, -1 as error index
    else
        return idx;
}

void StrategoLogic::enable_legal_action(std::vector<int>& action_mask, const Board& board,
                                        const std::vector<std::vector<int>>& action_arr,
                                        const std::vector<int>& act_range,
                                        const pos_type& pos, const pos_type& pos_to) {
    //  func to check a new move for correctness, and if so, enable it in the mask

    // the move to check for correctness
    move_type move = {pos, pos_to};

    if(is_legal_move(board, move)) {
        // if the move is legal we want to set the action_mask at the
        // right index to 1
        pos_type action_base = {pos_to[0] - pos[0], pos_to[1] - pos[1]};
        std::vector<std::vector<int>> slice(act_range.size());
        for(int idx = 0; idx < slice.size(); ++idx) {
            slice[idx] = action_arr[act_range[idx]];
        }
        int idx = StrategoLogic::find_action_idx(slice, action_base);
        action_mask[idx] = 1;
    }
};


std::vector<int> StrategoLogic::get_action_mask(const Board &board, const std::vector<std::vector<int>>& action_arr,
                                           const std::map<std::vector<int>, std::vector<int>>& piece_act_map, int player) {

    std::vector<int> action_mask(action_arr.size(), 0);
    int board_len = board.get_board_len();

    for( auto elem = board.begin(); elem != board.end(); ++elem) {
        std::shared_ptr<Piece> piece = elem->second;
        if(!piece->is_null() && piece->get_team() == player && piece->get_flag_can_move()) {

            std::vector<int> type_ver = {piece->get_type(), piece->get_version()};
            const std::vector<int>& act_range = piece_act_map.at(type_ver);

            // the position we are dealing with
            pos_type pos = piece->get_position();

            if(piece->get_type() == 2) {
                // all possible moves to the right until board ends
                for(int i = 1; i < board_len - pos[0]; ++i) {
                    // if pos has been inverted already, then we need to make sure, that
                    // pos_to is reinverted back to normal. Otherwise the call to is_legal_move
                    // in enable_legal_action,
                    pos_type pos_to = {pos[0] + i, pos[1]};
                    StrategoLogic::enable_legal_action(action_mask, board,
                                                       action_arr, act_range,
                                                       pos, pos_to);
                    }
                // all possible moves to the top until board ends
                for(int i = 1; i < board_len - pos[1]; ++i) {
                    pos_type pos_to = {pos[0], pos[1] + i};
                    StrategoLogic::enable_legal_action(action_mask, board,
                                                       action_arr, act_range,
                                                       pos, pos_to);
                }
                // all possible moves to the left until board ends
                for(int i = 1; i < pos[0] + 1; ++i) {
                    pos_type pos_to = {pos[0] - i, pos[1]};
                    StrategoLogic::enable_legal_action(action_mask, board,
                                                       action_arr, act_range,
                                                       pos, pos_to);
                }
                // all possible moves to the bottom until board ends
                for(int i = 1; i < pos[1] + 1; ++i) {
                    pos_type pos_to = {pos[0], pos[1] - i};
                    StrategoLogic::enable_legal_action(action_mask, board,
                                                       action_arr, act_range,
                                                       pos, pos_to);
                }
            }
            else {
                // all moves are 1 step to left, right, top, or bottom
                std::vector<pos_type> pos_tos = {{pos[0]+1, pos[1]},
                                                 {pos[0]  , pos[1]+1},
                                                 {pos[0]-1, pos[1]},
                                                 {pos[0]  , pos[1]-1}};
                for(auto& pos_to : pos_tos) {
                    StrategoLogic::enable_legal_action(action_mask, board,
                                                       action_arr, act_range,
                                                       pos, pos_to);
                }
            }
        }
    }
    return action_mask;
}