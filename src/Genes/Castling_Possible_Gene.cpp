#include "Genes/Castling_Possible_Gene.h"

#include <string>
#include <memory>
#include <map>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Color.h"
#include "Moves/Move.h"

#include "Utility/Random.h"
#include "Utility/Math.h"

Castling_Possible_Gene::Castling_Possible_Gene() :
    Gene(true),
    kingside_preference(0.0),
    queenside_preference(0.0)
{
}

std::map<std::string, double> Castling_Possible_Gene::list_properties() const
{
    auto properties = Gene::list_properties();
    properties["Kingside Preference"] = kingside_preference;
    properties["Queenside Preference"] = queenside_preference;
    return properties;
}

void Castling_Possible_Gene::load_properties(const std::map<std::string, double>& properties)
{
    Gene::load_properties(properties);
    kingside_preference = properties.at("Kingside Preference");
    queenside_preference = properties.at("Queenside Preference");
}

std::unique_ptr<Gene> Castling_Possible_Gene::duplicate() const
{
    return std::make_unique<Castling_Possible_Gene>(*this);
}

std::string Castling_Possible_Gene::name() const
{
    return "Castling Possible Gene";
}

double Castling_Possible_Gene::score_board(const Board& board, Color perspective, size_t prior_real_moves) const
{
    // check if a castling move lies between here and the actual state of the board
    if( ! board.game_record().empty())
    {
        auto castling_index = board.castling_move_index(perspective);
        auto last_move_index = board.game_record().size() - 1;

        if(castling_index <= last_move_index) // castling has occurred in the past of this board
        {
            if(castling_index >= prior_real_moves) // castling has not occurred on the actual board
            {
                return Math::sign(board.game_record()[castling_index]->file_change() > 0 ?
                                  kingside_preference : queenside_preference);
            }
            else // castling already happened in past of actual board, no longer relevant
            {
                return 0.0;
            }
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

    auto normalizing_factor = std::abs(kingside_preference) + std::abs(queenside_preference);
    return score/normalizing_factor;
}

void Castling_Possible_Gene::gene_specific_mutation()
{
    if(Random::coin_flip())
    {
        kingside_preference += Random::random_laplace(0.1);
    }
    else
    {
        queenside_preference += Random::random_laplace(0.1);
    }
}
