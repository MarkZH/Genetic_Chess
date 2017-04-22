#include "Moves/Move.h"

#include "Game/Board.h"
#include "Pieces/Piece.h"
#include "Utility.h"
#include "Exceptions/Illegal_Move_Exception.h"


Move::Move(int d_file_in, int d_rank_in) : d_file(d_file_in), d_rank(d_rank_in)
{
    if(std::abs(d_file) >= 8 ||
       std::abs(d_rank) >= 8 ||
       (d_file == 0 && d_rank == 0))
    {
        throw Illegal_Move_Exception("Badly constructed move: " +
                                     std::to_string(d_file) +
                                     " - "
                                     + std::to_string(d_rank));
    }
}

Move::~Move()
{
}

void Move::side_effects(Board&, char /* file_start */, int /* rank_start */) const
{
}

bool Move::is_legal(const Board& board, char file_start, int rank_start) const
{
    char file_end = file_start + file_change();
    int rank_end = rank_start + rank_change();

    // starting or ending square is outside board
    if( ! (Board::inside_board(file_start, rank_start)
           && Board::inside_board(file_end, rank_end)))
    {
        return false;
    }

    // Piece-move compatibility
    auto moving_piece = board.view_piece_on_square(file_start, rank_start);
    if( ! moving_piece
            || moving_piece->color() != board.whose_turn()
            || ! moving_piece->can_move(this))
    {
        return false;
    }

    // Check that there are no intervening pieces for straight-line moves
    // if(...) conditional excludes checking knight moves
    if(file_change() == 0
            || rank_change() == 0
            || abs(file_change()) == abs(rank_change()))
    {
        int max_move = std::max(abs(file_change()), abs(rank_change()));
        int file_step = file_change()/max_move;
        int rank_step = rank_change()/max_move;

        for(int step = 1; step < max_move; ++step)
        {
            if(board.view_piece_on_square(file_start + file_step*step,
                                          rank_start + rank_step*step))
            {
                return false;
            }
        }
    }

    // Cannot capture piece of same color
    auto attacked_piece = board.view_piece_on_square(file_end, rank_end);
    if(attacked_piece && moving_piece->color() == attacked_piece->color())
    {
        return false;
    }

    // Enforce non-capturing moves
    if(attacked_piece && ! can_capture())
    {
        return false;
    }

    if( ! move_specific_legal(board, file_start, rank_start))
    {
        return false;
    }

    // King should not be in check after move
    Board trial(board);
    trial.make_move(file_start, rank_start, file_end, rank_end);
    side_effects(trial, file_start, rank_start);
    return ! trial.king_is_in_check(board.whose_turn());
}

bool Move::move_specific_legal(const Board& /* board */, char /* file_start */, int /* rank_start */) const
{
    return true;
}

bool Move::can_capture() const
{
    return true;
}

int Move::file_change() const
{
    return d_file;
}

int Move::rank_change() const
{
    return d_rank;
}

std::string Move::name() const
{
    return "Move";
}

std::string Move::game_record_item(const Board& board, char file_start, int rank_start) const
{
    char file_end = file_start + file_change();
    int  rank_end = rank_start + rank_change();

    auto original_piece = board.view_piece_on_square(file_start, rank_start);
    std::string move_record = original_piece->pgn_symbol();

    bool record_file = false;
    bool record_rank = false;
    for(char file_other = 'a'; file_other <= 'h'; ++file_other)
    {
        for(int rank_other = 1; rank_other <= 8; ++rank_other)
        {
            if( ! board.view_piece_on_square(file_other, rank_other))
            {
                continue;
            }
            if(file_other == file_start && rank_other == rank_start)
            {
                continue;
            }
            if(file_other == file_end && rank_other == rank_end)
            {
                continue;
            }
            auto new_piece = board.view_piece_on_square(file_other, rank_other);
            if((*original_piece) != (*new_piece))
            {
                continue;
            }

            if(board.is_legal(file_other, rank_other, file_end, rank_end))
            {
                if(file_other != file_start && ! record_file)
                {
                    record_file = true;
                    continue;
                }
                if(rank_other != rank_start)
                {
                    record_rank = true;
                }
            }
        }
    }

    if(record_file)
    {
        move_record += file_start;
    }
    if(record_rank)
    {
        move_record += std::to_string(rank_start);
    }

    if(board.view_piece_on_square(file_end, rank_end))
    {
        move_record += 'x';
    }

    move_record += file_end;
    move_record += std::to_string(rank_end);

    return move_record;
}

std::string Move::coordinate_move(char file_start, int rank_start) const
{
    return file_start
           + std::to_string(rank_start)
           + char(file_start + file_change())
           + std::to_string(rank_start + rank_change());
}
