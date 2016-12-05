#include "Genes/Opponent_Pieces_Targeted_Gene.h"

#include "Genes/Piece_Strength_Gene.h"
#include "Game/Board.h"
#include "Pieces/Piece.h"
#include "Utility.h"

Opponent_Pieces_Targeted_Gene::Opponent_Pieces_Targeted_Gene(const std::shared_ptr<const Piece_Strength_Gene>& piece_strength_gene) :
    Gene(0.0),
    piece_strenth_source(piece_strength_gene)
{
}

Opponent_Pieces_Targeted_Gene::~Opponent_Pieces_Targeted_Gene()
{
}

double Opponent_Pieces_Targeted_Gene::score_board(const Board& board, Color perspective) const
{
    double score = 0.0;
    std::map<std::shared_ptr<const Piece>, bool> already_counted;

    auto hypothetical = board.make_hypothetical();
    hypothetical.set_turn(perspective);

    for(const auto& complete_move : hypothetical.all_legal_moves())
    {
        auto start_file = complete_move.starting_file;
        auto start_rank = complete_move.starting_rank;
        auto move = complete_move.move;
        auto end_file = start_file + move->file_change();
        auto end_rank = start_rank + move->rank_change();
        auto attacking_piece = hypothetical.piece_on_square(start_file, start_rank);
        auto target_piece = hypothetical.piece_on_square(end_file, end_rank);
        if(target_piece && ! already_counted[target_piece])
        {
            score += piece_strenth_source->piece_value(target_piece);
            already_counted[target_piece] = true;
        }
        else if(hypothetical.is_en_passant_targetable(end_file, end_rank) && attacking_piece->pgn_symbol().empty())
        {
            // Pawn attacking another pawn en passant
            score += piece_strenth_source->piece_value('P');
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

void Opponent_Pieces_Targeted_Gene::reset_piece_strength_gene(const std::shared_ptr<const Piece_Strength_Gene>& psg)
{
    piece_strenth_source = psg;
}
