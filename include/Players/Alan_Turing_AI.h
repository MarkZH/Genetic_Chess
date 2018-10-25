#ifndef ALAN_TURING_AI_H
#define ALAN_TURING_AI_H

#include "Players/Player.h"

#include <vector>
#include <utility>

#include "Game/Color.h"

class Move;
class Board;
class CLock;
class Piece;
class Game_Result;

// As described in Turing, Alan M., ``Digital Computers Applied to Games.''
// Faster Than Thought. B. V. Bowden. London: Sir Isaac Pitman \& Sons, Ltd.,
// 1953, pgs. 286-297. Print.
class Alan_Turing_AI : public Player
{
    public:
        const Move& choose_move(const Board& board, const Clock& clock) const override;

        std::string name() const override;
        std::string author() const override;

    private:
        std::vector<const Move*> considerable_moves(const Board& board) const;
        bool is_considerable(const Move& move, const Board& board) const;
        bool last_move_captured(const Board& board) const;

        double material_value(const Board& board, Color perspective) const;
        double piece_value(const Piece* piece) const;

        double position_play_value(const Board& board, Color perspective) const;

        std::pair<double, double> score_board(const Board& board,
                                              Color perspective,
                                              const Game_Result& move_result) const;

        // Returns a pair of values consiting of the material value and the position value
        // of a board in that order.
        std::pair<double, double> position_value(const Board& board,
                                                 Color perspective,
                                                 const Game_Result& move_result) const;
};

#endif // ALAN_TURING_AI_H
