#pragma once
#include "aze/board/Position.h"

namespace {
template < size_t N >
using int_pos = Position< int, N >;
template < size_t N >
using unsigned_int_pos = Position< unsigned int, N >;
template < size_t N >
using float_pos = Position< float, N >;
template < size_t N >
using double_pos = Position< double, N >;
};  // namespace
