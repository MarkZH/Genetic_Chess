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
            return Math::sign(board.castling_direction(perspective) > 0 ?
                              kingside_preference : queenside_preference);
        }
        else // castling already happened in past of actual board, no longer relevant
        {
            return 0.0;
        }
    }

    auto king_square = board.find_king(perspective);
    if(board.piece_has_moved(king_square))
    {
        return 0.0;
    }

    // King has not moved, check rooks and intervening pieces
    auto score = 0.0;

    for(auto rook_file : {'a', 'h'})
    {
        auto preference = (rook_file == 'h' ? kingside_preference : queenside_preference);
        int files_to_clear = std::abs(rook_file - king_square.file()) - 1;

        // Factors to count: 1) Rook not moved
        //                   2,3) squares king crosses are not attacked
        //                   4,5) empty squares between king and rook
        //                   6) extra empty square for queenside castling
        //                   +1) Only get full marks for actually castling.
        double score_per_clear_square = preference/(files_to_clear + 4);

        if( ! board.piece_has_moved({rook_file, king_square.rank()}))
        {
            score += score_per_clear_square; // score for keeping rook unmoved

            // Add score for clearing pieces between king and rook
            auto first_file = std::min(king_square.file(), rook_file) + 1;
            auto last_file = std::max(king_square.file(), rook_file);
            for(char file = first_file; file < last_file; ++file)
            {
                if( ! board.piece_on_square({file, king_square.rank()}))
                {
                    score += score_per_clear_square;

                    if(std::abs(king_square.file() - file) <= 2 && board.safe_for_king({file, king_square.rank()}, perspective))
                    {
                        score += score_per_clear_square;
                    }
                }
            }
        }
    }

    return score;
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
