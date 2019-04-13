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
    square_queue.fill(square_queue.size()); // initially filled with invalid indices
    const auto& king_square = board.find_king(perspective);
    auto king_index = board.square_index(king_square.file(), king_square.rank());
    square_queue[0] = king_index;
    size_t queue_insertion_point = 1;

    std::array<bool, 64> in_queue{};
    in_queue[king_index] = true;

    double friendly_block_total = 0.0;
    double opponent_block_total = 0.0;
    int free_space_total = 0;

    for(auto square_index : square_queue)
    {
        if(square_index == square_queue.size())
        {
            break;
        }

        auto square = Square{square_index};

        auto attacked_by_other = ! board.safe_for_king(square.file(), square.rank(), perspective);
        auto piece = board.piece_on_square(square.file(), square.rank());
        bool occupied_by_same = piece &&
                                piece->color() == perspective &&
                                piece->type() != KING;

        auto keep_going = square_index == king_index;
        if(occupied_by_same)
        {
            friendly_block_total += friendly_block_score;
        }
        else if(attacked_by_other)
        {
            opponent_block_total += opponent_block_score;
        }
        else
        {
            ++free_space_total;
            keep_going = true;
        }

        // Add surrounding squares to square_queue.
        // always check the squares surrounding the king's current positions, even if
        // it is not safe (i.e., the king is in check).
        if(keep_going)
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
                    if(new_index >= square_queue.size() || rank_diff != new_mod - old_mod)
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
