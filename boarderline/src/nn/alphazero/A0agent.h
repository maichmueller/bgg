//
// Created by michael on 13.08.19.
//

#ifndef STRATEGO_CPP_A0AGENT_H
#define STRATEGO_CPP_A0AGENT_H


class AlphaZeroAgent : public AgentReinforceBase {
    using Base = AgentReinforceBase;
    static inline bool check_condition(const std::shared_ptr<Piece>& piece, int team, int type, int version, bool hidden);
    inline std::vector<std::tuple<int, int, int, bool>> create_conditions();

public:

    explicit AlphaZeroAgent(int team, bool learner, const std::shared_ptr<NetworkWrapper> & model_sptr)
            : AgentReinforceBase(team, true, model_sptr)
    {}
    void install_board(const Board& board) override;
    torch::Tensor board_to_state_rep(const Board& board) override;
    strat_move_t decide_move(const Board& board) override;

};



#endif //STRATEGO_CPP_A0AGENT_H
