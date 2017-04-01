#include "Moves/Pawn_Promotion.h"

#include <memory>
#include <cstdlib>
#include <string>

#include "Game/Board.h"
#include "Pieces/Piece.h"
#include "Utility.h"

Pawn_Promotion::Pawn_Promotion(std::shared_ptr<const Piece> promotion_piece) :
    Pawn_Move(promotion_piece->color()),
    promote_to(std::move(promotion_piece))
{
}

Pawn_Promotion::~Pawn_Promotion()
{
}

void Pawn_Promotion::side_effects(Board& board, char file_start, int rank_start) const
{
    char file_end = file_start + file_change();
    int  rank_end = rank_start + rank_change();

    Pawn_Move::side_effects(board, file_start, rank_start);
    board.piece_moved[promote_to.get()] = true;
    board.piece_on_square(file_end, rank_end) = promote_to; // will call dtor on this if on original board
}

std::string Pawn_Promotion::name() const
{
    return std::string("Pawn Promotion ") + promote_to->pgn_symbol();
}

bool Pawn_Promotion::move_specific_legal(const Board& /* board */, char /* file_start */, int rank_start) const
{
    return (rank_start == (rank_change() == 1 ? 7 : 2)); // promoting
}

bool Pawn_Promotion::can_capture() const
{
    return false;
}

std::string Pawn_Promotion::game_record_item(const Board& board, char file_start, int rank_start) const
{
    return Pawn_Move::game_record_item(board, file_start, rank_start) + "=" + promote_to->pgn_symbol();
}

std::string Pawn_Promotion::coordinate_move(char file_start, int rank_start) const
{
    return Move::coordinate_move(file_start, rank_start) + char(std::tolower(promote_to->fen_symbol()));
}
