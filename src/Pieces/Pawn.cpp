#include "Pieces/Pawn.h"

#include <vector>
#include <string>
#include <memory>

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
    possible_moves.emplace_back(std::make_unique<Pawn_Move>(color_in));
    possible_moves.emplace_back(std::make_unique<Pawn_Double_Move>(color_in));

    std::vector<std::shared_ptr<const Piece>> possible_promotions;
    possible_promotions.emplace_back(std::make_shared<Queen>(color_in));
    possible_promotions.emplace_back(std::make_shared<Knight>(color_in));
    possible_promotions.emplace_back(std::make_shared<Rook>(color_in));
    possible_promotions.emplace_back(std::make_shared<Bishop>(color_in));

    for(auto dir : {'r', 'l'})
    {
        possible_moves.emplace_back(std::make_unique<Pawn_Capture>(color_in, dir));
        possible_moves.emplace_back(std::make_unique<En_Passant>(color_in, dir));
        for(auto promote : possible_promotions)
        {
            possible_moves.emplace_back(std::make_unique<Pawn_Promotion_by_Capture>(promote, dir));
        }
    }

    for(auto promote : possible_promotions)
    {
        possible_moves.emplace_back(std::make_unique<Pawn_Promotion>(promote));
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
