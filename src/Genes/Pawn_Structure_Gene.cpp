#include "Genes/Pawn_Structure_Gene.h"

#include <string>

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

double Pawn_Structure_Gene::score_board(const Board& board, Piece_Color perspective, size_t, double game_progress) const noexcept
{
    const auto guarded_by_pawn = Math::interpolate(opening_guarded_by_pawn, endgame_guarded_by_pawn, game_progress);
    const auto guarded_by_piece = Math::interpolate(opening_guarded_by_piece, endgame_guarded_by_piece, game_progress);

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
                score += guarded_by_pawn;
            }
            else if(const auto guard1 = square + Square_Difference{-1, guard_direction};
                    guard1.inside_board() && board.piece_on_square(guard1) == pawn)
            {
                score += guarded_by_pawn;
            }
            else if( ! board.safe_for_king(square, opposite(perspective)))
            {
                score += guarded_by_piece;
            }
        }
    }

    return score/8;
}

void Pawn_Structure_Gene::gene_specific_mutation() noexcept
{
    switch(Random::random_integer(1, 4))
    {
        case 1:
            opening_guarded_by_pawn += Random::random_laplace(0.03);
            break;
        case 2:
            opening_guarded_by_piece += Random::random_laplace(0.03);
            break;
        case 3:
            endgame_guarded_by_pawn += Random::random_laplace(0.03);
            break;
        case 4:
            endgame_guarded_by_piece += Random::random_laplace(0.03);
            break;
    }

    normalize_guard_scores();
}

void Pawn_Structure_Gene::adjust_properties(std::map<std::string, double>& properties) const noexcept
{
    properties["Guarded By Pawn - Opening"] = opening_guarded_by_pawn;
    properties["Guarded By Piece - Opening"] = opening_guarded_by_piece;

    properties["Guarded By Pawn - Endgame"] = opening_guarded_by_pawn;
    properties["Guarded By Piece - Endgame"] = opening_guarded_by_piece;
}

void Pawn_Structure_Gene::load_gene_properties(const std::map<std::string, double>& properties)
{
    opening_guarded_by_pawn = properties.at("Guarded By Pawn - Opening");
    opening_guarded_by_piece = properties.at("Guarded By Piece - Opening");

    opening_guarded_by_pawn = properties.at("Guarded By Pawn - Endgame");
    opening_guarded_by_piece = properties.at("Guarded By Piece - Endgame");

    normalize_guard_scores();
}

void Pawn_Structure_Gene::normalize_guard_scores() noexcept
{
    Math::normalize(opening_guarded_by_pawn, opening_guarded_by_piece);
    Math::normalize(endgame_guarded_by_pawn, endgame_guarded_by_piece);
}
