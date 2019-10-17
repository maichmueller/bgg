
#pragma once

#include "../../board/Position.h"
#include "../../board/Move.h"

template <typename Vector, typename KinType>
struct Action {

    using vector_type = Vector; // not associated with the std::vector
    using kin_type = KinType;

    vector_type m_effect_pl_0;
    vector_type m_effect_pl_1;
    kin_type m_piece_id;

    Action(vector_type action, kin_type piece_identifier)
            : m_effect_pl_0(action), m_effect_pl_1(action.invert()), m_piece_id(piece_identifier)
    {}

    [[nodiscard]] vector_type get_effect(int player) const {
        if(player) return m_effect_pl_1;
        else return m_effect_pl_0;
    }
    [[nodiscard]] kin_type get_piece_id() const {return m_piece_id;}

    template <typename LengthType, size_t dim>
    Move<Position<LengthType, dim>> to_move(const Position<LengthType, dim> & pos, int player) {
        if(player==0)
            return pos + m_effect_pl_0;
        else
            return pos + m_effect_pl_1;
    }
};
