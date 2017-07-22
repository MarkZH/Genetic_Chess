#ifndef CLAUDE_SHANNON_AI_H
#define CLAUDE_SHANNON_AI_H

#include "Players/Player.h"

/***************************************
Philosophical Magazine, Ser.7, Vol. 41, No. 314 - March 1950.
XXII. Programming a Computer for Playing Chess [1]
By CLAUDE E. SHANNON
Bell Telephone Laboratories, Inc., Murray Hill, N.J.2
[Received November 8, 1949]

[1] First presented at the National IRE Convention, March 9, 1949, NewYork, U.S.A.

http://vision.unipv.it/IA1/aa2009-2010/ProgrammingaComputerforPlayingChess.pdf
*****************************************/

#include "Game/Color.h"

class Move;
class Board;
class Clock;

class Claude_Shannon_AI : public Player
{
    public:
        Claude_Shannon_AI();

        const Move& choose_move(const Board& board, const Clock& clock) const override;

        std::string name() const override;
        std::string author() const override;

    private:
        int recursion_depth;

        int evalutate_position(const Board& board, Color perspective, int look_ahead) const;
        const Move& choose_move(const Board& board, int look_ahead) const;

        int count_pieces(const Board& board, Color piece_color, char piece_symbol) const;
        int count_doubled_pawns(const Board& board, Color pawn_color) const;
        int count_backward_pawns(const Board& board, Color pawn_color) const;
        int count_isolated_pawns(const Board& board, Color pawn_color) const;
        int count_legal_moves(const Board& board, Color perspective) const;
};

#endif // CLAUDE_SHANNON_AI_H
