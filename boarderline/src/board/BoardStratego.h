//
// Created by Michael on 20/02/2019.
//

#pragma once

#include "map"
#include "array"
#include "vector"
#include "memory"

#include "PieceStratego.h"
#include "Board.h"


class BoardStratego : public Board<PieceStratego, typename PieceStratego::position_type> {

public:
    using base_type = Board<PieceStratego, typename PieceStratego::position_type>;
    using base_type::piece_type;
    using base_type::position_type;
    using base_type::kin_type;
private:
    std::vector<std::shared_ptr<piece_type>> adapt_setup(const std::map<position_type , int> &setup);

    BoardStratego(const std::array<int, m_dim> &shape,
                                  const std::map<position_type , int> &setup_0,
                                  const std::map<position_type , int> &setup_1)
            : base_type(shape, adapt_setup(setup_0), adapt_setup(setup_1))
    {}

};


