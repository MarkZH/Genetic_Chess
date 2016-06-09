#include "Genes/King_Confinement_Gene.h"

#include "Game/Board.h"
#include "Game/Color.h"
#include "Pieces/Piece.h"

#include "Exceptions/Generic_Exception.h"

King_Confinement_Gene::King_Confinement_Gene() : Gene(0.0)
{
}

void King_Confinement_Gene::reset_properties() const
{
    reset_base_properties();
}

void King_Confinement_Gene::load_properties()
{
    load_base_properties();
}

King_Confinement_Gene::~King_Confinement_Gene()
{
}

King_Confinement_Gene* King_Confinement_Gene::duplicate() const
{
    return new King_Confinement_Gene(*this);
}

std::string King_Confinement_Gene::name() const
{
    return "King Confinement Gene";
}

double King_Confinement_Gene::score_board(const Board& board, Color perspective) const
{
    auto temp = board.make_hypothetical();
    temp.set_turn(perspective);

    auto king_location = temp.find_king(perspective);
    char king_file = king_location.first;
    int  king_rank = king_location.second;

    auto score = 0;
    for(const auto& cm : temp.all_legal_moves())
    {
        auto piece = temp.piece_on_square(cm.starting_file, cm.starting_rank);
        if(piece->pgn_symbol() == "K")
        {
            score += 1;
        }
    }

    return score;
}
