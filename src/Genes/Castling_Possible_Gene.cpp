#include "Genes/Castling_Possible_Gene.h"

#include <string>
#include <map>

#include "Genes/Gene.h"
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
    Math::normalize(opening_kingside_preference, opening_queenside_preference);
    Math::normalize(endgame_kingside_preference, endgame_queenside_preference);
}

void Castling_Possible_Gene::adjust_properties(std::map<std::string, double>& properties) const noexcept
{
    properties["Kingside Preference - Opening"] = opening_kingside_preference;
    properties["Queenside Preference - Opening"] = opening_queenside_preference;
    properties["Kingside Preference - Endgame"] = endgame_kingside_preference;
    properties["Queenside Preference - Endgame"] = endgame_queenside_preference;
}

void Castling_Possible_Gene::load_gene_properties(const std::map<std::string, double>& properties)
{
    opening_kingside_preference = properties.at("Kingside Preference - Opening");
    opening_queenside_preference = properties.at("Queenside Preference - Opening");
    endgame_kingside_preference = properties.at("Kingside Preference - Endgame");
    endgame_queenside_preference = properties.at("Queenside Preference - Endgame");
    normalize_sides();
}

std::string Castling_Possible_Gene::name() const noexcept
{
    return "Castling Possible Gene";
}

double Castling_Possible_Gene::score_board(const Board& board, const Piece_Color perspective, const size_t depth, const double game_progress) const noexcept
{
    const auto kingside_preference = Math::interpolate(opening_kingside_preference, endgame_kingside_preference, game_progress);
    const auto queenside_preference = Math::interpolate(opening_queenside_preference, endgame_queenside_preference, game_progress);

    // check if a castling move lies between here and the actual state of the board
    if(board.player_castled(perspective))
    {
        const auto castling_index = board.castling_move_index(perspective);
        const auto first_searched_move_index = board.played_ply_count() - depth;
        if(castling_index >= first_searched_move_index) // castling has not occurred on the actual board
        {
            const auto castling_distance = castling_index - first_searched_move_index + 1;
            return (board.castling_direction(perspective) > 0 ? kingside_preference : queenside_preference)/double(castling_distance);
        }
        else
        {
            return 0.0;
        }
    }
    else if(const auto king_square = board.find_king(perspective);
            ! board.piece_has_moved(king_square))
    {
        auto score = 0.0;
        const auto base_rank = king_square.rank();
        for(const auto file : {'a', 'h'})
        {
            const auto rook_square = Square{file, base_rank};
            if(!board.piece_has_moved(rook_square))
            {
                const auto preference = file == 'a' ? queenside_preference : kingside_preference;
                const auto between_squares = Squares_in_a_Line(king_square, rook_square);
                const auto moves_to_go = std::count_if(between_squares.begin(),
                                                       between_squares.end(),
                                                       [&board](const auto square)
                                                       {
                                                           return board.piece_on_square(square);
                                                       });
                score += preference/double(depth + 2*moves_to_go + 1);
            }
        }

        return score;
    }
    else
    {
        return 0.0;
    }
}

void Castling_Possible_Gene::gene_specific_mutation() noexcept
{
    switch(Random::random_integer(1, 4))
    {
        case 1:
            opening_kingside_preference += Random::random_laplace(0.03);
            break;
        case 2:
            endgame_kingside_preference += Random::random_laplace(0.03);
            break;
        case 3:
            opening_queenside_preference += Random::random_laplace(0.03);
            break;
        case 4:
            endgame_queenside_preference += Random::random_laplace(0.03);
            break;
        default:
            assert(false);
    }

    normalize_sides();
}
