#include "Genes/King_Confinement_Gene.h"

#include "Game/Board.h"
#include "Game/Color.h"
#include "Pieces/Piece.h"

King_Confinement_Gene::King_Confinement_Gene() : Gene(0.0)
{
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

    std::vector<std::pair<char, int>> square_queue;
    square_queue.emplace_back(board.find_king(perspective));

    std::map<std::pair<char, int>, bool> visited;

    int safe_square_count = 0;

    for(size_t i = 0; i < square_queue.size(); ++i)
    {
        if(visited[square_queue[i]])
        {
            continue;
        }
        visited[square_queue[i]] = true;

        auto file = square_queue[i].first;
        auto rank = square_queue[i].second;

        bool attacked_by_other = board.square_attacked_by(file, rank, opposite(perspective));

        auto piece = board.piece_on_square(file, rank);
        bool occupied_by_same = piece &&
                                piece->color() == perspective &&
                                piece->pgn_symbol() != "K";

        auto is_safe = ! attacked_by_other && ! occupied_by_same;
        if(is_safe)
        {
            ++safe_square_count;
        }

        // always check the squares surrounding the king's current positions, even if
        // it is not safe (i.e., the king is in check)
        if(is_safe || square_queue.size() == 1)
        {
            for(char new_file = file - 1; new_file <= file + 1; ++new_file)
            {
                if( ! board.inside_board(new_file))
                {
                    continue;
                }

                for(int new_rank = rank - 1; new_rank <= rank + 1; ++new_rank)
                {
                    if( ! board.inside_board(new_rank))
                    {
                        continue;
                    }

                    if( ! visited[std::make_pair(new_file, new_rank)])
                    {
                        square_queue.emplace_back(new_file, new_rank);
                    }
                }
            }
        }
    }

    return safe_square_count/64.0; // normalized so max is 1
}
