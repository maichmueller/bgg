//
// Created by michael on 17.04.19.
//

#ifndef STRATEGO_CPP_TORCH_UTILS_H
#define STRATEGO_CPP_TORCH_UTILS_H


#include "torch/torch.h"
#include "type_traits"

#include "utils.h"


namespace torch_utils {

    struct GLOBAL_DEVICE {
        // if CUDA is available, we are using it. Otherwise CPU
        static const torch::DeviceType global_dev;

        static auto& get_device() {
            return global_dev;
        }
    };

    template <typename Val_Type>
    struct NestedVectorManip {
        static const int dimension = 0;

        template <typename size_type>
        static void sizes(const Val_Type&, std::vector<size_type>&) {}

    };

    template <template <class, class> class Vec, typename Val_Type, typename Alloc>
    struct NestedVectorManip<Vec<Val_Type, Alloc>> {
        static const int dimension = 1 + NestedVectorManip<Val_Type>::dimension;

        template <typename size_type>
        static void sizes(const Vec<Val_Type,Alloc>& v, std::vector<size_type>& size)
        {
            size.push_back(v.size());
            NestedVectorManip<Val_Type>::sizes(v[0],size);
        }
    };

    // specialized fill tensor function for the one dim case
    template <template <class, class> class Container, typename Val_Type, typename Alloc>
    static void fill_tensor(const Container<Val_Type, Alloc>& vec,
                            torch::Tensor& tensor_to_fill) {

        for( int i = 0; i < vec.size(); ++i) {
            tensor_to_fill[i] = vec[i];
        }
    }

    template <template <class, class> class OuterContainer, template <class, class> class InnerContainer, typename OuterAlloc,
                                        typename InnerDType, typename InnerAlloc>
    static void fill_tensor(const OuterContainer<InnerContainer<InnerDType, InnerAlloc>, OuterAlloc>& vec,
                            torch::Tensor& tensor_to_fill) {

        for( int i = 0; i < vec.size(); ++i) {
            InnerContainer<InnerDType, InnerAlloc> next_level_vec = vec[i];

            // assert that vec is indeed a std::vector
            static_assert(std::is_same< decltype(next_level_vec), std::vector<InnerDType, InnerAlloc>>::value,
                          "InnerVec is not a vector");

            auto next_level_tensor = tensor_to_fill[i];
            fill_tensor(next_level_vec, next_level_tensor);

        }
    }

    template <typename DType>
    torch::Tensor tensor_from_vector(const std::vector<DType>& vec) {
        /// Copies the vector (even nested vectors in vectors) data 
        /// into a tensor and returns this tensor by copy.

//        // get the number of nested vector<vector<...<vector<TYPE>...>>
//        int dimensions = NestedVectorManip<std::vector<DType>>::dimension;
        // init a shape vector catching the sizes of each nested vector
        std::vector<int64_t > shape;
        NestedVectorManip<std::vector<DType>>::sizes(vec, shape);
        // pass shape vector to ArrayRef (which is holding only the pointer to the vector)
        // for torch API
        torch::ArrayRef<int64_t> tensor_shape(shape);
        auto build_details = torch::TensorOptions().dtype(torch::kInt64);
        torch::Tensor tensor_to_fill = torch::zeros(tensor_shape, build_details);
//        auto tensor_access = tensor_to_fill.accessor<int64_t, NestedVectorManip<std::vector<DType>>::dimension>();

        fill_tensor(vec, tensor_to_fill);

        return tensor_to_fill;
    }

    template <typename DType>
    torch::Tensor tensor_from_vector(const std::vector<DType>& vec, torch::TensorOptions& options) {
        /// Copies the vector (even nested vectors in vectors) data
        /// into a tensor and returns this tensor by copy.

//        // get the number of nested vector<vector<...<vector<TYPE>...>>
//        int dimensions = NestedVectorManip<std::vector<DType>>::dimension;
        // init a shape vector catching the sizes of each nested vector
        std::vector<int64_t > shape;
        NestedVectorManip<std::vector<DType>>::sizes(vec, shape);
        // pass shape vector to ArrayRef (which is holding only the pointer to the vector)
        // for torch API
        torch::ArrayRef<int64_t> tensor_shape(shape);

        torch::Tensor tensor_to_fill = torch::zeros(tensor_shape, options);
//        auto tensor_access = tensor_to_fill.accessor<int64_t, NestedVectorManip<std::vector<DType>>::dimension>();

        fill_tensor(vec, tensor_to_fill);

        return tensor_to_fill;
    }

};


namespace torch_utils::StateRep {

    template <typename Piece>
    bool check_condition(const std::shared_ptr<Piece>& piece,
                         int team,
                         int type,
                         int version,
                         bool hidden) {

        if(team == 0) {
            if(!hidden) {
                // if it is about team 0, the 'hidden' status is unimportant
                // (since the alpha zero agent alwazs plays from the perspective
                // of player 0, therefore it can see all its own pieces).
                bool eq_team = piece->get_team() == team;
                bool eq_type = piece->get_type() == type;
                bool eq_vers = piece->get_version() == version;
                return eq_team && eq_type && eq_vers;
            }
            else {
                // 'hidden' is only important for the single condition that specifically
                // checks for this property (information about own pieces visible or not).
                bool eq_team = piece->get_team() == team;
                bool hide = piece->get_flag_hidden() == hidden;
                return eq_team && hide;
            }
        }
        else if(team == 1) {
            // for team 1 we only get the info about type and version if it isn't hidden
            // otherwise it will fall into the 'hidden' layer
            if(!hidden) {
                if(piece->get_flag_hidden())
                    return false;
                else {
                    bool eq_team = piece->get_team() == team;
                    bool eq_type = piece->get_type() == type;
                    bool eq_vers = piece->get_version() == version;
                    return eq_team && eq_type && eq_vers;
                }
            }
            else {
                bool eq_team = piece->get_team() == team;
                bool hide = piece->get_flag_hidden() == hidden;
                return eq_team && hide;
            }
        }
        else {
            // only the obstacle should reach here
            return piece->get_team() == team;
        }
    }


    template < typename Board>
    torch::Tensor b2s_cond_check(const Board &board,
                                     int state_dim,
                                     int board_len,
                                     const std::vector<std::tuple<int, int, int, bool>>& conditions) {
        /*
         * We are trying to build a state representation of a Stratego board.
         * To this end, i will define conditions that are evaluated for each
         * piece on the board. These conditions are checked in sequence.
         * Each condition receives its own layer with 0's everywhere, except
         * for where the specific condition was true, which holds a 1.
         * In short: x conditions -> x binary layers (one for each condition)
         */

        torch::Tensor board_state_rep = torch::zeros({1, state_dim, board_len, board_len});
        auto board_state_access = board_state_rep.accessor<int, 4> ();
        for(const auto& pos_piece : board) {
            pos_type pos = pos_piece.first;
            auto piece = pos_piece.second;
            if(!piece->is_null()) {
                // TODO: Check if rvalue ref here is suitable
                for(auto&& [i, cond_it] = std::make_tuple(0, conditions.begin()); cond_it != conditions.end(); ++i, ++cond_it) {
                    // unpack the condition
                    auto [team, type, vers, hidden] = *cond_it;
                    // write the result of the condition check to the tensor
                    board_state_access[0][i][pos[0]][pos[1]] = check_condition(piece, team, type, vers, hidden);
                }
            }
        }
        // send the tensor to the global device
        board_state_rep.to(torch_utils::GLOBAL_DEVICE::get_device());

        return board_state_rep;
    }

    using cond_type = std::tuple<int, int, int, bool>;

    std::vector<cond_type> create_conditions(const std::map<int, unsigned int>& type_counter,
                                             int own_team) {

        std::vector<std::tuple<int, int, int, bool>> conditions(21);

        // own team 0
        // [flag, 1, 2, 3, 4, ..., 10, bombs] UNHIDDEN
        for(const auto& entry : type_counter) {
            int type = entry.first;
            for (int version = 1; version <= entry.second; ++version) {
                conditions.emplace_back(std::make_tuple(own_team, type, version, false));
            }
        }
        // [all own pieces] HIDDEN
        // Note: type and version info are being "short-circuited" (unused)
        // in the check in this case (thus -1)
        conditions.emplace_back(std::make_tuple(own_team, -1, -1, false));

        // enemy team 1
        // [flag, 1, 2, 3, 4, ..., 10, bombs] UNHIDDEN
        for(const auto& entry : type_counter) {
            int type = entry.first;
            for (int version = 1; version <= entry.second; ++version) {
                conditions.emplace_back(std::make_tuple(1-own_team, type, version, false));
            }
        }
        // [all enemy pieces] HIDDEN
        // Note: type and version info are being "short-circuited" (unused)
        // in the check in this case (thus -1)
        conditions.emplace_back(std::make_tuple(1-own_team, -1, -1, false));

        return conditions;
    }

    std::vector<cond_type> state_torch_conv_conditions_0{};
    std::vector<cond_type> state_torch_conv_conditions_1{};

    void set_state_rep_conditions(int game_len) {
        if (game_len == 5) {
            auto t_count_s = utils::counter(GameDeclarations::get_available_types(5));
            state_torch_conv_conditions_0 = create_conditions(t_count_s, 0);
            state_torch_conv_conditions_1 = create_conditions(t_count_s, 1);
        } else if (game_len == 7) {
            auto t_count_m = utils::counter(GameDeclarations::get_available_types(7));
            state_torch_conv_conditions_0 = create_conditions(t_count_m, 0);
            state_torch_conv_conditions_1 = create_conditions(t_count_m, 1);
        } else if (game_len == 10) {
            auto t_count_l = utils::counter(GameDeclarations::get_available_types(10));
            state_torch_conv_conditions_1 = create_conditions(t_count_l, 1);
            state_torch_conv_conditions_0 = create_conditions(t_count_l, 0);
        }
    }
}


#endif //STRATEGO_CPP_TORCH_UTILS_H
