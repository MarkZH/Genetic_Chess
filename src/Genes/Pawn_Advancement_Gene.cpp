#include "Genes/Pawn_Advancement_Gene.h"

#include <string>

#include "Game/Board.h"
#include "Pieces/Piece.h"
#include "Genes/Piece_Strength_Gene.h"
#include "Utility.h"

Pawn_Advancement_Gene::Pawn_Advancement_Gene(const std::shared_ptr<const Piece_Strength_Gene>& piece_strength_source_in) :
    Gene(1.0),
    piece_strength_source(piece_strength_source_in),
    promoted_pawn_bonus(0.0)
{
}

void Pawn_Advancement_Gene::reset_properties() const
{
    reset_base_properties();
    properties["Promoted Pawn Bonus"] = promoted_pawn_bonus;
}

void Pawn_Advancement_Gene::load_properties()
{
    load_base_properties();
    promoted_pawn_bonus = properties["Promoted Pawn Bonus"];
}

double Pawn_Advancement_Gene::score_board(const Board& board, Color color) const
{
    double score = 0.0;

    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            auto piece = board.view_square(file, rank).piece_on_square();
            if(piece && piece->color() == color && toupper(piece->fen_symbol()) == 'P')
            {
                // 1 point per pawn + 1 point per move towards promotion
                score += std::abs((color == WHITE ? 1 : 8) - rank);
            }
        }
    }

    for(size_t i = (color == WHITE ? 0 : 1); i < board.get_game_record().size(); i += 2)
    {
        if(board.get_game_record()[i].find('=') != std::string::npos)
        {
            auto piece_symbol = toupper(String::split(board.get_game_record()[i], "=")[1][0]); // char
            score += promoted_pawn_bonus*piece_strength_source->piece_value(piece_symbol);
        }
    }

    return score;
}

Pawn_Advancement_Gene* Pawn_Advancement_Gene::duplicate() const
{
    return new Pawn_Advancement_Gene(*this);
}

std::string Pawn_Advancement_Gene::name() const
{
    return "Pawn Advancement Gene";
}

void Pawn_Advancement_Gene::mutate()
{
    Gene::mutate();
    promoted_pawn_bonus += Random::random_normal(1.0);
    promoted_pawn_bonus = std::max(promoted_pawn_bonus, 0.0);
}

void Pawn_Advancement_Gene::reset_piece_strength_gene(const std::shared_ptr<const Piece_Strength_Gene>& psg)
{
    piece_strength_source = psg;
}
