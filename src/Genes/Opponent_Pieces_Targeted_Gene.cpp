#include "Genes/Opponent_Pieces_Targeted_Gene.h"

#include "Game/Board.h"
#include "Pieces/Piece.h"
#include "Moves/Move.h"
#include "Utility.h"
#include "Genes/Gene.h"
#include "Genes/Piece_Strength_Gene.h"

Opponent_Pieces_Targeted_Gene::Opponent_Pieces_Targeted_Gene(const Piece_Strength_Gene* piece_strength_gene) :
    piece_strenth_source(piece_strength_gene)
{
}

double Opponent_Pieces_Targeted_Gene::score_board(const Board& board) const
{
    double score = 0.0;
    std::map<const Piece*, bool> already_counted;

    for(const auto& move : board.legal_moves())
    {
        if( ! move->can_capture())
        {
            continue;
        }
        auto end_file = move->end_file();
        auto end_rank = move->end_rank();
        if(move->name().front() == 'E') // En passant capture
        {
            end_rank += (board.whose_turn() == WHITE ? -1 : 1);
        }
        auto target_piece = board.view_piece_on_square(end_file, end_rank);
        if(target_piece && ! already_counted[target_piece])
        {
            score += piece_strenth_source->piece_value(target_piece);
            already_counted[target_piece] = true;
        }
    }

    return score;
}

Opponent_Pieces_Targeted_Gene* Opponent_Pieces_Targeted_Gene::duplicate() const
{
    return new Opponent_Pieces_Targeted_Gene(*this);
}

std::string Opponent_Pieces_Targeted_Gene::name() const
{
    return "Opponent Pieces Targeted Gene";
}

void Opponent_Pieces_Targeted_Gene::reset_piece_strength_gene(const Piece_Strength_Gene* psg)
{
    piece_strenth_source = psg;
}
