#include "Genes/King_Confinement_Gene.h"

#include <cmath>
#include <array>
#include <memory>
#include <map>
#include <algorithm>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Color.h"
#include "Game/Square.h"
#include "Game/Piece.h"

#include "Utility/Random.h"
#include "Utility/Math.h"

King_Confinement_Gene::King_Confinement_Gene() :
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
    make_priority_minimum_zero();
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
    // A flood-fill-like algorithm to count the squares that are reachable by the
    // king from its current positions with unlimited consecutive moves. The
    // boundaries of this area area squares attacked by the other color or occupied
    // by pieces of the same color.

    std::array<size_t, 64> square_queue; // list of indices to check
    const size_t invalid_index = 64;
    square_queue.fill(invalid_index);
    size_t queue_insertion_point = 0; // where to insert the index of the next square to check
    std::array<bool, 64> in_queue{}; // track which indices have or will have been checked

    const auto& king_square = board.find_king(perspective);
    const auto king_index = board.square_index(king_square.file(), king_square.rank());

    square_queue[queue_insertion_point++] = king_index;
    in_queue[king_index] = true;

    double friendly_block_total = 0.0;
    double opponent_block_total = 0.0;
    int free_space_total = 0;

    for(auto square_index : square_queue)
    {
        if(square_index == invalid_index)
        {
            break;
        }

        // Always check the squares surrounding the king's current positions, even if
        // it is not safe (i.e., the king is in check).
        auto add_surrounding_squares = square_index == king_index;

        auto square = Square{square_index};
        auto piece = board.piece_on_square(square.file(), square.rank());

        if(piece && piece->color() == perspective && piece->type() != KING) // Square is blocked by a friendly piece
        {
            friendly_block_total += friendly_block_score;
        }
        else if( ! board.safe_for_king(square.file(), square.rank(), perspective)) // Square is attacked by an opposing piece
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
            // The index difference between squares on the same rank but adjacent files is 8.
            for(int file_diff = -8; file_diff <= 8; file_diff += 8)
            {
                auto new_base_index = square_index + file_diff;

                // The index difference between squares on the same file but different ranks is 1.
                for(int rank_diff = -1; rank_diff <= 1; ++rank_diff)
                {
                    int old_mod = new_base_index%8;
                    auto new_index = new_base_index + rank_diff;
                    int new_mod = new_index%8;

                    // Make sure new_index is on board and the rank change didn't
                    // result in an index on a different file.
                    if(new_index >= invalid_index || rank_diff != new_mod - old_mod)
                    {
                        continue;
                    }

                    if( ! in_queue[new_index])
                    {
                        square_queue[queue_insertion_point++] = new_index;
                        in_queue[new_index] = true;
                    }
                }
            }
        }
    }

    auto normalizer = std::abs(friendly_block_score) + std::abs(opponent_block_score);
    return ((friendly_block_total + opponent_block_total)/free_space_total)/normalizer;
}
