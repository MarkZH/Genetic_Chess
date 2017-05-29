#include "Genes/Castling_Possible_Gene.h"

#include <string>

#include "Game/Board.h"
#include "Game/Color.h"

#include "Utility.h"

Castling_Possible_Gene::Castling_Possible_Gene() : kingside_preference(0.5)
{
}

Castling_Possible_Gene::~Castling_Possible_Gene()
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

double Castling_Possible_Gene::score_board(const Board& board, Color perspective) const
{
    // Check game record to see if player already castled
    auto base_rank = (perspective == WHITE ? 1 : 8);
    auto king_file = 'e';
    auto starting_king_square = king_file + std::to_string(base_rank);
    auto kingside_move = starting_king_square + 'g' + starting_king_square.back();
    auto queenside_move = starting_king_square + 'c' + starting_king_square.back();
    const auto& game_record = board.get_game_record();
    for(size_t i = (perspective == WHITE ? 0 : 1); i < game_record.size(); i += 2)
    {
        if(game_record[i] == kingside_move || game_record[i] == queenside_move)
        {
            return 1.0;
        }

        if(String::starts_with(game_record[i], starting_king_square))
        {
            break;
        }
    }

    auto score = 0.0;

    if(board.piece_has_moved(king_file, base_rank)) // king moved before castling
    {
        return score;
    }

    // Add score for clearing pieces between king and rook
    for(auto rook_file : {'a', 'h'})
    {
        if( ! board.piece_has_moved(rook_file, base_rank))
        {
            for(char file = std::min(king_file, rook_file) + 1;
                file < std::max(king_file, rook_file);
                ++file)
            {
                if( ! board.view_piece_on_square(file, base_rank))
                {
                    auto preference = (rook_file == 'h' ? kingside_preference : 1.0 - kingside_preference);
                    int files_to_clear = std::abs(rook_file - king_file) - 1;
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
