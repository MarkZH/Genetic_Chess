#include "Genes/Castling_Possible_Gene.h"

#include <string>
#include <memory>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Color.h"

#include "Utility.h"

Castling_Possible_Gene::Castling_Possible_Gene() : kingside_preference(0.5)
{
}

void Castling_Possible_Gene::reset_properties() const
{
    Gene::reset_properties();
    properties["Kingside Preference"] = kingside_preference;
}

void Castling_Possible_Gene::load_properties()
{
    Gene::load_properties();
    kingside_preference = properties["Kingside Preference"];
}

std::unique_ptr<Gene> Castling_Possible_Gene::duplicate() const
{
    return std::make_unique<Castling_Possible_Gene>(*this);
}

std::string Castling_Possible_Gene::name() const
{
    return "Castling Possible Gene";
}

double Castling_Possible_Gene::score_board(const Board& board) const
{
    auto perspective = board.whose_turn();

    // Check game record to see if player already castled
    auto base_rank = (perspective == WHITE ? 1 : 8);
    auto king_start_file = 'e';
    auto kingside_file = 'g';
    auto queenside_file = 'c';

    // Check if king has moved (castling or not)
    if(board.piece_has_moved(king_start_file, base_rank))
    {
        for(auto move : board.get_game_record())
        {
            if(move->start_file() == king_start_file && move->start_rank() == base_rank)
            {
                if(move->end_file() == kingside_file || move->end_file() == queenside_file)
                {
                    // The only way the king can move two files is by castling
                    return 1.0;
                }
                else
                {
                    // King moved but did not castle
                    return 0.0;
                }
            }
        }
    }

    // King has not moved, check rooks and intervening pieces
    auto score = 0.0;
    bool both_rooks_moved = true;

    for(auto rook_file : {'a', 'h'})
    {
        if( ! board.piece_has_moved(rook_file, base_rank))
        {
            both_rooks_moved = false;

            auto preference = (rook_file == 'h' ? kingside_preference : 1.0 - kingside_preference);
            int files_to_clear = std::abs(rook_file - king_start_file) - 1;
            double score_per_clear_square = preference/(files_to_clear + 1);

            // Add score for clearing pieces between king and rook
            auto first_file = std::min(king_start_file, rook_file) + 1;
            auto last_file = std::max(king_start_file, rook_file);
            for(char file = first_file; file < last_file; ++file)
            {
                if( ! board.view_piece_on_square(file, base_rank))
                {
                    score += score_per_clear_square;
                }
            }
        }
    }

    if(both_rooks_moved)
    {
        return 0.0;
    }

    return score;
}

void Castling_Possible_Gene::gene_specific_mutation()
{
    kingside_preference += Random::random_normal(0.01);
}
