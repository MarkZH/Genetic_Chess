#include "Genes/King_Confinement_Gene.h"

#include <cmath>

#include "Game/Board.h"
#include "Game/Color.h"
#include "Pieces/Piece.h"

King_Confinement_Gene::King_Confinement_Gene() : maximum_score(0)
{
    // Maximum score comes from a king in the middle of an empty board
    auto king_file = 'e';
    auto king_rank = 4;

    for(auto file = 'a'; file <= 'h'; ++file)
    {
        for(auto rank = 1; rank <= 8; ++rank)
        {
            auto distance = std::max(std::abs(king_file - file), std::abs(king_rank - rank));
            maximum_score += 1.0/(distance + 1.0);
        }
    }
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
    // A flood-fill-like algorithm to count the squares that are reachable by the
    // king from its current positions with unlimited consecutive moves. The
    // boundaries of this area area squares attacked by the other color or occupied
    // by pieces of the same color.
    //
    // The moves it takes to reach a square, the less it adds to the score.

    std::vector<Square> square_queue;
    auto king_square = board.find_king(perspective);
    square_queue.push_back(king_square);

    std::map<Square, bool> visited;
    std::map<Square, int> distance;
    distance[king_square] = 0;

    double score = 0.0;

    for(size_t i = 0; i < square_queue.size(); ++i)
    {
        auto square = square_queue[i];

        if(visited[square])
        {
            continue;
        }
        visited[square] = true;

        bool attacked_by_other = board.square_attacked_by(square.file,
                                                          square.rank,
                                                          opposite(perspective));

        auto piece = board.piece_on_square(square.file, square.rank);
        bool occupied_by_same = piece &&
                                piece->color() == perspective &&
                                piece->pgn_symbol() != "K";

        auto is_safe = ! attacked_by_other && ! occupied_by_same;
        if(is_safe)
        {
            score += 1.0/(distance[square] + 1.0);
        }

        // always check the squares surrounding the king's current positions, even if
        // it is not safe (i.e., the king is in check)
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
                    if( ! visited[new_square])
                    {
                        square_queue.push_back(new_square);
                        distance[new_square] = distance[square] + 1;
                    }
                }
            }
        }
    }

    return score/maximum_score; // normalized so max is 1
}
