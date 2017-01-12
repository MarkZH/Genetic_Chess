#include "Genes/Castling_Possible_Gene.h"

#include <string>

#include "Game/Board.h"
#include "Game/Color.h"

Castling_Possible_Gene::~Castling_Possible_Gene()
{
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
    auto base_rank = perspective == WHITE ? 1 : 8;
    auto score = 0.0;

    if(board.piece_has_moved('e', base_rank)) // king
    {
        return score;
    }

    if( ! board.piece_has_moved('a', base_rank)) // queenside rook
    {
        score += 0.5;
    }

    if( ! board.piece_has_moved('h', base_rank))  // kingside rook
    {
        score += 0.5;
    }
    return score;
}
