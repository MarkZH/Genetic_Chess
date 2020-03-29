#include "Game/Musketeer_Board.h"

#include <string>
#include <array>

#include "Game/Piece.h"
#include "Moves/Move.h"
#include "Utility/String.h"

namespace
{
    std::string extract_standard_fen(const std::string& original_fen)
    {
        return std::string();
    }
}

Musketeer_Board::Musketeer_Board(const std::string& input_fen) : Board(extract_standard_fen(input_fen))
{
    // interpret other fields
}

void Musketeer_Board::other_move_effects(const Move& move) noexcept
{
    auto base_rank = whose_turn() == Piece_Color::WHITE ? 1 : 8;
    if(move.start().rank() == base_rank)
    {
        auto& gated_piece = gated_pieces[static_cast<int>(whose_turn())][move.start().file() - 'a'];
        if(gated_piece)
        {
            place_piece(gated_piece, move.start());
            gated_piece = {};
        }
    }
}
