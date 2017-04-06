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
    for(size_t i = (perspective == WHITE ? 0 : 1); i < board.get_game_record().size(); i += 2)
    {
        if(board.get_game_record().at(i).front() == 'O') // already castled
        {
            return 1.0;
        }
    }

    auto base_rank = perspective == WHITE ? 1 : 8;
    auto score = 0.0;

    if(board.piece_has_moved('e', base_rank)) // king
    {
        return score;
    }

    if( ! board.piece_has_moved('h', base_rank)) // kingside rook
    {
        score += kingside_preference;
    }

    if( ! board.piece_has_moved('a', base_rank))  // queenside rook
    {
        score += (1.0 - kingside_preference);
    }
    return score;
}

void Castling_Possible_Gene::gene_specific_mutation()
{
    kingside_preference += Random::random_normal(0.05);
}
