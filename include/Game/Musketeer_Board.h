#ifndef MUSKETEER_BOARD_H
#define MUSKETEER_BOARD_H

#include "Board.h"

#include <string>
#include <array>

#include "Game/Piece.h"

class Musketeer_Board : public Board
{
    public:
        Musketeer_Board(const std::string& input_fen);

    protected:
        void other_move_effects(const Move& move) noexcept override;

    private:
        std::array<std::array<Piece, 8>, 2> gated_pieces{}; // indexed by gated_pieces[Color index][File index]
};

#endif // MUSKETEER_BOARD_H