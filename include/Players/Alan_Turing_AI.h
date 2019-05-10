#ifndef ALAN_TURING_AI_H
#define ALAN_TURING_AI_H

#include "Players/Player.h"

#include <vector>
#include <tuple>

#include "Game/Color.h"

class Move;
class Board;
class CLock;
class Piece;
class Game_Result;

//! An AI player designed by Alan Turing and David Champernowne.
//
//! Not having a computer on which to run this program, Turing first ran
//! this chess program using pencil and paper, taking 30 minutes per move.
//!
//! Algorithm taken from Turing, Alan M., "Digital Computers Applied to Games."
//! Faster Than Thought. B. V. Bowden. London: Sir Isaac Pitman \& Sons, Ltd.,
//! 1953, pgs. 286-297. Print.
class Alan_Turing_AI : public Player
{
    public:
        const Move& choose_move(const Board& board, const Clock& clock) const override;

        std::string name() const override;
        std::string author() const override;

    private:
        std::vector<const Move*> considerable_moves(const Board& board) const;
        bool is_considerable(const Move& move, const Board& board) const;

        double material_value(const Board& board, Color perspective) const;
        double piece_value(Piece piece) const;

        double position_play_value(const Board& board, Color perspective) const;

        std::tuple<double, double, int> score_board(const Board& board,
                                                    Color perspective,
                                                    const Game_Result& move_result,
                                                    int depth) const;

        // Returns a pair of values consiting of the material value and the position value
        // of a board in that order.
        std::tuple<double, double, int> position_value(const Board& board,
                                                       Color perspective,
                                                       const Game_Result& move_result,
                                                       int depth) const;
};

#endif // ALAN_TURING_AI_H
