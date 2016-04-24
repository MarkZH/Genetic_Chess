#include "Genes/Pawn_Advancement_Gene.h"

#include "Game/Board.h"
#include "Pieces/Piece.h"

Pawn_Advancement_Gene::Pawn_Advancement_Gene() : Gene(1.0)
{
    reset_properties();
}

void Pawn_Advancement_Gene::reset_properties()
{
    reset_base_properties();
}

double Pawn_Advancement_Gene::score_board(const Board& board, Color color) const
{
    double score = 0;

    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            auto piece = board.view_square(file, rank).piece_on_square();
            if(piece && piece->color() == color && toupper(piece->fen_symbol()) == 'P')
            {
                // 1 point per pawn + 1 point per move towards promotion
                score += std::abs((color == WHITE ? 1 : 8) - rank);
            }
        }
    }

    return score;
}

Pawn_Advancement_Gene* Pawn_Advancement_Gene::duplicate() const
{
    auto dupe = new Pawn_Advancement_Gene(*this);
    dupe->reset_properties();
    return dupe;
}

std::string Pawn_Advancement_Gene::name() const
{
    return "Pawn Advancement Gene";
}
