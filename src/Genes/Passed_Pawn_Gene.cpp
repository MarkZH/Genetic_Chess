#include "Genes/Passed_Pawn_Gene.h"

#include <memory>
#include <string>
#include <array>

#include "Genes/Gene.h"

#include "Game/Board.h"
#include "Game/Piece.h"
#include "Game/Color.h"

double Passed_Pawn_Gene::score_board(const Board& board, Color perspective, size_t) const noexcept
{
    double score = 0.0;
    auto own_pawn = Piece{perspective, PAWN};
    auto other_pawn = Piece{opposite(perspective), PAWN};
    auto near_rank = (perspective == WHITE ? 1 : 8);
    auto far_rank  = (perspective == WHITE ? 7 : 2);
    auto rank_step = (perspective == WHITE ? 1 : -1);
    auto other_pawn_ranks_occupied = std::array<int, 8>{};

    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = far_rank; rank != near_rank; rank -= rank_step)
        {
            auto piece = board.piece_on_square({file, rank});
            if(piece == own_pawn)
            {
                score += 1.0;
                auto left_file  = std::max('a', char(file - 1));
                auto right_file = std::min('h', char(file + 1));
                auto score_diff = 1.0/(right_file - left_file + 1);

                for(char pawn_file = left_file; pawn_file <= right_file; ++pawn_file)
                {
                    auto other_pawn_rank = other_pawn_ranks_occupied[size_t(pawn_file - 'a')];
                    if(other_pawn_rank != 0 && other_pawn_rank != rank)
                    {
                        score -= score_diff;
                    }
                }
            }
            else if(piece == other_pawn)
            {
                other_pawn_ranks_occupied[size_t(file - 'a')] = rank;
            }
        }
    }

    return score/8; // maximum score == 1
}

std::string Passed_Pawn_Gene::name() const noexcept
{
    return "Passed Pawn Gene";
}
