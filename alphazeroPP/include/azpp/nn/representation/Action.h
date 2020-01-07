
#pragma once

#include "azpp/board/Position.h"
#include "azpp/board/Move.h"

template<typename Vector, typename KinType>
struct Action {

    using vector_type = Vector; // not associated with the std::vector
    using kin_type = KinType;

    vector_type m_effect_vec;
    kin_type m_assoc_kin;
    size_t m_index;

    Action(vector_type displacement_vector, kin_type piece_identifier, size_t index)
            : m_effect_vec(displacement_vector),
              m_assoc_kin(piece_identifier),
              m_index(index) {}

    [[nodiscard]] vector_type get_effect() const { return m_effect_vec; }

    [[nodiscard]] kin_type get_assoc_kin() const { return m_assoc_kin; }

    [[nodiscard]] size_t get_index() const { return m_index; }

    template<typename ValueType, size_t dim>
    Move<Position<ValueType, dim>> to_move(const Position<ValueType, dim> &pos, int player) {
        return pos + m_effect_vec;
    }
};


namespace std {
    template<typename VectorType, typename KinType>
    struct hash<Action<VectorType, KinType>> {
        size_t operator()(const Action<VectorType, KinType> &action) const {
            return std::hash<KinType>()(action.get_assoc_kin());
        }
    };
}
