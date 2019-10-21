//
// Created by michael on 13.08.19.
//

#ifndef STRATEGO_CPP_A0AGENT_H
#define STRATEGO_CPP_A0AGENT_H

#include "../../agent/AgentReinforce.h"

template <template <typename, typename> class Board, typename Piece, typename Position, typename Move>
class AlphaZeroAgent : public AgentReinforceBase {
    using Base = AgentReinforceBase;
    static inline bool check_condition(const std::shared_ptr<Piece>& piece, int team, int type, int version, bool hidden);
    inline std::vector<std::tuple<int, int, int, bool>> create_conditions();

public:

    explicit AlphaZeroAgent(int team, bool learner, const std::shared_ptr<NetworkWrapper> & model_sptr)
            : AgentReinforceBase(team, true, model_sptr)
    {
        return StateRepresentation::b2s_cond_check(board, Base::m_conditions);
    }
    void install_board(const Board<Piece, Position>& board) override;
    torch::Tensor board_to_state_rep(const Board<Piece, Position>& board) override {
        Base::install_board(board);
        Base::m_conditions = create_conditions();
        Base::m_state_dim = Base::m_conditions.size();
    };
    Move decide_move(const Board<Piece, Position>& board) override {
        torch::Tensor board_state = board_to_state_rep(board);
        auto [pi, v] = Base::m_model->predict(board_state);

        int action = pi.argmax().template item<int64_t>();

        strat_move_t move = ActionRep::action_to_move(action, Base::m_action_dim, Base::m_board_len, Base::m_actors, m_team);

        return move;
    };

};



#endif //STRATEGO_CPP_A0AGENT_H
