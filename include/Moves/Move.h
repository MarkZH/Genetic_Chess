#ifndef MOVE_H
#define MOVE_H

#include <string>

class Board;

class Move
{
    public:
        explicit Move(int d_file_in, int d_rank_in);
        Move(const Move&) = delete;
        Move& operator=(const Move&) = delete;
        virtual ~Move() = default;

        // side effects are changes to the state of the board beyond the change
        // in position of the moved piece and captured piece (movement by rook
        // in castling, marking a square as a en passant target after a double
        // pawn move, etc.)
        virtual void side_effects(Board& board, char file_start, int rank_start) const;

        // Contains rules for move
        bool is_legal(const Board& board, char file_start, int rank_start) const;

        // Can this move capture a piece?
        virtual bool can_capture() const;

        int file_change() const;
        int rank_change() const;

        virtual std::string name() const;

        virtual std::string game_record_item(const Board& board, char file_start, int rank_start) const;
        virtual std::string coordinate_move(char file_stat, int rank_start) const;

    protected:
        int d_file;
        int d_rank;

    private:
        virtual bool move_specific_legal(const Board& board, char file_start, int rank_start) const;
};

#endif // MOVE_H
