#ifndef MOVE_H
#define MOVE_H

#include <string>

class Board;

//! A class to represent the movement of pieces.
class Move
{
    public:
        Move(char file_start, int rank_start,
             char file_end,   int rank_end);
        virtual ~Move() = default;

        //! Since Piece instances are static, there is no reason to copy Move instances.

        //! This saves work by preventing all unnecessary copying (which is all copying).
        Move(const Move&) = delete;

        //! Since there's only one instance of every Move that is unique to the Piece that contains it, assignment can only lose information.
        Move& operator=(const Move&) = delete;

        virtual void side_effects(Board& board) const;

        bool is_legal(const Board& board) const;

        bool can_capture() const;

        char start_file() const;
        int  start_rank() const;

        char end_file() const;
        int  end_rank() const;

        int file_change() const;
        int rank_change() const;

        std::string game_record_item(const Board& board) const;
        std::string coordinate_move() const;

        bool is_en_passant() const;
        bool is_castling() const;
        virtual char promotion_piece_symbol() const;

    protected:
        //! The file of the square where the move starts.
        char starting_file;
        //! The rank of the square where the move starts.
        int  starting_rank;

        //! The file of the square where the move ends.
        char ending_file;
        //! The rank of the square where the move ends.
        int  ending_rank;

        //! Is the move allowed to capture a piece on the end square?
        bool able_to_capture;
        //! Is this move en passant?
        bool is_en_passant_move;
        //! Is the move a castling move?
        bool is_castling_move;
            
        virtual std::string game_record_move_item(const Board& board) const;

    private:
        virtual bool move_specific_legal(const Board& board) const;

        std::string game_record_ending_item(Board board) const;
};

#endif // MOVE_H
