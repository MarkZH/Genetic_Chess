#include "Genes/Opponent_Pieces_Targeted_Gene.h"

#include <array>
#include <memory>

#include "Game/Board.h"
#include "Pieces/Piece.h"
#include "Pieces/Piece_Types.h"
#include "Moves/Move.h"
#include "Genes/Gene.h"
#include "Genes/Piece_Strength_Gene.h"

Opponent_Pieces_Targeted_Gene::Opponent_Pieces_Targeted_Gene(const Piece_Strength_Gene* piece_strength_gene) :
    piece_strenth_source(piece_strength_gene)
{
}

double Opponent_Pieces_Targeted_Gene::score_board(const Board& board, const Board&, size_t) const
{
    double score = 0.0;
    auto squares_attacked = board.all_square_indices_attacked();

    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            if(squares_attacked[Board::square_index(file, rank)])
            {
                score += piece_strenth_source->piece_value(board.piece_on_square(file, rank));
            }
        }
    }

    return score;
}

std::unique_ptr<Gene> Opponent_Pieces_Targeted_Gene::duplicate() const
{
    return std::make_unique<Opponent_Pieces_Targeted_Gene>(*this);
}

std::string Opponent_Pieces_Targeted_Gene::name() const
{
    return "Opponent Pieces Targeted Gene";
}

void Opponent_Pieces_Targeted_Gene::reset_piece_strength_gene(const Piece_Strength_Gene* psg)
{
    piece_strenth_source = psg;
}
