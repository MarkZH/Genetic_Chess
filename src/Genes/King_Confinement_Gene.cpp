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

#include "Utility.h"

King_Confinement_Gene::King_Confinement_Gene() :
    friendly_block_score(0.0),
    enemy_block_score(0.0)
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
}

void King_Confinement_Gene::reset_properties() const
{
    Gene::reset_properties();
    properties["Friendly Block Score"] = friendly_block_score;
    properties["Enemy Block Score"] = enemy_block_score;
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
            enemy_block_score += mutation_size;
            break;
        default:
            throw std::logic_error("Bad random value in King Confinement Gene");
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

    std::array<int, 64> in_queue;
    in_queue.fill(false);
    in_queue[Board::square_index(king_square.file(), king_square.rank())] = true;

    auto squares_safe_for_king = opposite_board.squares_safe_for_king();

    double friendly_block_total = 0.0;
    double enemy_block_total = 0.0;
    int free_space_total = 0;

    for(size_t i = 0; i < square_queue.size(); ++i)
    {
        auto square = square_queue[i];
        auto square_index = Board::square_index(square.file(), square.rank());

        auto attacked_by_other = ! squares_safe_for_king[square_index];
        auto piece = board.piece_on_square(square.file(), square.rank());
        bool occupied_by_same = piece &&
                                piece->color() == perspective &&
                                piece->type() != KING;

        if(occupied_by_same)
        {
            friendly_block_total += friendly_block_score;
        }
        else if(attacked_by_other)
        {
            enemy_block_total += enemy_block_score;
        }
        else
        {
            ++free_space_total;
        }

        auto keep_going = (square == king_square) || (! occupied_by_same && ! attacked_by_other);

        // Add surrounding squares to square_queue.
        // always check the squares surrounding the king's current positions, even if
        // it is not safe (i.e., the king is in check).
        if(keep_going)
        {
            auto left_file = std::max('a', char(square.file() - 1));
            auto right_file = std::min('h', char(square.file() + 1));
            auto low_rank = std::max(1, square.rank() - 1);
            auto high_rank = std::min(8, square.rank() + 1);
            for(char new_file = left_file; new_file <= right_file; ++new_file)
            {
                for(int new_rank = low_rank; new_rank <= high_rank; ++new_rank)
                {
                    auto new_index = Board::square_index(new_file, new_rank);
                    if( ! in_queue[new_index])
                    {
                        square_queue.emplace_back(new_file, new_rank);
                        in_queue[new_index] = true;
                    }
                }
            }
        }
    }

    auto normalizer = std::abs(friendly_block_score) + std::abs(enemy_block_score);
    return ((friendly_block_total + enemy_block_total)/free_space_total)/normalizer;
}
