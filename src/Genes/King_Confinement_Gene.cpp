#include "Genes/King_Confinement_Gene.h"

#include "Game/Board.h"
#include "Game/Color.h"
#include "Pieces/Piece.h"

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
    for(const auto& move : temp.piece_on_square(king_file, king_rank)->get_move_list())
    {
        if(temp.is_legal(king_file, king_rank, move))
        {
            score += 1;
        }
    }

    return double(score)/temp.piece_on_square(king_file, king_rank)->get_move_list().size();
}
