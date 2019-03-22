#ifndef MOVE_H
#define MOVE_H

#include <string>

class Board;

class Move
{
    public:
        Move(char file_start, int rank_start,
             char file_end,   int rank_end);
        virtual ~Move() = default;
        Move(const Move&) = delete;
        Move& operator=(const Move&) = delete;

        // side effects are changes to the state of the board beyond the change
        // in position of the moved piece and captured piece (movement by rook
        // in castling, marking a square as a en passant target after a double
        // pawn move, etc.)
        virtual void side_effects(Board& board) const;

        // Contains rules for move
        bool is_legal(const Board& board) const;

        // Can this move capture a piece?
        bool can_capture() const;

        char start_file() const;
        int  start_rank() const;

        char end_file() const;
        int  end_rank() const;

        int file_change() const;
        int rank_change() const;

        std::string game_record_item(const Board& board) const;
        std::string coordinate_move() const;

        // Special case functions
        bool is_en_passant() const;
        bool is_castling() const;
        virtual char promotion_piece_symbol() const;

    protected:
        char starting_file;
        int  starting_rank;

        char ending_file;
        int  ending_rank;

        bool able_to_capture;
        bool is_en_passant_move;
        bool is_castling_move;

        virtual std::string game_record_move_item(const Board& board) const;

    private:
        virtual bool move_specific_legal(const Board& board) const;

        std::string game_record_ending_item(Board board) const;
};

#endif // MOVE_H
