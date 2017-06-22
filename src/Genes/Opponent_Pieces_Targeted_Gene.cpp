#include "Genes/Opponent_Pieces_Targeted_Gene.h"

#include "Game/Board.h"
#include "Pieces/Piece.h"
#include "Moves/Complete_Move.h"
#include "Utility.h"
#include "Genes/Gene.h"
#include "Genes/Piece_Strength_Gene.h"

Opponent_Pieces_Targeted_Gene::Opponent_Pieces_Targeted_Gene(const Piece_Strength_Gene* piece_strength_gene) :
    piece_strenth_source(piece_strength_gene)
{
}

double Opponent_Pieces_Targeted_Gene::score_board(const Board& board, Color perspective) const
{
    double score = 0.0;
    std::map<const Piece*, bool> already_counted;

    auto hypothetical = board;
    hypothetical.set_turn(perspective);

    for(const auto& complete_move : hypothetical.legal_moves())
    {
        if( ! complete_move.can_capture())
        {
            continue;
        }
        auto end_file = complete_move.end_file();
        auto end_rank = complete_move.end_rank();
        if(complete_move.name().front() == 'E') // En passant capture
        {
            end_rank += (hypothetical.whose_turn() == WHITE ? -1 : 1);
        }
        auto target_piece = hypothetical.view_piece_on_square(end_file, end_rank);
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
