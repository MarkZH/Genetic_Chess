#include "Genes/King_Confinement_Gene.h"

#include "Game/Board.h"
#include "Game/Color.h"
#include "Pieces/Piece.h"

#include "Exceptions/Generic_Exception.h"

King_Confinement_Gene::King_Confinement_Gene() : Gene(1.0)
{
    reset_properties();
}

void King_Confinement_Gene::reset_properties()
{
    reset_base_properties();
}

King_Confinement_Gene::~King_Confinement_Gene()
{
}

King_Confinement_Gene* King_Confinement_Gene::duplicate() const
{
    auto dupe = new King_Confinement_Gene(*this);
    dupe->reset_properties();
    return dupe;
}

std::string King_Confinement_Gene::name() const
{
    return "King Confinement Gene";
}

double King_Confinement_Gene::score_board(const Board& board, Color perspective) const
{
    auto temp = board.make_hypothetical();
    temp.set_turn(perspective);
    auto score = 0;
    for(const auto& cm : temp.all_legal_moves())
    {
        auto piece = temp.view_square(cm.starting_file, cm.starting_rank).piece_on_square();
        if(piece->pgn_symbol() == "K")
        {
            score += 1;
        }
    }

    return score;
}
