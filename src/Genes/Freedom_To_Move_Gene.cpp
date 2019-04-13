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
    static auto initial_score = double(attack_count(Board(), WHITE));

    return attack_count(board, perspective)/initial_score;
}

std::unique_ptr<Gene> Freedom_To_Move_Gene::duplicate() const
{
    return std::make_unique<Freedom_To_Move_Gene>(*this);
}

std::string Freedom_To_Move_Gene::name() const
{
    return "Freedom to Move Gene";
}

size_t Freedom_To_Move_Gene::attack_count(const Board& board, Color perspective) const
{
    size_t count = 0;
    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = 1; rank <= 8; ++rank)
        {
            auto piece = board.piece_on_square(file, rank);
            if( ! piece ||  piece->color() != perspective)
            {
                count += board.moves_attacking_square(file, rank, perspective).count();
            }
        }
    }

    return count;
}
