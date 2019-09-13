//
// Created by michael on 08.08.19.
//

#pragma once

#include <numeric>
#include "array"
#include "vector"
#include "map"
#include "unordered_map"
#include "memory"

#include "torch/torch.h"

#include "../../board/Position.h"
#include "../../board/Move.h"
#include "../../game/GameState.h"

#include <experimental/type_traits>


template <typename T>
using enable_representation_t = decltype( std::declval<T&>().enable_representation() );
template <typename T>
using get_action_rep_t = decltype( std::declval<T&>().get_action_rep() );
template <typename T>
using state_representation_t = decltype( std::declval<T&>().state_representation() );
template <typename T>
using update_actors_t = decltype( std::declval<T&>().update_actors() );


template<typename Action, typename Board>
class ActionRepBase {

public:
    using action_type = Action;
    using board_type = Board;
    using game_state_type = GameState<Board, ActionRepBase>;
    using kin_type = typename game_state_type::kin_type;

    template <typename T>
    explicit ActionRepBase(const T& obj) : object(std::make_shared<Model<T>>( std::move(obj) )) {
        static_assert(std::experimental::is_detected<enable_representation_t, decltype(obj)>::value,
                      "No method 'enable_representation' available!");
        static_assert(std::experimental::is_detected<get_action_rep_t, decltype(obj)>::value,
                      "No method 'get_action_rep' available!");
        static_assert(std::experimental::is_detected<state_representation_t, decltype(obj)>::value,
                      "No method 'state_representation' available!");
        static_assert(std::experimental::is_detected<update_actors_t, decltype(obj)>::value,
                      "No method 'update_actors' available!");
    }

    void enable_representation(const game_state_type& gs) { object.enable_representation(); }
    void update_actors(int team, kin_type kin) { object.update_actors(team, kin); }
    const std::vector<Action> * get_act_rep() { return object->get_action_rep_vector(); }
    torch::Tensor state_representation(int player) { return object->state_representation(player); }

    template <typename Position>
    Move<Position> action_to_move(const Position & pos, int action, int player) {
        return pos + get_act_rep()[action].get_effect(player);
    }

    struct Concept {
        virtual void enable_representation(const game_state_type & gs) = 0;
        virtual void update_actors(int team, kin_type kin) = 0;
        virtual const std::vector<Action> * get_action_rep_vector() = 0;
        virtual torch::Tensor state_representation(int player) = 0;
    };

    template< typename T>
    struct Model : Concept {
        explicit Model(const T& obj) : object(obj) {}
        void enable_representation(const game_state_type & gs) override {object.enable_representation(gs);}
        void update_actors(int team, kin_type kin) override { object.update_actors(team, kin); }
        const std::vector<Action> * get_act_rep() override { return object->get_action_rep_vector(); }
        torch::Tensor state_representation(int player) override { return object->state_representation(player); }

    private:
        T object;
    };

    std::shared_ptr<const Concept> object;
};


//
//template <typename Action, typename Piece>
//class ActionRepBase {
//
//public:
//    using action_type = Action;
//    using piece_type = Piece;
//
//    using kin_type = typename Action::kin_type;
//    using hasher = typename kin_type::hash;
//    using eq_comp = typename kin_type::eq_comp;
//    using piece_ident_map = std::unordered_map<kin_type, std::shared_ptr<piece_type>, hasher, eq_comp>;
//
//protected:
//    static const std::vector<action_type > action_rep_vector;
//    std::array<piece_ident_map, 2> actors{};
//
//public:
//    template <typename GameState>
//    virtual void enable_representation(GameState & gs) = 0;
//    static auto const & get_act_rep() { return action_rep_vector; }
//
//    template <typename Position>
//    Move<Position> action_to_move(const Position & pos, int action, int player) {
//        return pos + action_rep_vector[action].get_action(player);
//    }
//
//    virtual torch::Tensor state_representation(int player) = 0;
//};
//
