//
// Created by michael on 08.08.19.
//

#ifndef STRATEGO_CPP_ACTIONREPRESENTATION_H
#define STRATEGO_CPP_ACTIONREPRESENTATION_H

#include "GameDeclarations.h"

#include <numeric>
#include "array"
#include "vector"
#include "map"
#include "unordered_map"
#include "memory"

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


    /*!
     * Method to convert from the action index @action to the associated move on the board.
     * E.g. as the action input 45 will be converted to the position change (0,2) which will
     * be added to the position of the associated piece to generate ((2,1),(2,3)) if the piece
     * associated to action 45 was at position (2,1).
     *
     * The attribute @player decides which of the two agents on the board to map as being player 0.
     * This is relevant for the current state representation for the neural network as it always evaluates
     * a board state from the perspective of player 0. Thus actions of player 1 will be flipped in orientation
     * to be moves as if it was player 0.
     *
     * @tparam Piece The piece type of the game. Relevant for later generalisation of the API
     * @param action int, the index of the action.
     * @param action_dim int, the total number of actions.
     * @param board_len int, the length of the board of the game.
     * @param actors std::unordered_map of int tuples to shared_ptr<Piece>. It maps from {piece_type, piece_version}
     * attribute pairs to actual shared_ptr of pieces on the board currently.
     * @param player int, the player for the state to move conversion. Flips the move essentially.
     * @return move_t, the associated move.
     */
    template<typename Piece>
    static move_t action_to_move(int action,
                                 int action_dim,
                                 int board_len,
                                 const std::unordered_map<
                                    std::tuple<int, int>,
                                    std::shared_ptr<Piece>,
                                    hash_tuple::hash<std::tuple<int, int> >,
                                    eqcomp_tuple::eqcomp<std::tuple<int, int> >
                                 > & actors,
                                 int player) {

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
        pos_t curr_pos = actor->get_position(player, board_len);
        pos_t new_pos = {curr_pos[0] + move_change[0], curr_pos[1] + move_change[1]};
        move_t move{curr_pos, new_pos};
        return move;
    }

};


#endif //STRATEGO_CPP_ACTIONREPRESENTATION_H