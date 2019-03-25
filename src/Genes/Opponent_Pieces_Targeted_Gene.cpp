#include "Genes/Opponent_Pieces_Targeted_Gene.h"

#include <array>
#include <memory>
#include <string>

#include "Game/Board.h"
#include "Genes/Gene.h"
#include "Genes/Piece_Strength_Gene.h"

//! The Opponent_Pieces_Targeted_Gene constructor requires a Piece_Strength_Gene to reference in score_board().

//! \param piece_strength_gene The source of piece values to weight the importance of various pieces.
Opponent_Pieces_Targeted_Gene::Opponent_Pieces_Targeted_Gene(const Piece_Strength_Gene* piece_strength_gene) :
    piece_strength_source(piece_strength_gene)
{
}

double Opponent_Pieces_Targeted_Gene::score_board(const Board& board, const Board&, size_t) const
{
    double score = 0.0;
    const auto& squares_attacked = board.all_square_indices_attacked();

    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            if(squares_attacked[Board::square_index(file, rank)])
            {
                auto piece = board.piece_on_square(file, rank);
                if(piece)
                {
                    score += piece_strength_source->piece_value(piece);
                }
            }
        }
    }

    return score/piece_strength_source->normalizer();
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
    piece_strength_source = psg;
}
