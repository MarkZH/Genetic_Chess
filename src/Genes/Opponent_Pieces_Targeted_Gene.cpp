#include "Genes/Opponent_Pieces_Targeted_Gene.h"

#include <utility>

#include "Game/Board.h"
#include "Game/Piece.h"
#include "Game/Color.h"

#include "Genes/Gene.h"
#include "Genes/Piece_Strength_Gene.h"

Opponent_Pieces_Targeted_Gene::Opponent_Pieces_Targeted_Gene(const Piece_Strength_Gene* const piece_strength_gene) noexcept :
    Clonable_Gene("Opponent Pieces Targeted Gene"),
    piece_strength_source(piece_strength_gene)
{
}

double Opponent_Pieces_Targeted_Gene::score_board(const Board& board, const Piece_Color perspective, size_t) const noexcept
{
    assert(piece_strength_source);
    double score = 0.0;
    const auto values = piece_strength_source->piece_values();

    for(const auto square : Square::all_squares())
    {
        const auto piece = board.piece_on_square(square);
        if(piece && piece.color() != perspective && board.attacked_by(square, perspective))
        {
            score += values[std::to_underlying(piece.type())];
        }
    }

    return score;
}

void Opponent_Pieces_Targeted_Gene::reset_piece_strength_gene(const Piece_Strength_Gene* const psg) noexcept
{
    piece_strength_source = psg;
}
