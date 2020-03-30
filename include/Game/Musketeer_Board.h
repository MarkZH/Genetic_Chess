#ifndef MUSKETEER_BOARD_H
#define MUSKETEER_BOARD_H

#include "Board.h"

#include <string>
#include <array>
#include <memory>

#include "Game/Piece.h"

class Move;

class Musketeer_Board : public Board
{
    public:
        Musketeer_Board() noexcept;
        explicit Musketeer_Board(const std::string& input_fen);

        std::unique_ptr<Board> copy() const noexcept override;

        std::string fen() const noexcept override;
        std::string extra_move_mark(const Move& move) const noexcept override;

    protected:
        void other_move_effects(const Move& move) noexcept override;

    private:
        std::array<std::array<Piece, 8>, 2> gated_pieces{}; // indexed by gated_pieces[Color index][File index]
        std::string starting_fen;

        void pick_and_place_random_gated_pieces() noexcept;
        std::string gate_fen(Piece_Color color) const noexcept;
        void set_unmoved_gate_guardians() noexcept;
};

#endif // MUSKETEER_BOARD_H