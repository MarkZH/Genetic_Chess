#include "Genes/Passed_Pawn_Gene.h"

#include <memory>
#include <string>
#include <algorithm>

#include "Game/Board.h"
#include "Pieces/Piece.h"
#include "Pieces/Piece_Types.h"

double Passed_Pawn_Gene::score_board(const Board& board) const
{
    // Counts the number of passed pawns. A passed pawn is one with
    // no enemy pawns on the same or adjacent files ahead of it.
    double score = 0;

    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 2; rank <= 7; ++rank)
        {
            auto piece = board.piece_on_square(file, rank);
            if(piece && piece->type() == PAWN && piece->color() == board.whose_turn())
            {
                auto rank_step = (board.whose_turn() == WHITE ? 1 : -1);
                auto last_rank = (board.whose_turn() == WHITE ? 8 : 1);
                auto left_file = std::max('a', char(file - 1));
                auto right_file = std::min('h', char(file + 1));

                auto possible_passed_pawn = true;
                for(int pawn_rank = rank + rank_step;
                    possible_passed_pawn && pawn_rank != last_rank;
                    pawn_rank += rank_step)
                {
                    for(char pawn_file = left_file;
                        possible_passed_pawn && pawn_file <= right_file;
                        ++pawn_file)
                    {
                        auto check_piece = board.piece_on_square(pawn_file, pawn_rank);
                        if(check_piece &&
                           check_piece->type() == PAWN &&
                           check_piece->color() == opposite(board.whose_turn()))
                        {
                            possible_passed_pawn = false;
                        }
                    }
                }

                if(possible_passed_pawn)
                {
                    score += 1;
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
