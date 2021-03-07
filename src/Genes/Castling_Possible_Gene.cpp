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
    properties["Unmoved Rook Bonus - Opening"] = opening_rook_unmoved_score;
    properties["Unmoved Rook Bonus - Endgame"] = endgame_rook_unmoved_score;
}

void Castling_Possible_Gene::load_gene_properties(const std::map<std::string, double>& properties)
{
    opening_kingside_preference = properties.at("Kingside Preference - Opening");
    opening_queenside_preference = properties.at("Queenside Preference - Opening");
    endgame_kingside_preference = properties.at("Kingside Preference - Endgame");
    endgame_queenside_preference = properties.at("Queenside Preference - Endgame");
    opening_rook_unmoved_score = properties.at("Unmoved Rook Bonus - Opening");
    endgame_rook_unmoved_score = properties.at("Unmoved Rook Bonus - Endgame");
    normalize_sides();
}

std::string Castling_Possible_Gene::name() const noexcept
{
    return "Castling Possible Gene";
}

double Castling_Possible_Gene::score_board(const Board& board, Piece_Color perspective, size_t depth, double game_progress) const noexcept
{
    auto kingside_preference = Math::interpolate(opening_kingside_preference, endgame_kingside_preference, game_progress);
    auto queenside_preference = Math::interpolate(opening_queenside_preference, endgame_queenside_preference, game_progress);

    // check if a castling move lies between here and the actual state of the board
    if(board.player_castled(perspective))
    {
        auto castling_index = board.castling_move_index(perspective);
        auto first_searched_move_index = board.game_length() - depth;
        if(castling_index >= first_searched_move_index) // castling has not occurred on the actual board
        {
            auto castling_distance = castling_index - first_searched_move_index + 1;
            return (board.castling_direction(perspective) > 0 ? kingside_preference : queenside_preference)/castling_distance;
        }
    }

    if( ! board.piece_has_moved(board.find_king(perspective)))
    {
        auto score = 0.0;
        auto base_rank = perspective == Piece_Color::WHITE ? 1 : 8;
        auto unmoved_rook_bonus = Math::interpolate(opening_rook_unmoved_score,
                                                    endgame_rook_unmoved_score,
                                                    game_progress);
        if( ! board.piece_has_moved({'a', base_rank}))
        {
            score += queenside_preference;
        }

        if( ! board.piece_has_moved({'h', base_rank}))
        {
            score += kingside_preference;
        }

        return score*unmoved_rook_bonus;
    }

    return 0.0;
}

void Castling_Possible_Gene::gene_specific_mutation() noexcept
{
    switch(Random::random_integer(1, 6))
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
        case 5:
            opening_rook_unmoved_score += Random::random_laplace(0.001);
            break;
        case 6:
            endgame_rook_unmoved_score += Random::random_laplace(0.001);
            break;
        default:
            assert(false);
    }

    normalize_sides();
}
