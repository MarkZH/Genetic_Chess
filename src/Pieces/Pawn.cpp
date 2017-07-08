#include "Pieces/Pawn.h"

#include <vector>
#include <string>

#include "Pieces/Piece.h"
#include "Game/Color.h"

#include "Moves/Move.h"
#include "Moves/Pawn_Move.h"
#include "Moves/Pawn_Capture.h"
#include "Moves/Pawn_Double_Move.h"
#include "Moves/Pawn_Promotion.h"
#include "Moves/Pawn_Promotion_by_Capture.h"
#include "Moves/En_Passant.h"

#include "Pieces/Rook.h"
#include "Pieces/Knight.h"
#include "Pieces/Bishop.h"
#include "Pieces/Queen.h"

Pawn::Pawn(Color color_in) : Piece(color_in, "P")
{
    // Ranks where pawns can exist
    auto base_rank = (color_in == WHITE ? 2 : 7);

    auto no_normal_move_rank = (color_in == WHITE ? 7 : 2);
    auto direction = (color_in == WHITE ? 1 : -1);
    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = base_rank; rank != no_normal_move_rank; rank += direction)
        {
            add_special_legal_move<Pawn_Move>(file, rank, color_in);
        }
    }

    for(char file = 'a'; file <= 'h'; ++file)
    {
        add_special_legal_move<Pawn_Double_Move>(file, base_rank, color_in);
    }

    std::vector<std::shared_ptr<const Piece>> possible_promotions;
    possible_promotions.emplace_back(std::make_shared<Queen>(color_in));
    possible_promotions.emplace_back(std::make_shared<Knight>(color_in));
    possible_promotions.emplace_back(std::make_shared<Rook>(color_in));
    possible_promotions.emplace_back(std::make_shared<Bishop>(color_in));

    // 'r' and 'l' refer to the direction of the capture from white's perspective
    for(auto dir : {'r', 'l'})
    {
        auto first_file = (dir == 'r' ? 'a' : 'b');
        auto last_file = (dir == 'r' ? 'g' : 'h');
        for(char file = first_file; file <= last_file; ++file)
        {
            for(int rank = base_rank; rank != no_normal_move_rank; rank += direction)
            {
                add_special_legal_move<Pawn_Capture>(file, rank, color_in, dir);
            }
        }

        auto en_passant_rank = (color_in == WHITE ? 5 : 4);
        for(char file = first_file; file <= last_file; ++file)
        {
            add_special_legal_move<En_Passant>(file, en_passant_rank, color_in, dir);
        }

        for(auto promote : possible_promotions)
        {
            for(auto file = first_file; file <= last_file; ++file)
            {
                add_special_legal_move<Pawn_Promotion_by_Capture>(file, no_normal_move_rank, promote, dir);
            }
        }
    }

    for(auto promote : possible_promotions)
    {
        for(auto file = 'a'; file <= 'h'; ++file)
        {
            add_special_legal_move<Pawn_Promotion>(file, no_normal_move_rank, promote);
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
