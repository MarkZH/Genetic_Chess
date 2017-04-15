#ifndef COMPLETE_MOVE_H
#define COMPLETE_MOVE_H

#include <string>

class Move;
class Board;

class Complete_Move
{
    public:
        Complete_Move(const Move* move_in, char file, int rank);
        Complete_Move();

        std::string name() const;
        std::string game_record_item(const Board& board) const;
        bool is_legal(const Board& board) const;
        std::string coordinate_move() const;
        bool can_capture() const;

        char start_file() const;
        char end_file() const;

        int start_rank() const;
        int end_rank() const;

        void side_effects(Board& board) const;

        bool operator==(const Complete_Move& other) const;

    private:
        const Move* move;
        char starting_file;
        int  starting_rank;
};


#endif // COMPLETE_MOVE_H
