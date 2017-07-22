#include "Pieces/Pawn.h"

#include <vector>
#include <string>

#include "Game/Board.h"
#include "Game/Color.h"

#include "Moves/Move.h"
#include "Moves/Pawn_Move.h"
#include "Moves/Pawn_Capture.h"
#include "Moves/Pawn_Double_Move.h"
#include "Moves/Pawn_Promotion.h"
#include "Moves/Pawn_Promotion_by_Capture.h"
#include "Moves/En_Passant.h"

#include "Pieces/Piece.h"
#include "Pieces/Rook.h"
#include "Pieces/Knight.h"
#include "Pieces/Bishop.h"
#include "Pieces/Queen.h"

Pawn::Pawn(Color color_in) : Piece(color_in, "P")
{
    // Ranks where pawns can exist
    auto base_rank = (color_in == WHITE ? 2 : 7);

    // Pawns cannot make a normal move from this rank without promoting
    auto no_normal_move_rank = (color_in == WHITE ? 7 : 2);
    auto direction = (color_in == WHITE ? 1 : -1);
    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = base_rank; rank != no_normal_move_rank; rank += direction)
        {
            add_special_legal_move(std::make_unique<Pawn_Move>(color_in, file, rank));
        }
    }

    for(char file = 'a'; file <= 'h'; ++file)
    {
        add_special_legal_move(std::make_unique<Pawn_Double_Move>(color_in, file));
    }

    std::vector<const Piece*> possible_promotions;
    possible_promotions.emplace_back(Board::get_queen(color_in));
    possible_promotions.emplace_back(Board::get_knight(color_in));
    possible_promotions.emplace_back(Board::get_rook(color_in));
    possible_promotions.emplace_back(Board::get_bishop(color_in));

    for(auto dir : {RIGHT, LEFT})
    {
        auto first_file = (dir == RIGHT ? 'a' : 'b');
        auto last_file = (dir == RIGHT ? 'g' : 'h');
        for(char file = first_file; file <= last_file; ++file)
        {
            for(int rank = base_rank; rank != no_normal_move_rank; rank += direction)
            {
                add_special_legal_move(std::make_unique<Pawn_Capture>(color_in, dir, file, rank));
            }
        }

        auto en_passant_rank = (color_in == WHITE ? 5 : 4);
        for(char file = first_file; file <= last_file; ++file)
        {
            add_special_legal_move(std::make_unique<En_Passant>(color_in, dir, file, en_passant_rank));
        }

        for(auto promote : possible_promotions)
        {
            for(auto file = first_file; file <= last_file; ++file)
            {
                add_special_legal_move(std::make_unique<Pawn_Promotion_by_Capture>(promote, dir, file));
            }
        }
    }

    for(auto promote : possible_promotions)
    {
        for(auto file = 'a'; file <= 'h'; ++file)
        {
            add_special_legal_move(std::make_unique<Pawn_Promotion>(promote, file));
        }
    }

    // ASCII Art http://ascii.co.uk/art/chess (VK)
    ascii_art_lines.push_back("  _  ");
    ascii_art_lines.push_back(" ( ) ");
    ascii_art_lines.push_back("/___\\");
    if(color() == BLACK)
    {
        ascii_art_lines[1].replace(2, 1, "#");
        ascii_art_lines[2].replace(1, 3, "###");
    }
}

std::string Pawn::pgn_symbol() const
{
    return "";
}

bool Pawn::is_pawn() const
{
    return true;
}
