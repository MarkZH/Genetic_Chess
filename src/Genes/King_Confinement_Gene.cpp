#include "Genes/King_Confinement_Gene.h"

#include <cmath>
#include <array>
#include <algorithm>
#include <cassert>
#include <memory>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Color.h"
#include "Game/Square.h"
#include "Pieces/Piece.h"

King_Confinement_Gene::King_Confinement_Gene() : maximum_score(0.0)
{
    // Maximum score comes from a king in the middle of an empty board
    auto king_square = Square{'e', 4};

    for(auto file = 'a'; file <= 'h'; ++file)
    {
        for(auto rank = 1; rank <= 8; ++rank)
        {
            auto distance = king_distance(king_square, {file, rank});
            maximum_score += 1.0/(distance + 1.0);
        }
    }
}

std::unique_ptr<Gene> King_Confinement_Gene::duplicate() const
{
    return std::make_unique<King_Confinement_Gene>(*this);
}

std::string King_Confinement_Gene::name() const
{
    return "King Confinement Gene";
}

double King_Confinement_Gene::score_board(const Board& board) const
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
    auto king_square = board.find_king(perspective);
    square_queue.push_back(king_square);

    std::array<int, 64> distance{};
    assert(std::all_of(distance.begin(), distance.end(), [](auto x){ return x == 0; }));

    distance[Board::board_index(king_square.file, king_square.rank)] = 1;

    double score = 0.0;

    for(size_t i = 0; i < square_queue.size(); ++i)
    {
        auto square = square_queue[i];
        auto square_index = Board::board_index(square.file, square.rank);

        bool attacked_by_other = ! board.safe_for_king(square.file,
                                                       square.rank,
                                                       perspective);

        auto piece = board.piece_on_square(square.file, square.rank);
        bool occupied_by_same = piece &&
                                piece->color() == perspective &&
                                ! piece->is_king();

        auto is_safe = ! attacked_by_other && ! occupied_by_same;
        if(is_safe)
        {
            score += 1.0/distance[square_index];
        }

        // Add surrounding squares to square_queue.
        // always check the squares surrounding the king's current positions, even if
        // it is not safe (i.e., the king is in check).
        if(is_safe || square_queue.size() == 1)
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

                    auto new_square = Square{new_file, new_rank};
                    auto new_index = Board::board_index(new_file, new_rank);
                    if(distance[new_index] == 0) // never checked
                    {
                        square_queue.push_back(new_square);
                        distance[new_index] = distance[square_index] + 1;
                    }
                }
            }
        }
    }

    return score/maximum_score; // normalized so max is 1
}
