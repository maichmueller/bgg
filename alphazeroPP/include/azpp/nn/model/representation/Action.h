
#pragma once

#include "azpp/board/Position.h"
#include "azpp/board/Move.h"

template <typename Vector, typename KinType>
struct Action {

    using vector_type = Vector; // not associated with the std::vector
    using kin_type = KinType;

    vector_type m_displacement_vec_player;
    kin_type m_piece_id;

    Action(vector_type displacement_vector, kin_type piece_identifier)
            : m_displacement_vec_player(displacement_vector), m_piece_id(piece_identifier)
    {}

    [[nodiscard]] vector_type get_effect() const {return m_displacement_vec_player;}
    [[nodiscard]] kin_type get_piece_id() const {return m_piece_id;}

    template <typename LengthType, size_t dim>
    Move<Position<LengthType, dim>> to_move(const Position<LengthType, dim> & pos, int player) {
        return pos + m_displacement_vec_player;
    }
};
