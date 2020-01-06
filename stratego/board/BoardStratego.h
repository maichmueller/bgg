//
// Created by Michael on 20/02/2019.
//

#pragma once

#include "map"
#include "array"
#include "vector"
#include "memory"

#include "PieceStratego.h"
#include "azpp/board.h"


class BoardStratego : public Board<PieceStratego> {

public:
    using base_type = Board<PieceStratego>;

    // also specializing one
    BoardStratego(const std::array<size_t, 2> &shape,
                  const std::map<position_type, int> &setup_0,
                  const std::map<position_type, int> &setup_1)
            :
            base_type(
                    shape,
                    adapt_setup(setup_0),
                    adapt_setup(setup_1)
            ) {
        _add_obstacles();
    }

    // decorating the constructors with add_obstacles after construction
    template <typename ... Params>
    BoardStratego(Params... params)
    : base_type(params...) {
        _add_obstacles();
    }

    [[nodiscard]] std::string print_board(int player, bool hide_unknowns) const override;

private:
    void _add_obstacles();
    static std::vector<std::shared_ptr<piece_type>> adapt_setup(const std::map<position_type, int> &setup);


};


