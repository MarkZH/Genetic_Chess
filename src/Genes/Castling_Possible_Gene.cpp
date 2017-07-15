#include "Genes/Castling_Possible_Gene.h"

#include <string>

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

Castling_Possible_Gene* Castling_Possible_Gene::duplicate() const
{
    return new Castling_Possible_Gene(*this);
}

std::string Castling_Possible_Gene::name() const
{
    return "Castling Possible Gene";
}

double Castling_Possible_Gene::score_board(const Board& board) const
{
    // Check game record to see if player already castled
    auto perspective = board.whose_turn();
    auto base_rank = (perspective == WHITE ? 1 : 8);
    auto king_file = 'e';
    auto starting_king_square = king_file + std::to_string(base_rank);
    auto kingside_move = starting_king_square + 'g' + starting_king_square.back();
    auto queenside_move = starting_king_square + 'c' + starting_king_square.back();
    for(size_t i = (perspective == WHITE ? 0 : 1); i < board.get_game_record().size(); i += 2)
    {
        // If already castled, return full score.
        auto move_text = board.get_game_record()[i].coordinate_move();
        if(move_text == kingside_move ||
           move_text == queenside_move)
        {
            return 1.0;
        }

        // If king moved before castling, return 0.
        if(String::starts_with(move_text, starting_king_square))
        {
            return 0.0;
        }
    }

    auto score = 0.0;

    for(auto rook_file : {'a', 'h'})
    {
        if( ! board.piece_has_moved(rook_file, base_rank))
        {
            auto preference = (rook_file == 'h' ? kingside_preference : 1.0 - kingside_preference);
            int files_to_clear = std::abs(rook_file - king_file) - 1;

            // Add score for clearing pieces between king and rook
            for(char file = std::min(king_file, rook_file) + 1;
                file < std::max(king_file, rook_file);
                ++file)
            {
                if( ! board.view_piece_on_square(file, base_rank))
                {
                    score += preference/(files_to_clear + 1);
                }
            }
        }
    }

    return score;
}

void Castling_Possible_Gene::gene_specific_mutation()
{
    kingside_preference += Random::random_normal(0.01);
}
