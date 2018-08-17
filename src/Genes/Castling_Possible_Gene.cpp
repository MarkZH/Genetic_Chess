#include "Genes/Castling_Possible_Gene.h"

#include <string>
#include <memory>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Color.h"

#include "Utility.h"

Castling_Possible_Gene::Castling_Possible_Gene() :
    kingside_preference(0.5),
    queenside_preference(0.5)
{
}

void Castling_Possible_Gene::reset_properties() const
{
    Gene::reset_properties();
    properties["Kingside Preference"] = kingside_preference;
    properties["Queenside Preference"] = queenside_preference;
}

void Castling_Possible_Gene::load_properties()
{
    Gene::load_properties();
    kingside_preference = properties["Kingside Preference"];
    queenside_preference = properties["Queenside Preference"];
}

std::unique_ptr<Gene> Castling_Possible_Gene::duplicate() const
{
    return std::make_unique<Castling_Possible_Gene>(*this);
}

std::string Castling_Possible_Gene::name() const
{
    return "Castling Possible Gene";
}

double Castling_Possible_Gene::score_board(const Board& board, const Board&, size_t depth) const
{
    auto normalizing_factor = std::abs(kingside_preference) + std::abs(queenside_preference);

    // check if a castling move lies between here and the actual state of the board
    if( ! board.game_record().empty())
    {
        auto castling_index = board.castling_move_index(board.whose_turn());
        auto last_move_index = board.game_record().size() - 1;

        if(castling_index <= last_move_index) // castling has occurred in the past of this board
        {
            if(last_move_index - castling_index <= depth) // castling has not occured on the actual board
            {
                return (board.game_record()[castling_index]->file_change() > 0 ?
                        kingside_preference : queenside_preference)/normalizing_factor;
            }
            else // castling already happened in past of actual board, no longer relevant
            {
                return 0.0;
            }
        }
    }

    auto king_start_file = 'e';
    auto base_rank = (board.whose_turn() == WHITE ? 1 : 8);
    if(board.piece_has_moved(king_start_file, base_rank))
    {
        return 0.0;
    }

    // King has not moved, check rooks and intervening pieces
    auto score = 0.0;

    for(auto rook_file : {'a', 'h'})
    {
        if( ! board.piece_has_moved(rook_file, base_rank))
        {
            auto preference = (rook_file == 'h' ? kingside_preference : queenside_preference);
            int files_to_clear = std::abs(rook_file - king_start_file) - 1;
            double score_per_clear_square = preference/(files_to_clear + 2);
            score += score_per_clear_square; // score for keeping rook unmoved

            // Add score for clearing pieces between king and rook
            auto first_file = std::min(king_start_file, rook_file) + 1;
            auto last_file = std::max(king_start_file, rook_file);
            for(char file = first_file; file < last_file; ++file)
            {
                if( ! board.piece_on_square(file, base_rank))
                {
                    score += score_per_clear_square;
                }
            }
        }
    }

    return score/normalizing_factor;
}

void Castling_Possible_Gene::gene_specific_mutation()
{
    make_priority_minimum_zero();

    if(Random::coin_flip())
    {
        kingside_preference += Random::random_laplace(0.1);
    }
    else
    {
        queenside_preference += Random::random_laplace(0.1);
    }
}
