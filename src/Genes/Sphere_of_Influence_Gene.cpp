#include "Genes/Sphere_of_Influence_Gene.h"

#include "Game/Board.h"
#include "Pieces/Piece.h"
#include "Exceptions/Generic_Exception.h"
#include "Piece_Strength_Gene.h"
#include "Utility.h"

Sphere_of_Influence_Gene::Sphere_of_Influence_Gene(const std::shared_ptr<const Piece_Strength_Gene>& piece_strength_source_in) :
    Gene(1.0),
    legal_bonus(0.0),
    strength_factor(0.0),
    piece_strength_source(piece_strength_source_in)
{
}

void Sphere_of_Influence_Gene::reset_properties() const
{
    reset_base_properties();
    properties["Legal Bonus"] = legal_bonus;
    properties["Strength Factor"] = strength_factor;
}

void Sphere_of_Influence_Gene::load_properties()
{
    load_base_properties();
    legal_bonus = properties["Legal Bonus"];
    strength_factor = properties["Strength Factor"];
}

Sphere_of_Influence_Gene::~Sphere_of_Influence_Gene()
{
}

Sphere_of_Influence_Gene* Sphere_of_Influence_Gene::duplicate() const
{
    return new Sphere_of_Influence_Gene(*this);
}

std::string Sphere_of_Influence_Gene::name() const
{
    return "Sphere of Influence Gene";
}

// Count all potentially capturing moves as if a given piece and
// the opposing king were the only pieces on the board.
double Sphere_of_Influence_Gene::score_board(const Board& board, Color color) const
{
    // Search for opponent's king
    char king_file = 'a';
    int  king_rank = 1;
    for( ; king_file <= 'h'; ++king_file)
    {
        for( ; king_rank <= 8; ++king_rank)
        {
            auto piece = board.piece_on_square(king_file, king_rank);
            if(piece && piece->pgn_symbol() == "K" && piece->color() == opposite(color))
            {
                break;
            }
        }
    }

    std::map<std::string, double> square_score;
    for(const auto& cm : board.all_moves())
    {
        char final_file = cm.starting_file + cm.move->file_change();
        char final_rank = cm.starting_rank + cm.move->rank_change();

        if( ! Board::inside_board(final_file) || ! Board::inside_board(final_rank))
        {
            continue;
        }

        // Only potentially capturing moves are counted
        if(cm.move->name() == "Pawn Move" ||
           cm.move->name() == "Pawn Double Move" ||
           (String::starts_with(cm.move->name(), "Pawn Promotion") && ! String::contains(cm.move->name(), "Capture")) ||
           String::contains(cm.move->name(), "Castle"))
        {
            continue;
        }

        auto piece_symbol = std::toupper(board.piece_on_square(cm.starting_file, cm.starting_rank)->fen_symbol());
        auto piece_strength = piece_strength_source->piece_value(piece_symbol);

        int distance = std::max(std::abs(final_file - king_file),
                                std::abs(final_rank - king_rank));

        auto legal_multiplier = 1 + (board.is_legal(cm) ? legal_bonus : 0.0);

        // use 1 + piece_strength in case piece_strength is 0
        auto move_score = (8 - distance)*legal_multiplier/(1 + strength_factor*piece_strength);

        std::string square_address;
        square_address.push_back(final_file);
        square_address += std::to_string(final_rank);
        if(move_score > square_score[square_address])
        {
            square_score[square_address] = move_score;
        }
    }

    int score = 0;
    for(const auto& square_value : square_score)
    {
        score += square_value.second;
    }
    return score;
}

void Sphere_of_Influence_Gene::mutate()
{
    Gene::mutate();
    legal_bonus = std::max(legal_bonus + Random::random_normal(1.0), 0.0);
    strength_factor = std::max(strength_factor + Random::random_normal(1.0), 0.0);
}

void Sphere_of_Influence_Gene::reset_piece_strength_gene(const std::shared_ptr<const Piece_Strength_Gene>& psg)
{
    piece_strength_source = psg;
}
