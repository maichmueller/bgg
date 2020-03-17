
#pragma once

#include "aze/board/Move.h"
#include "aze/board/Position.h"

template < typename Vector, typename RoleType >
struct Action {
   using vector_type = Vector;  // not associated with the std::vector
   using role_type = RoleType;

   vector_type m_effect_vec;
   role_type m_assoc_role;
   size_t m_index;

   Action(
      vector_type displacement_vector, role_type piece_identifier, size_t index)
       : m_effect_vec(displacement_vector),
         m_assoc_role(piece_identifier),
         m_index(index)
   {
   }

   [[nodiscard]] vector_type get_effect() const { return m_effect_vec; }

   [[nodiscard]] role_type get_assoc_role() const { return m_assoc_role; }

   [[nodiscard]] size_t get_index() const { return m_index; }

   template < typename ValueType, size_t dim >
   Move< Position< ValueType, dim > > to_move(
      const Position< ValueType, dim > &pos, int player)
   {
      return pos + m_effect_vec;
   }
};

namespace std {
template < typename VectorType, typename RoleType >
struct hash< Action< VectorType, RoleType > > {
   size_t operator()(const Action< VectorType, RoleType > &action) const
   {
      return std::hash< RoleType >()(action.get_assoc_role());
   }
};
}  // namespace std
