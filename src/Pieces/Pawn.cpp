#include "Pieces/Pawn.h"
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

Pawn::Pawn(Color color_in) : Piece(color_in)
{
    symbol = "P";

    // Normal move
    possible_moves.emplace_back(new Pawn_Move(my_color));

    for(auto dir : {'r', 'l'})
    {
        // Capture
        possible_moves.emplace_back(new Pawn_Capture(my_color, dir));

        // En Passant Capture
        possible_moves.emplace_back(new En_Passant(my_color, dir));

        // Promotion by capture
        possible_moves.emplace_back(new Pawn_Promotion_by_Capture(new Rook(my_color), dir));
        possible_moves.emplace_back(new Pawn_Promotion_by_Capture(new Knight(my_color), dir));
        possible_moves.emplace_back(new Pawn_Promotion_by_Capture(new Bishop(my_color), dir));
        possible_moves.emplace_back(new Pawn_Promotion_by_Capture(new Queen(my_color), dir));
    }

    // Double first move
    possible_moves.emplace_back(new Pawn_Double_Move(my_color));

    // Promotion
    possible_moves.emplace_back(new Pawn_Promotion(new Rook(my_color)));
    possible_moves.emplace_back(new Pawn_Promotion(new Knight(my_color)));
    possible_moves.emplace_back(new Pawn_Promotion(new Bishop(my_color)));
    possible_moves.emplace_back(new Pawn_Promotion(new Queen(my_color)));


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

Pawn::~Pawn()
{
}

std::string Pawn::pgn_symbol() const
{
    return "";
}
