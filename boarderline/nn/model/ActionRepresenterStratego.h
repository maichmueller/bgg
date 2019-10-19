
#pragma once

#include "ActionRepresenter.h"
#include "Action.h"
#include "../../game/GameStateStratego.h"


class GameStateStratego;


class ActionRepStratego : public ActionRepBase<Action<typename BoardStratego::position_type,
                                                      typename BoardStratego::piece_type::kin_type>,
                                               GameStateStratego> {
public:
    using state_type = GameStateStratego;
    using position_type = state_type::position_type;
    using kin_type = state_type::piece_type::kin_type;
    using move_type = state_type::move_type;
    using piece_type = state_type::piece_type;
    using board_type = state_type::board_type;
    using action_type = Action<position_type, kin_type>;
    static_assert(std::is_same<typename move_type::position_type, typename board_type::position_type>::value);

class ActionRepStrategoExectutor {
    using state_type = GameStateStratego;
    using position_type = state_type::position_type;
    using kin_type = state_type::piece_type::kin_type;

    using cond_type = std::tuple<int, kin_type, bool>;

    torch::Tensor state_representation(int action, int player);

    void assign_actors(state_type & gs) {
        for(const auto& entry: gs.get_board()) {
            const auto& piece = entry.second;
            if(!piece->is_null() && piece->get_kin() != kin_type{99, 99};
                    actors[piece->get_team()][piece->get_kin()] = piece;
        }
    }

    static int _fill_act_vector(std::vector<action_type > & action_ar,
                                int board_len);
    /*
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
    * @param Piece The piece type of the game. Relevant for later generalisation of the API
    * @param action int, the index of the action.
    * @param action_dim int, the total number of actions.
    * @param board_len int, the length of the board of the game.
    * @param actors std::unordered_map of int tuples to shared_ptr<Piece>. It maps from {piece_type, piece_version}
    * attribute pairs to actual shared_ptr of pieces on the board currently.
    * @param player int, the player for the state to move conversion. Flips the move essentially.
    * @return move_t, the associated move.
    */
    template<typename Piece>
    move_type action_to_move(int action,
                             int action_dim,
                             int board_len,
                             const std::unordered_map<
                                     std::tuple<int, int>,
                                     std::shared_ptr<Piece>,
                                     tuple::hash<std::tuple<int, int> >,
                                     eqcomp_tuple::eqcomp<std::tuple<int, int> >
                             >  & actors,
                             int player);

    void enable_representation(const game_state_type & gs);
    void update_actors(int team, kin_type kin) { object.update_actors(team, kin); }
    const std::vector<Action> * get_act_rep() { return object->get_action_rep_vector(); }
    torch::Tensor state_representation(int player) { return object->state_representation(player); }

private:
    bool conditions_set;

};





