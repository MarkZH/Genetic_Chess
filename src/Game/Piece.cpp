#include "Game/Piece.h"

#include <cctype>
#include <vector>
#include <algorithm>
#include <cassert>
#include <array>

#include "Game/Board.h"
#include "Game/Piece_Types.h"

#include "Moves/Move.h"
#include "Moves/Direction.h"
#include "Moves/Pawn_Move.h"
#include "Moves/Pawn_Capture.h"
#include "Moves/Pawn_Double_Move.h"
#include "Moves/Pawn_Promotion.h"
#include "Moves/Pawn_Promotion_by_Capture.h"
#include "Moves/En_Passant.h"
#include "Moves/Castle.h"

#include "Utility/String.h"

int Piece::maximum_piece_height = 0;

//! Create a piece.

//! \param color_in The color of the piece.
//! \param type_in The type of piece.
Piece::Piece(Color color_in, Piece_Type type_in) :
    my_color(color_in),
    my_type(type_in)
{
    switch(type())
    {
        case PAWN:
            add_pawn_moves();
            add_pawn_art();
            break;
        case ROOK:
            add_rook_moves();
            add_rook_art();
            break;
        case KNIGHT:
            add_knight_moves();
            add_knight_art();
            break;
        case BISHOP:
            add_bishop_moves();
            add_bishop_art();
            break;
        case QUEEN:
            add_bishop_moves();
            add_rook_moves();
            add_queen_art();
            break;
        case KING:
            add_king_moves();
            add_king_art();
            break;
        default:
            throw std::invalid_argument("Program bug: Invalid piece type in Piece(): " + std::to_string(type()));
    }
}

//! Return a row of the ASCII art representation of the piece.

//! \param row Which row of the square to return, with 0 being the top.
//!        If the height is above or below the piece's picture, then an
//!        empty string is returned.
//! \param square_height The height of the square in characters.
//! \returns One row of text that forms a picture of the piece.
//! \throws Debug assert fail if the square height is smaller than the piece height.
//!
//! Piece design by VK (?) and taken from http://ascii.co.uk/art/chess.
std::string Piece::ascii_art(int row, int square_height) const
{
    assert(square_height >= maximum_piece_height);

    int empty_bottom_rows = (square_height - maximum_piece_height)/2;
    int empty_top_rows = square_height - int(ascii_art_lines.size()) - empty_bottom_rows;
    int line = row - empty_top_rows;
    if(0 <= line && line < int(ascii_art_lines.size()))
    {
        return ascii_art_lines[line];
    }
    return {};
}

//! The color of the piece.

//! \returns The Color of the player that controls the piece.
Color Piece::color() const
{
    return my_color;
}

//! Get the PGN symbol for the piece.

//! \returns The symbol for the moving piece when writing a game record. A pawn is represented by an empty string.
std::string Piece::pgn_symbol() const
{
    return type() == PAWN ? std::string{} : std::string(1, std::toupper(fen_symbol()));
}

// Get the piece symbol when writing an FEN string.

//! \returns A single character symbol for the piece. Uppercase is white, lowercase is black.
char Piece::fen_symbol() const
{
    static auto symbols = "PRNBQK";
    auto symbol = symbols[type()];
    return (my_color == WHITE ? symbol : std::tolower(symbol));
}

//! Check that a piece is allowed to make a certain move.

//! \param move A pointer to a prospective move.
//! \returns Whether or not the piece is allowed to move in the manner described by the parameter.
bool Piece::can_move(const Move* move) const
{
    return std::find_if(possible_moves.begin(),
                        possible_moves.end(),
                        [move](const auto& x){ return x.get() == move; }) != possible_moves.end();
}

//! Get all possibly legal moves of a piece starting from a given square.

//! \param file The file of the starting square.
//! \param rank The rank of the starting square.
//! \returns A list of legal moves starting from that square.
const std::vector<const Move*>& Piece::move_list(char file, int rank) const
{
    return legal_moves[Board::square_index(file, rank)];
}

//! Get the type of the piece.

//! \returns The kind of piece, i.e., PAWN, ROOK, etc.
Piece_Type Piece::type() const
{
    return my_type;
}

void Piece::add_standard_legal_move(int file_step, int rank_step)
{
    for(char start_file = 'a'; start_file <= 'h'; ++start_file)
    {
        for(int start_rank = 1; start_rank <= 8; ++start_rank)
        {
            char end_file = start_file + file_step;
            int  end_rank = start_rank + rank_step;

            if(Board::inside_board(end_file, end_rank))
            {
                add_legal_move<Move>(start_file, start_rank,
                                     end_file, end_rank);
            }
        }
    }
}

void Piece::add_pawn_moves()
{
    auto base_rank = (color() == WHITE ? 2 : 7);
    auto no_normal_move_rank = (color() == WHITE ? 7 : 2);
    auto direction = (color() == WHITE ? 1 : -1);
    for(char file = 'a'; file <= 'h'; ++file)
    {
        for(int rank = base_rank; rank != no_normal_move_rank; rank += direction)
        {
            add_legal_move<Pawn_Move>(color(), file, rank);
        }
    }

    for(char file = 'a'; file <= 'h'; ++file)
    {
        add_legal_move<Pawn_Double_Move>(color(), file);
    }

    auto possible_promotions = {QUEEN, KNIGHT, ROOK, BISHOP};

    for(auto dir : {RIGHT, LEFT})
    {
        auto first_file = (dir == RIGHT ? 'a' : 'b');
        auto last_file = (dir == RIGHT ? 'g' : 'h');
        for(char file = first_file; file <= last_file; ++file)
        {
            for(int rank = base_rank; rank != no_normal_move_rank; rank += direction)
            {
                add_legal_move<Pawn_Capture>(color(), dir, file, rank);
            }
        }

        for(char file = first_file; file <= last_file; ++file)
        {
            add_legal_move<En_Passant>(color(), dir, file);
        }

        for(auto promote : possible_promotions)
        {
            for(auto file = first_file; file <= last_file; ++file)
            {
                add_legal_move<Pawn_Promotion_by_Capture>(promote, color(), dir, file);
            }
        }
    }

    for(auto promote : possible_promotions)
    {
        for(auto file = 'a'; file <= 'h'; ++file)
        {
            add_legal_move<Pawn_Promotion>(promote, color(), file);
        }
    }
}

void Piece::add_rook_moves()
{
    for(int d_file = -1; d_file <= 1; ++d_file)
    {
        for(int d_rank = -1; d_rank <= 1; ++d_rank)
        {
            if(d_file == 0 && d_rank == 0) { continue; }
            if(d_file != 0 && d_rank != 0) { continue; }

            for(int move_size = 1; move_size <= 7; ++move_size)
            {
                add_standard_legal_move(move_size*d_file, move_size*d_rank);
            }
        }
    }
}

void Piece::add_knight_moves()
{
    for(auto d_file : {1, 2})
    {
        auto d_rank = 3 - d_file;
        for(auto file_direction : {-1, 1})
        {
            for(auto rank_direction : {-1, 1})
            {
                add_standard_legal_move(d_file*file_direction, d_rank*rank_direction);
            }
        }
    }
}

void Piece::add_bishop_moves()
{
    for(int d_rank : {-1, 1})
    {
        for(int d_file : {-1, 1})
        {
            for(int move_size = 1; move_size <= 7; ++move_size)
            {
                add_standard_legal_move(move_size*d_file, move_size*d_rank);
            }
        }
    }
}

void Piece::add_king_moves()
{
    for(int d_rank = -1; d_rank <= 1; ++d_rank)
    {
        for(int d_file = -1; d_file <= 1; ++d_file)
        {
            if(d_rank == 0 && d_file == 0) { continue; }

            add_standard_legal_move(d_file, d_rank);
        }
    }

    int base_rank = (color() == WHITE ? 1 : 8);
    add_legal_move<Castle>(base_rank, LEFT);
    add_legal_move<Castle>(base_rank, RIGHT);
}

void Piece::add_pawn_art()
{
    // ASCII Art http://ascii.co.uk/art/chess (VK)
    ascii_art_lines.push_back("( )");
    ascii_art_lines.push_back("/___\\");
    add_color();
}

void Piece::add_rook_art()
{
    // ASCII Art http://ascii.co.uk/art/chess (VK)
    ascii_art_lines.push_back("|U|");
    ascii_art_lines.push_back("| |");
    ascii_art_lines.push_back("/___\\");
    add_color();
}

void Piece::add_knight_art()
{
    // ASCII Art http://ascii.co.uk/art/chess (VK)
    ascii_art_lines.push_back("/\")");
    ascii_art_lines.push_back("7 (");
    ascii_art_lines.push_back("/___\\");
    add_color();
}

void Piece::add_bishop_art()
{
    // ASCII art http://ascii.co.uk/art/chess (VK)
    ascii_art_lines.push_back("(V)");
    ascii_art_lines.push_back(") (");
    ascii_art_lines.push_back("/___\\");
    add_color();
}

void Piece::add_queen_art()
{
    // ASCII Art http://ascii.co.uk/art/chess (VK)
    ascii_art_lines.push_back("\\^/");
    ascii_art_lines.push_back(") (");
    ascii_art_lines.push_back("(___)");
    add_color();
}

void Piece::add_king_art()
{
    // ASCII Art http://ascii.co.uk/art/chess (VK)
    ascii_art_lines.push_back("\\+/");
    ascii_art_lines.push_back("| |");
    ascii_art_lines.push_back("/___\\");
    add_color();
}

void Piece::add_color()
{
    maximum_piece_height = std::max(maximum_piece_height, int(ascii_art_lines.size()));

    if(color() == WHITE)
    {
        return;
    }

    for(auto& line : ascii_art_lines)
    {
        if(String::contains(line, ' ') || String::contains(line, "___"))
        {
            for(auto& c : line)
            {
                if(c == ' ' || c == '_')
                {
                    c = '#';
                }
            }
        }
    }
}

//! Gives a list of moves that are allowed to capture other pieces.

//! \param file The file of the square where the attacking move starts.
//! \param rank The rank of the square where the attacking move starts.
const std::vector<const Move*>& Piece::attacking_moves(char file, int rank) const
{
    return attack_moves[Board::square_index(file, rank)];
}
