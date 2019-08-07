//
// Created by Michael on 22/02/2019.
//

#ifndef STRATEGO_CPP_GAMEDECLARATIONS_H
#define STRATEGO_CPP_GAMEDECLARATIONS_H

#include <numeric>
#include "array"
#include "vector"
#include "map"
#include "unordered_map"
#include "algorithm"
#include "memory"

#include "utils.h"


namespace GameDeclarations {
    // should be of length 2 only!
    using pos_t = std::array<int, 2>;
    using move_base_t = pos_t;
    using move_t = std::array<pos_t, 2>;

    const std::vector<int> available_types_s = {0, 1, 2, 2, 2, 3, 3, 10, 11, 11};
    const std::vector<int> available_types_m = {0, 1, 2, 2, 2, 2, 2, 3, 3, 3, 4,
                                                4, 4, 5, 5, 6, 10, 11, 11, 11, 11};
    const std::vector<int> available_types_l = {0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3,
                                                3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5,
                                                6, 6, 6, 6, 7, 7, 7, 8, 8, 9, 10,
                                                11, 11, 11, 11, 11, 11};

    const std::vector<pos_t> start_pos_0_s = {{0, 0},
                                              {0, 1},
                                              {0, 2},
                                              {0, 3},
                                              {0, 4},
                                              {1, 0},
                                              {1, 1},
                                              {1, 2},
                                              {1, 3},
                                              {1, 4}};
    const std::vector<pos_t> start_pos_1_s = {{4, 0},
                                              {4, 1},
                                              {4, 2},
                                              {4, 3},
                                              {4, 4},
                                              {3, 0},
                                              {3, 1},
                                              {3, 2},
                                              {3, 3},
                                              {3, 4}};

    const std::vector<pos_t> start_pos_0_m = {{0, 0},
                                              {0, 1},
                                              {0, 2},
                                              {0, 3},
                                              {0, 4},
                                              {0, 5},
                                              {0, 6},
                                              {1, 0},
                                              {1, 1},
                                              {1, 2},
                                              {1, 3},
                                              {1, 4},
                                              {1, 5},
                                              {1, 6},
                                              {2, 0},
                                              {2, 1},
                                              {2, 2},
                                              {2, 3},
                                              {2, 4},
                                              {2, 5},
                                              {2, 6}};
    const std::vector<pos_t> start_pos_1_m = {{4, 0},
                                              {4, 1},
                                              {4, 2},
                                              {4, 3},
                                              {4, 4},
                                              {4, 5},
                                              {4, 6},
                                              {5, 0},
                                              {5, 1},
                                              {5, 2},
                                              {5, 3},
                                              {5, 4},
                                              {5, 5},
                                              {5, 6},
                                              {6, 0},
                                              {6, 1},
                                              {6, 2},
                                              {6, 3},
                                              {6, 4},
                                              {6, 5},
                                              {6, 6}};

    const std::vector<pos_t> start_pos_0_l = {{0, 0},
                                              {0, 1},
                                              {0, 2},
                                              {0, 3},
                                              {0, 4},
                                              {0, 5},
                                              {0, 6},
                                              {0, 7},
                                              {0, 8},
                                              {0, 9},
                                              {1, 0},
                                              {1, 1},
                                              {1, 2},
                                              {1, 3},
                                              {1, 4},
                                              {1, 5},
                                              {1, 6},
                                              {1, 7},
                                              {1, 8},
                                              {1, 9},
                                              {2, 0},
                                              {2, 1},
                                              {2, 2},
                                              {2, 3},
                                              {2, 4},
                                              {2, 5},
                                              {2, 6},
                                              {2, 7},
                                              {2, 8},
                                              {2, 9},
                                              {3, 0},
                                              {3, 1},
                                              {3, 2},
                                              {3, 3},
                                              {3, 4},
                                              {3, 5},
                                              {3, 6},
                                              {3, 7},
                                              {3, 8},
                                              {3, 9}};

    const std::vector<pos_t> start_pos_1_l = {{6, 0},
                                              {6, 1},
                                              {6, 2},
                                              {6, 3},
                                              {6, 4},
                                              {6, 5},
                                              {6, 6},
                                              {6, 7},
                                              {6, 8},
                                              {6, 9},
                                              {7, 0},
                                              {7, 1},
                                              {7, 2},
                                              {7, 3},
                                              {7, 4},
                                              {7, 5},
                                              {7, 6},
                                              {7, 7},
                                              {7, 8},
                                              {7, 9},
                                              {8, 0},
                                              {8, 1},
                                              {8, 2},
                                              {8, 3},
                                              {8, 4},
                                              {8, 5},
                                              {8, 6},
                                              {8, 7},
                                              {8, 8},
                                              {8, 9},
                                              {9, 0},
                                              {9, 1},
                                              {9, 2},
                                              {9, 3},
                                              {9, 4},
                                              {9, 5},
                                              {9, 6},
                                              {9, 7},
                                              {9, 8},
                                              {9, 9}};


    const std::vector<pos_t> obstacle_pos_s = {{2, 2}};
    const std::vector<pos_t> obstacle_pos_m = {{3, 1},
                                               {3, 5}};
    const std::vector<pos_t> obstacle_pos_l = {{4, 2},
                                               {5, 2},
                                               {4, 3},
                                               {5, 3},
                                               {4, 6},
                                               {5, 6},
                                               {4, 7},
                                               {5, 7}};

    constexpr inline auto const &get_obstacle_pos(int game_len) {
        if (game_len == 5)
            return obstacle_pos_s;
        else if (game_len == 7)
            return obstacle_pos_m;
        else if (game_len == 10)
            return obstacle_pos_l;
        else
            throw std::invalid_argument("'game_len' not in [5, 7, 10].");
    }

    constexpr inline auto const &get_available_types(int game_len) {
        if (game_len == 5)
            return available_types_s;
        else if (game_len == 7)
            return available_types_m;
        else if (game_len == 10)
            return available_types_l;
        else
            throw std::invalid_argument("'game_len' not in [5, 7, 10].");
    }

    constexpr inline auto const &get_start_positions(int game_len, int team) {
        if (game_len == 5) {
            if (team == 0)
                return start_pos_0_s;
            else if (team == 1)
                return start_pos_1_s;
            else
                throw std::invalid_argument("'team' not in [0, 1].");
        } else if (game_len == 7) {
            if (team == 0)
                return start_pos_0_m;
            else if (team == 1)
                return start_pos_1_m;
            else
                throw std::invalid_argument("'team' not in [0, 1].");
        } else if (game_len == 10) {
            if (team == 0)
                return start_pos_0_s;
            else if (team == 1)
                return start_pos_1_s;
            else
                throw std::invalid_argument("'team' not in [0, 1].");
        } else
            throw std::invalid_argument("'game_len' not in [5, 7, 10].");
    }
}

using namespace GameDeclarations;

struct key_comp {
    bool operator()(const pos_t &lhs, const pos_t &rhs) const {
        if (lhs[0] < rhs[0]) {
            return true;
        } else if (lhs[0] == rhs[0]) {
            if (lhs[1] < rhs[1]) {
                return true;
            } else {
                // on equivalence == or greater > case it has to return false
                return false;
            }
        } else {
            return false;
        }
    }
};

struct ActionRep {

    static const std::vector<move_base_t > action_ar_s;
    static const std::vector<move_base_t > action_ar_m;
    static const std::vector<move_base_t > action_ar_l;

    static const std::map<std::array<int, 2>, std::tuple<int, std::vector<int>>> piece_act_map_s;
    static const std::map<std::array<int, 2>, std::tuple<int, std::vector<int>>> piece_act_map_m;
    static const std::map<std::array<int, 2>, std::tuple<int, std::vector<int>>> piece_act_map_l;

    static int fill_act_vector(
            std::vector<move_base_t > &action_ar,
            std::vector<int> const &available_types,
            std::map<std::array<int, 2>, std::tuple<int, std::vector<int>>> &piece_act_map
    ) {
        int curr_idx = 0;
        int curr_type = -1;
        int curr_version = -1;
        /*
            we want to iterate over every type of piece (as often as this type exists)
            and add the move-changes, i.e. the pos-vector that will need to be added to the
            current position of the piece. These quasi-moves for each type are added in
            sequence to the action_arr.
        */
        for (auto &type : available_types) {
            if (0 < type && type < 11) {
                if (curr_type != type) {
                    curr_type = type;
                    curr_version = 0;
                } else
                    curr_version += 1;

                int start_idx = curr_idx;
                // if its of type 2 it can reach further -> encoded in the step_len
                int step_len = 1;
                if (type == 2)
                    step_len = 4;
                // add all four directions in which the piece can walk
                for (int i = 1; i < step_len + 1; ++i) {
                    action_ar[curr_idx] = {0, i};
                    action_ar[curr_idx + 1] = {i, 0};
                    action_ar[curr_idx + 2] = {-i, 0};
                    action_ar[curr_idx + 3] = {0, -i};
                    curr_idx += 4;
                }
                // here we remember the index numbers of the moves, that are assigned to this
                // specific piece type and version.
                std::vector<int> this_assign(static_cast<unsigned long> (curr_idx - start_idx));
                std::iota(this_assign.begin(), this_assign.end(), start_idx);
                piece_act_map[{curr_type, curr_version}] = std::tuple(start_idx, this_assign);
            }
        }
        return 0;
    }

    static auto const &get_act_rep(int game_len) {
        if (game_len == 5) return action_ar_s;
        else if (game_len == 7) return action_ar_m;
        else if (game_len == 10) return action_ar_l;
        else throw std::invalid_argument("Game length not in [5, 7, 10].");
    }

    static auto const &get_act_map(int game_len) {
        if (game_len == 5) return piece_act_map_s;
        else if (game_len == 7) return piece_act_map_m;
        else if (game_len == 10) return piece_act_map_l;
        else throw std::invalid_argument("Game length not in [5, 7, 10].");
    }

    template<typename Piece>
    static move_t action_to_move(int action, int action_dim, int board_len,
                                 std::unordered_map<std::tuple<int, int>,
                                    std::shared_ptr<Piece>,
                                    hash_tuple::hash<std::tuple<int, int> >,
                                    eqcomp_tuple::eqcomp<std::tuple<int, int> > > const &actors) {

        if (action < 0 || action >= action_dim)
            throw std::invalid_argument("Action index out of range.");

        int type = -1;
        int version = -1;
        // Iterate over the piece_action_map to find the type and the version of the
        // piece belonging to this action index.
        for (auto const&[type_ver_vec, ind_vec_and_idx] : ActionRep::get_act_map(board_len)) {
            auto ind_vec = std::get<1>(ind_vec_and_idx);
            auto find_action_ind = std::find(ind_vec.begin(), ind_vec.end(), action);
            if (find_action_ind != ind_vec.end()) {
                type = type_ver_vec[0];
                version = type_ver_vec[1];
                break;
            }
        }
        // error checking. If we didn't find anything, then there is something wrong in the logic.
        if (type == -1) {
            std::string err_msg = "Couldn't find type and version for action index " + std::to_string(action) + ".";
            throw std::logic_error(err_msg);
        }
        std::shared_ptr<Piece> actor = actors.at(std::make_tuple(type, version));
        pos_t move_change = ActionRep::get_act_rep(board_len)[action];
        pos_t curr_pos = actor->get_position();
        pos_t new_pos = {curr_pos[0] + move_change[0], curr_pos[1] + move_change[1]};
        move_t move{curr_pos, new_pos};
        return move;
    }

};


#endif //STRATEGO_CPP_GAMEDECLARATIONS_H