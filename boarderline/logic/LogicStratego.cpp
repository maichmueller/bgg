//
// Created by Michael on 28/02/2019.
//

#include "LogicStratego.h"


// LogicStratego


std::map<Position, int> LogicStratego::initialize_battle_matrix() {
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

const std::map<std::array<int, 2>, int> LogicStratego::battle_matrix = LogicStratego::initialize_battle_matrix();

void LogicStratego::_invert_pos(int &len, Position &pos) {
    pos[0] = len - 1 - pos[0];
    pos[1] = len - 1 - pos[1] ;
}

void LogicStratego::_invert_move(strat_move_t &move, int &len) {
    LogicStratego::_invert_pos(len, move[0]);
    LogicStratego::_invert_pos(len, move[1]);
}
int LogicStratego::_invert_team(int team) {
    return 1 - team;
}



bool LogicStratego::is_legal_move(const Board &board, const strat_move_t &move) {
    int board_len = board.get_shape() - 1;

    const auto & [pos_before, pos_after] = move;

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

    if(p_b->is_null())
        return false;
    if(!p_a->is_null()) {
        if(p_a->get_team() == p_b->get_team())
            return false; // cant fight pieces of own m_team
        if(p_a->get_kin() == 99)
            return false; // cant fight obstacle
    }

    int move_dist = abs(pos_after[1] - pos_before[1]) + abs(pos_after[0] - pos_before[0]);
    if(move_dist > 1) {
        if(p_b->get_kin() != 2)
            return false;  // not of type 2 , but is supposed to go far

        if(pos_after[0] == pos_before[0]) {
            int dist = pos_after[1] - pos_before[1];
            int sign = (dist >= 0) ? 1 : -1;
            for(int i = 1; i < std::abs(dist); ++i) {
                Position pos = {pos_before[0], pos_before[1] + sign * i};
                if(!board[pos]->is_null())
                    return false;
            }
        }

        else if(pos_after[1] == pos_before[1]) {
            int dist = pos_after[0] - pos_before[0];
            int sign = (dist >= 0) ? 1 : -1;
            for(int i = 1; i < std::abs(dist); ++i) {
                Position pos = {pos_before[0] + sign * i, pos_before[1]};
                if(!board[pos]->is_null())
                    return false;
            }
        }

        else
            return false;  // diagonal moves not allowed
    }
    return true;
}


//std::vector<strat_move_t> LogicStratego::get_poss_moves(const Board &board, int player, bool flip_teams) {
//
//    std::function<void(int&, strat_move_t&)> canonize_move = &LogicStratego::move_ident;
//    std::function<int(int)> canonize_team = &LogicStratego::team_ident;
//
//    if(flip_teams) {
//        canonize_move = &LogicStratego::move_invert;
//        canonize_team = &LogicStratego::team_invert;
//    }
//
//    int m_shape = board.get_shape();
//    std::vector<strat_move_t> moves_possible;
//    for( auto elem = board.begin(); elem != board.end(); ++elem) {
//        std::shared_ptr<Piece> piece = elem->second;
//        if(!piece->is_null() && canonize_team(piece->get_team()) == player && piece->get_flag_can_move()) {
//            // the position we are dealing with
//            Position pos = piece->get_position();
//
//            if(piece->get_kin() == 2) {
//                // all possible moves to the right until board ends
//                for(int i = 1; i < m_shape - pos[0]; ++i) {
//                    Position pos_to = {pos[0] + i, pos[1]};
//                    strat_move_t move = {pos, pos_to};
//                    if(is_legal_move(board, move, flip_teams)) {
//                        canonize_move(m_shape, move);
//                        moves_possible.push_back(move);
//                    }
//                }
//                // all possible moves to the top until board ends
//                for(int i = 1; i < m_shape - pos[1]; ++i) {
//                    Position pos_to = {pos[0], pos[1] + i};
//                    strat_move_t move = {pos, pos_to};
//                    if(is_legal_move(board, move, flip_teams)) {
//                        canonize_move(m_shape, move);
//                        moves_possible.push_back(move);
//                    }
//                }
//                // all possible moves to the left until board ends
//                for(int i = 1; i < pos[0] + 1; ++i) {
//                    Position pos_to = {pos[0] - i, pos[1]};
//                    strat_move_t move = {pos, pos_to};
//                    if(is_legal_move(board, move, flip_teams)) {
//                        canonize_move(m_shape, move);
//                        moves_possible.push_back(move);
//                    }
//
//                }
//                // all possible moves to the bottom until board ends
//                for(int i = 1; i < pos[1] + 1; ++i) {
//                    Position pos_to = {pos[0], pos[1] - i};
//                    strat_move_t move = {pos, pos_to};
//                    if(is_legal_move(board, move, flip_teams)) {
//                        canonize_move(m_shape, move);
//                        moves_possible.push_back(move);
//                    }
//                }
//            }
//            else {
//                // all moves are 1 step to left, right, top, or bottom
//                std::vector<Position> pos_tos = {{pos[0] + 1, pos[1]},
//                                              {pos[0]  , pos[1]+1},
//                                              {pos[0]-1,   pos[1]},
//                                              {pos[0]  , pos[1]-1}};
//                for(auto& pos_to : pos_tos) {
//                    strat_move_t move = {pos, pos_to};
//                    if(is_legal_move(board, move, flip_teams)) {
//                        canonize_move(m_shape, move);
//                        moves_possible.push_back(move);
//                    }
//                }
//            }
//        }
//    }
//    return moves_possible;
//}

std::vector<strat_move_t> LogicStratego::_get_poss_moves(const Board &board, int player) {
    int board_len = board.get_shape();
    std::vector<strat_move_t> moves_possible;
    for( auto elem = board.begin(); elem != board.end(); ++elem) {
        std::shared_ptr<Piece> piece = elem->second;
        if(!piece->is_null() && piece->get_team() == player && piece->get_flag_can_move()) {
            // the position we are dealing with
            Position pos = piece->get_position();

            if(piece->get_kin() == 2) {
                // all possible moves to the right until board ends
                for(int i = 1; i < board_len - pos[0]; ++i) {
                    Position pos_to = {pos[0] + i, pos[1]};
                    strat_move_t move = {pos, pos_to};
                    if(is_legal_move(board, move)) {
                        moves_possible.push_back(move);
                    }
                }
                // all possible moves to the top until board ends
                for(int i = 1; i < board_len - pos[1]; ++i) {
                    Position pos_to = {pos[0], pos[1] + i};
                    strat_move_t move = {pos, pos_to};
                    if(is_legal_move(board, move)) {
                        moves_possible.push_back(move);
                    }
                }
                // all possible moves to the left until board ends
                for(int i = 1; i < pos[0] + 1; ++i) {
                    Position pos_to = {pos[0] - i, pos[1]};
                    strat_move_t move = {pos, pos_to};
                    if(is_legal_move(board, move)) {
                        moves_possible.push_back(move);
                    }

                }
                // all possible moves to the bottom until board ends
                for(int i = 1; i < pos[1] + 1; ++i) {
                    Position pos_to = {pos[0], pos[1] - i};
                    strat_move_t move = {pos, pos_to};
                    if(is_legal_move(board, move)) {
                        moves_possible.push_back(move);
                    }
                }
            }
            else {
                // all moves are 1 step to left, right, top, or bottom
                std::vector<Position> pos_tos = {{pos[0] + 1, pos[1]},
                                                 {pos[0]  , pos[1]+1},
                                                 {pos[0]-1,   pos[1]},
                                                 {pos[0]  , pos[1]-1}};
                for(auto& pos_to : pos_tos) {
                    strat_move_t move = {pos, pos_to};
                    if(is_legal_move(board, move)) {
                        moves_possible.push_back(move);
                    }
                }
            }
        }
    }
    return moves_possible;
}

std::vector<strat_move_t> LogicStratego::get_poss_moves(const Board &board, int player, bool flip_board) {
    std::vector<strat_move_t> moves = _get_poss_moves(board, player);
    if(flip_board) {
        int board_len = board.get_shape();
        for(int i = 0; i < moves.size(); ++i) {
            _invert_move(moves[i], board_len);
        }
    }
    return moves;
}

bool LogicStratego::has_poss_moves(const Board &board, int player) {

    int board_len = board.get_shape();
    for( auto elem = board.begin(); elem != board.end(); ++elem) {
        std::shared_ptr<Piece> piece = elem->second;
        if(!piece->is_null() && piece->get_team() == player && piece->get_flag_can_move()) {
            // the position we are dealing with
            Position pos = piece->get_position();

            if(piece->get_kin() == 2) {
                // all possible moves to the right until board ends
                for(int i = 1; i < board_len - pos[0]; ++i) {
                    Position pos_to = {pos[0] + i, pos[1]};
                    strat_move_t move = {pos, pos_to};
                    if(is_legal_move(board, move))
                        return true;
                }
                // all possible moves to the top until board ends
                for(int i = 1; i < board_len - pos[1]; ++i) {
                    Position pos_to = {pos[0], pos[1] + i};
                    strat_move_t move = {pos, pos_to};
                    if(is_legal_move(board, move))
                        return true;
                }
                // all possible moves to the left until board ends
                for(int i = 1; i < pos[0] + 1; ++i) {
                    Position pos_to = {pos[0] - i, pos[1]};
                    strat_move_t move = {pos, pos_to};
                    if(is_legal_move(board, move))
                        return true;
                }
                // all possible moves to the bottom until board ends
                for(int i = 1; i < pos[1] + 1; ++i) {
                    Position pos_to = {pos[0], pos[1] - i};
                    strat_move_t move = {pos, pos_to};
                    if(is_legal_move(board, move))
                        return true;
                }
            }
            else {
                // all moves are 1 step to left, right, top, or bottom
                std::vector<Position> pos_tos = {{pos[0] + 1, pos[1]},
                                                 {pos[0]  , pos[1]+1},
                                                 {pos[0]-1,   pos[1]},
                                                 {pos[0]  , pos[1]-1}};
                for(auto& pos_to : pos_tos) {
                    strat_move_t move = {pos, pos_to};
                    if(is_legal_move(board, move))
                        return true;
                }
            }
        }
    }
    return false;
}

int LogicStratego::_find_action_idx(std::vector<strat_move_base_t> &vec_to_search, strat_move_base_t &action_to_find) {

    // func to find a specific vector in another (assuming types are all known)

    int idx = 0;
    std::vector<strat_move_base_t >::iterator entry;
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

/**
 * Method to check a move (as given by @pos and @pos_to) for validity on the @board. If the move
 * is legal, its associated action will be enabled (vector position set to 1) in the provided @action_mask.
 * The user needs to also provide the vector of the action representation and the vector of the action range
 * that this action belongs to, in order to search for the correct index of the action.
 * If @flip_board is true, the move change is inverted (multiplied comp. wise by -1). This becomes necessary,
 * since the action representation will need to be consistent for the neural net. As such, the action_mask for
 * player 1 needs to have the correct action associated for a flipped board. The example further demonstrates
 * this need.
 *
 * Example:
 * Imagine a board (table) in which the top row is index 0 and the leftmost column is index 0
 *      For player 0: The move m := (0,4) -> (2,4) means 'move 2 rows DOWN' -> action effect (2,0)
 *      For player 1: The move n := (4,0) -> (2,0) means 'move 2 rows UP'   -> action effect (-2,0)
 * However as the NN needs its representation always from the perspective of player 0, the move n of player 1
 * is translated to move m, and as such the action effect needs to be flipped.
 *
 * @param action_mask binary vector of action validity. Position i says action i is valid or invalid.
 * @param board the game board depicting the current scenario to which the action mask is sought.
 * @param act_range_start the start index of the action range that this potentially valid move falls into.
 * @param action_arr the vector of the action representation.
 * @param act_range the range of actions in which to search for the represented move.
 * @param pos the starting positon.
 * @param pos_to the target position.
 * @param flip_board switch to flip the for player 1.
 */
void LogicStratego::enable_action_if_legal(std::vector<int>& action_mask, const Board& board,
                                           int act_range_start,
                                           const std::vector<strat_move_base_t >& action_arr,
                                           const std::vector<int>& act_range,
                                           const Position& pos, const Position& pos_to,
                                           const bool flip_board) {

    strat_move_t move = {pos, pos_to};

    if(is_legal_move(board, move)) {
        strat_move_base_t action_effect;

        if(flip_board)
            action_effect = {pos[0] - pos_to[0], pos[1] - pos_to[1]};
        else
            action_effect = {pos_to[0] - pos[0], pos_to[1] - pos[1]};
        std::vector<strat_move_base_t > slice(act_range.size());
        for(unsigned long idx = 0; idx < slice.size(); ++idx) {
            slice[idx] = action_arr[act_range[idx]];
        }
        int idx = LogicStratego::_find_action_idx(slice, action_effect);
        action_mask[act_range_start + idx] = 1;
    }
};


std::vector<int> LogicStratego::get_action_mask(
        const Board &board, const std::vector<strat_move_base_t >& action_arr,
        const std::map<std::array<int, 2>, std::tuple<int, std::vector<int>>>& piece_act_map,
        int player) {

    std::vector<int> action_mask(action_arr.size(), 0);
    int board_len = board.get_shape();

    for( auto elem = board.begin(); elem != board.end(); ++elem) {
        std::shared_ptr<Piece> piece = elem->second;
        if(!piece->is_null() && piece->get_team() == player && piece->get_flag_can_move()) {

            std::array<int, 2> type_ver = {piece->get_kin(), piece->get_version()};
            const auto& [start_idx, act_range] = piece_act_map.at(type_ver);

            // the position we are dealing with
            Position pos = piece->get_position();

            std::vector<Position> all_pos_targets(4);
            if(piece->get_kin() == 2) {
                all_pos_targets.resize(board_len - pos[0] - 1 + board_len - pos[1] - 1 + pos[0] + pos[1]);
                auto all_pos_targets_it = all_pos_targets.begin();
                // all possible moves to the top until board ends
                for(int i = 1; i < board_len - pos[0]; ++i, ++all_pos_targets_it) {
                    *all_pos_targets_it = {pos[0] + i, pos[1] + 0};
                }
                // all possible moves to the right until board ends
                for(int i = 1; i < board_len - pos[1]; ++i, ++all_pos_targets_it) {
                    *all_pos_targets_it = {pos[0] + 0, pos[1] + i};
                }
                // all possible moves to the bottom until board ends
                for(int i = 1; i < pos[0] + 1; ++i, ++all_pos_targets_it) {
                    *all_pos_targets_it = {pos[0] - i, pos[1] + 0};
                }
                // all possible moves to the left until board ends
                for(int i = 1; i < pos[1] + 1; ++i, ++all_pos_targets_it) {
                    *all_pos_targets_it = {pos[0] + 0, pos[1] -i};
                }
            }
            else {
                // all moves are 1 step to left, right, top, or bottom
                all_pos_targets[0] = {pos[0] + 1, pos[1]};
                all_pos_targets[1] = {pos[0], pos[1] + 1};
                all_pos_targets[2] = {pos[0] - 1, pos[1]};
                all_pos_targets[3] = {pos[0], pos[1] - 1};

            }
            for(auto& pos_to : all_pos_targets) {
                LogicStratego::enable_action_if_legal(action_mask, board,
                                                      start_idx, action_arr, act_range,
                                                      pos, pos_to,
                                                      player);
            }
        }
    }
    return action_mask;
}