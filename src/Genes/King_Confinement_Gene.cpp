#include "Genes/King_Confinement_Gene.h"

#include <cmath>
#include <array>
#include <memory>
#include <map>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Color.h"
#include "Game/Square.h"
#include "Game/Piece.h"

#include "Utility/Random.h"

King_Confinement_Gene::King_Confinement_Gene() :
    Gene(true),
    friendly_block_score(0.0),
    opponent_block_score(0.0)
{
}

std::unique_ptr<Gene> King_Confinement_Gene::duplicate() const
{
    return std::make_unique<King_Confinement_Gene>(*this);
}

std::string King_Confinement_Gene::name() const
{
    return "King Confinement Gene";
}

void King_Confinement_Gene::load_properties(const std::map<std::string, double>& properties)
{
    Gene::load_properties(properties);
    friendly_block_score = properties.at("Friendly Block Score");
    opponent_block_score = properties.at("Opponent Block Score");
}

std::map<std::string, double> King_Confinement_Gene::list_properties() const
{
    auto properties = Gene::list_properties();
    properties["Friendly Block Score"] = friendly_block_score;
    properties["Opponent Block Score"] = opponent_block_score;
    return properties;
}

void King_Confinement_Gene::gene_specific_mutation()
{
    auto mutation_size = Random::random_laplace(2.0);
    switch(Random::random_integer(1, 2))
    {
        case 1:
            friendly_block_score += mutation_size;
            break;
        case 2:
            opponent_block_score += mutation_size;
            break;
        default:
            throw std::logic_error("Bad random value in King Confinement Gene");
    }
}

double King_Confinement_Gene::score_board(const Board& board, Color perspective, size_t) const
{
    std::array<Square, 64> square_queue{}; // list of indices to check
    size_t queue_insertion_point = 0; // where to insert the index of the next square to check
    std::array<bool, 64> in_queue{}; // track which indices have or will have been checked

    const auto& king_square = board.find_king(perspective);

    square_queue[queue_insertion_point++] = king_square;
    in_queue[king_square.index()] = true;

    double friendly_block_total = 0.0;
    double opponent_block_total = 0.0;
    int free_space_total = 0;

    for(auto square : square_queue)
    {
        if( ! square.is_set())
        {
            break;
        }

        // Always check the squares surrounding the king's current positions, even if
        // it is not safe (i.e., the king is in check).
        auto add_surrounding_squares = square == king_square;

        auto piece = board.piece_on_square(square);

        if(piece && piece.color() == perspective && piece.type() != KING) // Square is blocked by a friendly piece
        {
            friendly_block_total += friendly_block_score;
        }
        else if( ! board.safe_for_king(square, perspective)) // Square is attacked by an opposing piece
        {
            opponent_block_total += opponent_block_score;
        }
        else
        {
            ++free_space_total;
            add_surrounding_squares = true;
        }

        // Add surrounding squares to square_queue.
        if(add_surrounding_squares)
        {
            for(auto file_step = -1; file_step <= 1; ++file_step)
            {
                for(auto rank_step = -1; rank_step <= 1; ++rank_step)
                {
                    auto new_square = square + Square_Difference{file_step, rank_step};
                    if(new_square.inside_board() && ! in_queue[new_square.index()])
                    {
                        square_queue[queue_insertion_point++] = new_square;
                        in_queue[new_square.index()] = true;
                    }
                }
            }
        }
    }

    auto normalizer = std::abs(friendly_block_score) + std::abs(opponent_block_score);
    return (friendly_block_total + opponent_block_total)/(1 + free_space_total)/normalizer;
}
