//
// Created by Michael on 20/02/2019.
//

#pragma once

#include "map"
#include "array"
#include "vector"
#include "memory"

#include "PieceStratego.h"
#include "include/azpp/board.h"


class BoardStratego : public Board<PieceStratego> {

public:
    using base_type = Board<PieceStratego>;
    // inheriting base constructors
    using base_type::base_type;
    // also specializing one
    BoardStratego(const std::array<size_t, m_dim> &shape,
                  const std::map<position_type, int> &setup_0,
                  const std::map<position_type, int> &setup_1)
            : base_type(shape, adapt_setup(setup_0), adapt_setup(setup_1))
            {}
    std::string to_string_2D(bool flip_board, bool hide_unknowns) const override;
private:
    static std::vector<std::shared_ptr<piece_type>> adapt_setup(const std::map<position_type , int> &setup);


};


