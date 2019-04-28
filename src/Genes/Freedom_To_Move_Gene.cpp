#include "Genes/Freedom_To_Move_Gene.h"

#include <memory>
#include <string>
#include <array>
#include <bitset>

#include "Genes/Gene.h"
#include "Game/Board.h"
#include "Game/Color.h"
#include "Game/Piece.h"

double Freedom_To_Move_Gene::score_board(const Board& board, Color perspective, size_t) const
{
    static auto initial_score = double(Board().attack_count(WHITE));
    return board.attack_count(perspective)/initial_score;
}

std::unique_ptr<Gene> Freedom_To_Move_Gene::duplicate() const
{
    return std::make_unique<Freedom_To_Move_Gene>(*this);
}

std::string Freedom_To_Move_Gene::name() const
{
    return "Freedom to Move Gene";
}

size_t Freedom_To_Move_Gene::attack_count_scan(const Board& board, Color perspective) const
{
    size_t count = 0;
    for(auto square : Square::all_squares())
    {
        if(board.moves_attacking_square(square, perspective).any())
        {
            auto piece = board.piece_on_square(square);
            if(!piece || piece->color() != perspective)
            {
                count += board.moves_attacking_square(square, perspective).count();
            }
        }
    }

    return count;
}

//! Returns whether Board::attack_count() returns the same answer as the
//! former method used by the Freedom_To_Move_Gene that involved scanning
//! the Board square-by-square.
bool Freedom_To_Move_Gene::verify(const Board& board) const
{
    return board.attack_count(WHITE) == attack_count_scan(board, WHITE) &&
           board.attack_count(BLACK) == attack_count_scan(board, BLACK);
}
