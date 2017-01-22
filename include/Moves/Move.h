#ifndef MOVE_H
#define MOVE_H

#include <string>
#include <memory>

class Board;

class Move
{
    public:
        explicit Move(int d_file_in, int d_rank_in);
        Move(const Move&) = delete;
        Move& operator=(const Move&) = delete;
        virtual ~Move();

        // side effects are changes to the state of the board beyond the change
        // in position of the moved piece and captured piece (movement by rook
        // in castling, marking a square as a en passant target after a double
        // pawn move, etc.)
        virtual void side_effects(Board& board, char file_start, int rank_start) const;

        // Contains extra rules for special moves (first move for double pawn move, etc.)
        bool is_legal(const Board& board, char file_start, int rank_start, bool king_check) const;

        // Can this move capture a piece?
        virtual bool can_capture() const;

        int file_change() const;
        int rank_change() const;

        virtual std::string name() const;

        virtual std::string game_record_item(const Board& board, char file_start, int rank_start) const;

    protected:
        int d_file;
        int d_rank;

    private:
        virtual bool move_specific_legal(const Board& board, char file_start, int rank_start) const;
};

struct Complete_Move
{
    Complete_Move(const std::shared_ptr<const Move>& move_in, char file, int rank) :
        move(move_in),
        starting_file(file),
        starting_rank(rank)
    {
    }

    std::shared_ptr<const Move> move;
    char starting_file;
    int  starting_rank;
};

#endif // MOVE_H
