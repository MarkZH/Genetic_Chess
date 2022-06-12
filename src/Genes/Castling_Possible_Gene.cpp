#include "Genes/Castling_Possible_Gene.h"

#include <string>
#include <map>
#include <cmath>

#include "Genes/Gene.h"
#include "Genes/Interpolated_Gene_Value.h"
#include "Game/Board.h"
#include "Game/Color.h"

#include "Utility/Random.h"
#include "Utility/Math.h"

Castling_Possible_Gene::Castling_Possible_Gene() noexcept
{
    normalize_sides();
}

void Castling_Possible_Gene::normalize_sides() noexcept
{
    Math::normalize(kingside_preference.value(), queenside_preference.value());
}

void Castling_Possible_Gene::adjust_properties(std::map<std::string, std::string>& properties) const noexcept
{
    kingside_preference.write_to_map(properties);
    queenside_preference.write_to_map(properties);
}

void Castling_Possible_Gene::load_gene_properties(const std::map<std::string, std::string>& properties)
{
    kingside_preference.load_from_map(properties);
    queenside_preference.load_from_map(properties);
    normalize_sides();
}

std::string Castling_Possible_Gene::name() const noexcept
{
    return "Castling Possible Gene";
}

double Castling_Possible_Gene::score_board(const Board& board, const Piece_Color perspective, const size_t depth) const noexcept
{
    // check if a castling move lies between here and the actual state of the board
    if(board.player_castled(perspective))
    {
        const auto castling_index = board.castling_move_index(perspective);
        const auto first_searched_move_index = board.played_ply_count() - depth;
        if(castling_index >= first_searched_move_index) // castling has not occurred on the actual board
        {
            const auto castling_distance = castling_index - first_searched_move_index + 1;
            return (board.castling_direction(perspective) > 0 ?
                        kingside_preference.value() :
                        queenside_preference.value())/double(castling_distance);
        }
        else
        {
            return 0.0;
        }
    }
    else
    {
        auto score = 0.0;
        const auto king_square = board.find_king(perspective);
        for(const auto direction : {Direction::LEFT, Direction::RIGHT})
        {
            if(board.castle_is_legal(perspective, direction))
            {
                const auto preference = direction == Direction::LEFT ? queenside_preference.value() : kingside_preference.value();
                const auto rook_square = Square{direction == Direction::LEFT ? 'a' : 'h', perspective == Piece_Color::WHITE ? 1 : 8};
                const auto between_squares = Squares_in_a_Line(king_square, rook_square);
                const auto moves_to_go = std::ranges::count_if(between_squares, [&board](const auto square) { return board.piece_on_square(square); });
                score += preference/double(depth + 2*moves_to_go + 1);
            }
        }

        return score;
    }
}

void Castling_Possible_Gene::gene_specific_mutation() noexcept
{
    if(Random::coin_flip())
    {
        kingside_preference.mutate();
    }
    else
    {
        queenside_preference.mutate();
    }

    normalize_sides();
}
