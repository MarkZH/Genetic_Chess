#include "Genes/King_Confinement_Gene.h"

#include <cmath>
#include <array>
#include <memory>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Color.h"
#include "Game/Square.h"
#include "Game/Piece.h"

#include "Utility.h"

King_Confinement_Gene::King_Confinement_Gene() :
    friendly_block_score(0.0),
    enemy_block_score(0.0),
    maximum_distance(0)
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

void King_Confinement_Gene::load_properties()
{
    Gene::load_properties();
    friendly_block_score = properties["Friendly Block Score"];
    enemy_block_score = properties["Enemy Block Score"];
    maximum_distance = properties["Maximum Distance"];
}

void King_Confinement_Gene::reset_properties() const
{
    Gene::reset_properties();
    properties["Friendly Block Score"] = friendly_block_score;
    properties["Enemy Block Score"] = enemy_block_score;
    properties["Maximum Distance"] = maximum_distance;
}

void King_Confinement_Gene::gene_specific_mutation()
{
    make_priority_minimum_zero();
    auto mutation_size = Random::random_laplace(0.5);
    switch(Random::random_integer(1, 3))
    {
        case 1:
            friendly_block_score += mutation_size;
            break;
        case 2:
            enemy_block_score += mutation_size;
            break;
        case 3:
            maximum_distance += Random::random_integer(-1, 1);
            maximum_distance = Math::clamp(maximum_distance, 1, 63);
            break;
        default:
            throw std::runtime_error("Bad random value in King Confinement Gene");
    }
}

double King_Confinement_Gene::score_board(const Board& board, const Board& opposite_board, size_t) const
{
    // A flood-fill-like algorithm to count the squares that are reachable by the
    // king from its current positions with unlimited consecutive moves. The
    // boundaries of this area area squares attacked by the other color or occupied
    // by pieces of the same color.
    //
    // The more moves it takes to reach a square, the less it adds to the score.

    std::vector<Square> square_queue;
    square_queue.reserve(64);
    auto perspective = board.whose_turn();
    const auto& king_square = board.find_king(perspective);
    square_queue.push_back(king_square);

    std::array<int, 64> distance;
    distance.fill(-1); // never-visited value
    distance[Board::square_index(king_square.file, king_square.rank)] = 0;

    auto squares_attacked_by_other = opposite_board.all_square_indices_attacked();

    double friendly_block_total = 0.0;
    double enemy_block_total = 0.0;
    double free_space_total = 0.0;

    for(size_t i = 0; i < square_queue.size(); ++i)
    {
        auto square = square_queue[i];
        auto square_index = Board::square_index(square.file, square.rank);

        auto attacked_by_other = squares_attacked_by_other[square_index];

        auto piece = board.piece_on_square(square.file, square.rank);
        bool occupied_by_same = piece &&
                                piece->color() == perspective &&
                                piece->type() != KING;

        if(occupied_by_same)
        {
            friendly_block_total += friendly_block_score/(1 + distance[square_index]);
        }
        else if(attacked_by_other)
        {
            enemy_block_total += enemy_block_score/(1 + distance[square_index]);
        }
        else
        {
            free_space_total += 1.0/(1 + distance[square_index]);
        }

        auto is_safe = ! occupied_by_same && ! attacked_by_other;

        // Add surrounding squares to square_queue.
        // always check the squares surrounding the king's current positions, even if
        // it is not safe (i.e., the king is in check).
        if((is_safe && distance[square_index] < maximum_distance) || square_queue.size() == 1)
        {
            for(char new_file = square.file - 1; new_file <= square.file + 1; ++new_file)
            {
                if( ! board.inside_board(new_file))
                {
                    continue;
                }

                for(int new_rank = square.rank - 1; new_rank <= square.rank + 1; ++new_rank)
                {
                    if( ! board.inside_board(new_rank))
                    {
                        continue;
                    }

                    auto new_index = Board::square_index(new_file, new_rank);
                    if(distance[new_index] == -1) // never checked
                    {
                        square_queue.push_back(Square{new_file, new_rank});
                        distance[new_index] = distance[square_index] + 1;
                    }
                }
            }
        }
    }

    return (friendly_block_total + enemy_block_total)/free_space_total;
}
