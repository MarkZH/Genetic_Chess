#include "Genes/Pawn_Islands_Gene.h"

#include <memory>
#include <string>

#include "Game/Board.h"
#include "Pieces/Piece.h"

#include "Genes/Gene.h"

double Pawn_Islands_Gene::score_board(const Board& board, const Board&) const
{
    double score = 0.0;
    bool on_island = false;

    for(char file = 'a'; file <= 'h'; ++file)
    {
        bool own_pawn_found = false;
        for(int rank = 1; rank <= 8; ++rank)
        {
            auto piece = board.piece_on_square(file, rank);
            if(piece && piece->type() == PAWN && piece->color() == board.whose_turn())
            {
                own_pawn_found = true;
                if( ! on_island)
                {
                    on_island = true;
                    ++score;
                }
                break; // move to next file
            }
        }

        on_island = own_pawn_found;
    }

    return -score; // negative since, presumably, pawn islands are bad;
                   // priority can still evolve to be negative
}

std::unique_ptr<Gene> Pawn_Islands_Gene::duplicate() const
{
    return std::make_unique<Pawn_Islands_Gene>(*this);
}

std::string Pawn_Islands_Gene::name() const
{
    return "Pawn Islands Gene";
}
