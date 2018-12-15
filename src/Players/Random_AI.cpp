#include "Players/Random_AI.h"

#include <vector>
#include <string>
#include <stdexcept>

#include "Game/Board.h"
#include "Pieces/Piece.h"
#include "Moves/Move.h"
#include "Utility.h"

class Move;
class Clock;

const Move& Random_AI::choose_move(const Board& board, const Clock&) const
{
    return *choose_random_move(board);
}

const Move* Random_AI::choose_random_move(const Board& board) const
{
    const auto& moves = board.legal_moves();
    auto weights = std::vector<int>();
    weights.reserve(moves.size());
    auto weight_sum = 0;
    for(const auto& move : moves)
    {
        auto next_board = board;
        if(next_board.submit_move(*move).winner() != NONE)
        {
            return move;
        }

        auto move_score = 1;
        if(board.piece_on_square(move->start_file(), move->start_rank())->type() == PAWN)
        {
            move_score += 2;
        }
        if(board.move_captures(*move))
        {
            move_score += 4;
        }

        weights.push_back(move_score);
        weight_sum += move_score;
    }

    auto score_pick = Random::random_integer(0, weight_sum);
    for(size_t i = 0; i < weights.size(); ++i)
    {
        score_pick -= weights[i];
        if(score_pick <= 0)
        {
            return moves[i];
        }
    }

    throw std::logic_error("Bug in Random move picking algorithm");
}

std::string Random_AI::name() const
{
    return "Random AI";
}
