#include "Genes/Passed_Pawn_Gene.h"

#include <memory>
#include <string>
#include <algorithm>

#include "Game/Board.h"
#include "Pieces/Piece.h"
#include "Pieces/Piece_Types.h"

double Passed_Pawn_Gene::score_board(const Board& board, const Board&) const
{
    // Counts the number of passed pawns. A passed pawn is one with
    // no enemy pawns on the same or adjacent files ahead of it.
    double score = board.number_of_promoted_pawns(board.whose_turn());
    auto own_pawn = board.get_piece(PAWN, board.whose_turn());
    auto other_pawn = board.get_piece(PAWN, opposite(board.whose_turn()));

    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 2; rank <= 7; ++rank)
        {
            auto piece = board.piece_on_square(file, rank);
            if(piece == own_pawn)
            {
                score += 1.0;
                auto last_rank = (board.whose_turn() == WHITE ? 7 :  2);
                if(last_rank == rank)
                {
                    continue;
                }

                auto rank_step = (board.whose_turn() == WHITE ? 1 : -1);
                auto left_file  = std::max('a', char(file - 1));
                auto right_file = std::min('h', char(file + 1));

                for(char pawn_file = left_file; pawn_file <= right_file; ++pawn_file)
                {
                    for(int pawn_rank = rank + rank_step; pawn_rank != last_rank; pawn_rank += rank_step)
                    {
                        if(board.piece_on_square(pawn_file, pawn_rank) == other_pawn)
                        {
                            score -= (file == 'a' || file == 'h' ? 0.5 : 1.0/3.0);
                            break;
                        }
                    }
                }
            }
        }
    }

    return score/8; // maximum score == 1
}

std::unique_ptr<Gene> Passed_Pawn_Gene::duplicate() const
{
    return std::make_unique<Passed_Pawn_Gene>(*this);
}

std::string Passed_Pawn_Gene::name() const
{
    return "Passed Pawn Gene";
}
