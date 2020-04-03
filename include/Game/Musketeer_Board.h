#ifndef MUSKETEER_BOARD_H
#define MUSKETEER_BOARD_H

#include "Board.h"

#include <string>
#include <array>
#include <memory>
#include <vector>

#include "Game/Piece.h"

class Move;

//! \brief A Board on which Musketeer Chess is played.
//!
//! https://musketeerchess.net/home/index.html
class Musketeer_Board : public Board
{
    public:
        //! \brief Create a board in the standard chess setup with randomly chosen and placed gated pieces.
        Musketeer_Board() noexcept;

        //! \brief Create a board according to a given FEN string.
        //!
        //! \param input_fen A string specifying the state of a Musketeer Chess board
        explicit Musketeer_Board(const std::string& input_fen);

        std::unique_ptr<Board> copy() const noexcept override;

        std::string fen() const noexcept override;
        std::string extra_move_mark(const Move& move) const noexcept override;

    protected:
        //! \brief Copy constructor is protected to force use of copy() method.
        Musketeer_Board(const Musketeer_Board&) = default;

        //! \brief Move constructor is protected to force use of copy() method.
        Musketeer_Board(Musketeer_Board&&) = default;

        void other_move_effects(const Move& move) noexcept override;

    private:
        std::array<std::array<Piece, 8>, 2> gated_pieces{}; // indexed by gated_pieces[Color index][File index]
        std::array<std::vector<std::shared_ptr<const Move>>, 2> gated_pawn_promotions{};

        void pick_and_place_random_gated_pieces() noexcept;
        std::string gate_fen(Piece_Color color) const noexcept;
        void set_unmoved_gate_guardians() noexcept;
        void ascii_draw_above_board(int indentation, int symbol_width) const noexcept override;
        void ascii_draw_below_board(int indentation, int symbol_width) const noexcept override;
        void ascii_draw_gate(Piece_Color color, int indentation, int symbol_width) const noexcept;
        void add_other_moves(std::vector<const Move*>& move_list) noexcept override;
};

#endif // MUSKETEER_BOARD_H
