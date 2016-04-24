#include "Genes/Opponent_Pieces_Targeted_Gene.h"

#include "Genes/Piece_Strength_Gene.h"
#include "Game/Board.h"
#include "Pieces/Piece.h"

Opponent_Pieces_Targeted_Gene::Opponent_Pieces_Targeted_Gene(const std::shared_ptr<const Piece_Strength_Gene>& piece_strength_gene) :
    Gene(0.0),
    piece_strenth_source(piece_strength_gene)
{
    reset_properties();
}

void Opponent_Pieces_Targeted_Gene::reset_properties()
{
    reset_base_properties();
}

Opponent_Pieces_Targeted_Gene::~Opponent_Pieces_Targeted_Gene()
{
}

double Opponent_Pieces_Targeted_Gene::score_board(const Board& board, Color color) const
{
    double score = 0.0;
    std::map<std::shared_ptr<const Piece>, bool> already_counted;

    auto hypothetical = board.make_hypothetical();
    hypothetical.set_turn(color);

    for(const auto& complete_move : hypothetical.all_legal_moves())
    {
        auto start_file = complete_move.starting_file;
        auto start_rank = complete_move.starting_rank;
        auto move = complete_move.move;
        auto origin_square = hypothetical.view_square(start_file,
                                                      start_rank);
        auto attacking_piece = origin_square.piece_on_square();
        auto target_square = hypothetical.view_square(start_file + move->file_change(),
                                                      start_rank + move->rank_change());
        auto target_piece = target_square.piece_on_square();
        if(target_piece && ! already_counted[target_piece])
        {
            score += piece_strenth_source->piece_value(target_piece);
            already_counted[target_piece] = true;
        }
        else if(target_square.is_en_passant_targetable() && attacking_piece->pgn_symbol().empty())
        {
            // Pawn attacking another pawn en passant
            score += piece_strenth_source->piece_value('P');
        }
    }

    return score;
}

Opponent_Pieces_Targeted_Gene* Opponent_Pieces_Targeted_Gene::duplicate() const
{
    auto dupe = new Opponent_Pieces_Targeted_Gene(*this);
    dupe->reset_properties();
    return dupe;
}

std::string Opponent_Pieces_Targeted_Gene::name() const
{
    return "Opponent Pieces Targeted";
}

void Opponent_Pieces_Targeted_Gene::reset_piece_strength_gene(const std::shared_ptr<const Piece_Strength_Gene>& psg)
{
    piece_strenth_source = psg;
}
