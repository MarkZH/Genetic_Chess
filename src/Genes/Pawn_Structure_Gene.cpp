#include "Genes/Pawn_Structure_Gene.h"

#include <string>
#include <cmath>

#include "Genes/Gene.h"
#include "Genes/Interpolated_Gene_Value.h"
#include "Game/Board.h"
#include "Game/Square.h"
#include "Game/Piece.h"
#include "Game/Color.h"

#include "Utility/Math.h"
#include "Utility/Random.h"

Pawn_Structure_Gene::Pawn_Structure_Gene() noexcept
{
    normalize_guard_scores();
}

std::string Pawn_Structure_Gene::name() const noexcept
{
    return "Pawn Structure Gene";
}

double Pawn_Structure_Gene::score_board(const Board& board, Piece_Color perspective, size_t) const noexcept
{
    const auto pawn = Piece{perspective, Piece_Type::PAWN};
    const auto guard_direction = perspective == Piece_Color::WHITE ? -1 : 1;

    auto score = 0.0;
    for(const auto square : Square::all_squares())
    {
        if(board.piece_on_square(square) == pawn)
        {
            if(const auto guard0 = square + Square_Difference{1, guard_direction};
               guard0.inside_board() && board.piece_on_square(guard0) == pawn)
            {
                score += guarded_by_pawn.value();
            }
            else if(const auto guard1 = square + Square_Difference{-1, guard_direction};
                    guard1.inside_board() && board.piece_on_square(guard1) == pawn)
            {
                score += guarded_by_pawn.value();
            }
            else if(board.attacked_by(square, perspective))
            {
                score += guarded_by_piece.value();
            }
        }
    }

    return score/8;
}

void Pawn_Structure_Gene::gene_specific_mutation() noexcept
{
    if(Random::coin_flip())
    {
        guarded_by_pawn.mutate();
    }
    else
    {
        guarded_by_piece.mutate();
    }

    normalize_guard_scores();
}

void Pawn_Structure_Gene::adjust_properties(std::map<std::string, std::string>& properties) const noexcept
{
    guarded_by_pawn.write_to_map(properties);
    guarded_by_piece.write_to_map(properties);
}

void Pawn_Structure_Gene::load_gene_properties(const std::map<std::string, std::string>& properties)
{
    guarded_by_pawn.load_from_map(properties);
    guarded_by_piece.load_from_map(properties);
    normalize_guard_scores();
}

void Pawn_Structure_Gene::normalize_guard_scores() noexcept
{
    Math::normalize(guarded_by_pawn.value(), guarded_by_piece.value());
}
