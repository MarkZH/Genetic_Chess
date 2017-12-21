#include "Genes/Opponent_Pieces_Targeted_Gene.h"

#include <array>
#include <memory>

#include "Game/Board.h"
#include "Pieces/Piece.h"
#include "Moves/Move.h"
#include "Genes/Gene.h"
#include "Genes/Piece_Strength_Gene.h"

Opponent_Pieces_Targeted_Gene::Opponent_Pieces_Targeted_Gene(const Piece_Strength_Gene* piece_strength_gene) :
    piece_strenth_source(piece_strength_gene)
{
}

double Opponent_Pieces_Targeted_Gene::score_board(const Board& board) const
{
    double score = 0.0;
    std::array<bool, 64> already_counted{};

    for(const auto& move : board.legal_moves())
    {
        if( ! move->can_capture())
        {
            continue;
        }
        auto end_file = move->end_file();
        auto end_rank = move->end_rank();
        if(move->is_en_passant())
        {
            end_rank -= move->rank_change();
        }
        auto target_piece = board.piece_on_square(end_file, end_rank);
        auto target_index = Board::board_index(end_file, end_rank);
        if(target_piece && ! already_counted[target_index])
        {
            score += piece_strenth_source->piece_value(target_piece);
            already_counted[target_index] = true;
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
