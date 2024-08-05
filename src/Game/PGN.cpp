#include "Game/PGN.h"

#include "Game/Board.h"
#include "Game/Move.h"
#include "Game/Game_Result.h"
#include "Game/Color.h"

#include "Utility/Exceptions.h"
#include "Utility/String.h"

#include <iostream>
#include <fstream>
#include <string>

namespace
{
    //! Print the line number of the input stream at the current position.
    int line_number(std::istream& input, std::streampos error_position) noexcept;

    bool check_rule_result(const std::string& rule_source,
                           const std::string& rule_name,
                           const bool expected_ruling,
                           const bool actual_ruling,
                           std::istream& input) noexcept
    {
        const auto pass = expected_ruling == actual_ruling;
        if( ! pass)
        {
            const auto line_count = line_number(input, input.tellg());
            std::cerr << rule_source << " indicates "
                << (expected_ruling ? "" : "no ")
                << rule_name << ", but last move did "
                << (actual_ruling ? "" : "not ")
                << "trigger rule (line: " << line_count << ").\n";
        }

        return pass;
    }

    std::string get_pgn_header_value(std::istream& is) noexcept
    {
        std::string rest;
        std::getline(is, rest);
        return String::extract_delimited_text(rest, '"', '"');
    }

    //! \brief Skip to the end of the current line.
    //! 
    //! \param input A text input stream.
    void skip_rest_of_line(std::istream& input) noexcept
    {
        input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    int line_number(std::istream& input, std::streampos error_position) noexcept
    {
        if( ! input)
        {
            input.clear();
            input.seekg(0, input.end);
            error_position = input.tellg();
        }
        const auto position = input.tellg();
        input.seekg(0);
        auto line_count = 0;
        do
        {
            skip_rest_of_line(input);
            ++line_count;
        } while(input && input.tellg() < error_position);
        input.seekg(position);
        return line_count;
    }

    //! \brief Skip to the end of the current curly-braced comment. Returns true if successful, and false if the end of the input stream is reached.
    //! 
    //! \param input A text input stream. The position within the input stream should be passed the opening curly brace.
    bool skip_braced_comment(std::istream& input) noexcept
    {
        const auto stream_position = input.tellg();
        input.ignore(std::numeric_limits<std::streamsize>::max(), '}');
        if( ! input)
        {
            const auto line_count = line_number(input, stream_position);
            std::cerr << "Reached end of input before closing curly brace: line " << line_count << ".\n";
            return false;
        }

        return true;
    }

    //! \brief Skip to the end of the current RAV section. Returns true if successful, and false if the end of the input stream is reached.
    //! 
    //! \param input A text input stream. The position within the input stream should be passed the opening parenthesis.
    bool skip_rav(std::istream& input) noexcept
    {
        while(true)
        {
            const auto input_position = input.tellg();
            const auto c = input.get();
            if( ! input)
            {
                const auto line_count = line_number(input, input_position);
                std::cerr << "Reached end of input before end of RAV: line " << line_count << ".\n";
                return false;
            }

            switch(c)
            {
                case '(':
                    skip_rav(input);
                    break;
                case ')':
                    return true;
                case ';':
                    skip_rest_of_line(input);
                    break;
                case '{':
                    skip_braced_comment(input);
                    break;
                default:
                    continue;
            }
        }
    }
}

bool PGN::confirm_game_record(const std::string& file_name)
{
    auto input = std::ifstream(file_name);
    if( ! input)
    {
        throw std::runtime_error("Could not open file " + file_name + " for reading.");
    }

    auto game_count = 0;
    std::string move_number;

    auto expected_winner = Winner_Color::NONE;
    auto expect_checkmate = true;
    auto expect_fifty_move_draw = false;
    auto expect_threefold_draw = false;
    auto in_game = false;
    auto finished_game = false;
    Board board;
    Game_Result result;
    while(true)
    {
        if(finished_game)
        {
            if( ! check_rule_result("Header",
                                    "50-move draw",
                                    expect_fifty_move_draw,
                                    result.ending_reason().contains("50"),
                                    input))
            {
                return false;
            }

            if( ! check_rule_result("Header",
                                    "threefold draw",
                                    expect_threefold_draw,
                                    result.ending_reason().contains("fold"),
                                    input))
            {
                return false;
            }

            if( ! check_rule_result("Header",
                                    "checkmate",
                                    expect_checkmate,
                                    result.ending_reason().contains("mates"),
                                    input))
            {
                return false;
            }

            move_number = {};

            expected_winner = Winner_Color::NONE;
            expect_checkmate = true;
            expect_fifty_move_draw = false;
            expect_threefold_draw = false;
            in_game = false;
            finished_game = false;
            board = Board();
            result = {};
            ++game_count;
        }

        const auto next_character = input.get();
        if( ! input)
        {
            if(in_game)
            {
                std::cout << "File ended in middle of game.\n";
            }
            else
            {
                std::cout << "Found " << game_count << " " << (game_count == 1 ? "game" : "games") << ".\n";
            }

            return ! in_game;
        }

        if(std::isspace(next_character))
        {
            continue;
        }

        if(next_character == ';')
        {
            skip_rest_of_line(input);
            continue;
        }

        if(next_character == '{')
        {
            if(skip_braced_comment(input))
            {
                continue;
            }
            else
            {
                return false;
            }
        }

        if(next_character == '}')
        {
            const auto line_count = line_number(input, input.tellg());
            std::cerr << "Found closing curly brace before opener (line: " << line_count << ")\n";
            return false;
        }

        if(next_character == '(')
        {
            if(skip_rav(input))
            {
                continue;
            }
            else
            {
                return false;
            }
        }

        if(next_character == ')')
        {
            const auto line_count = line_number(input, input.tellg());
            std::cerr << "Found closing RAV parentheses before opener (line: " << line_count << ")\n";
            return false;
        }

        if(in_game && next_character == '[')
        {
            const auto line_count = line_number(input, input.tellg());
            std::cerr << "Found header line in the middle of another game (line: " << line_count << ")\n";
            return false;
        }

        std::string word;
        input.unget();
        input >> word;
        if(word == "[Result")
        {
            const auto result_tag = get_pgn_header_value(input);
            if(result_tag == "1-0")
            {
                expected_winner = Winner_Color::WHITE;
            }
            else if(result_tag == "0-1")
            {
                expected_winner = Winner_Color::BLACK;
            }
            else if(result_tag == "1/2-1/2")
            {
                expect_checkmate = false;
            }
            else if(result_tag == "*")
            {
                expect_checkmate = false;
            }
            else
            {
                const auto line_count = line_number(input, input.tellg());
                std::cerr << "Malformed Result: " << word << " " << result_tag << " (line: " << line_count << ")\n";
                return false;
            }
        }
        else if(word == "[GameEnding")
        {
            const auto terminator = get_pgn_header_value(input);
            expect_checkmate = false;
            if(terminator.contains("fold"))
            {
                expect_threefold_draw = true;
            }
            else if(terminator.contains("50"))
            {
                expect_fifty_move_draw = true;
            }
        }
        else if(word == "[FEN")
        {
            board = Board(get_pgn_header_value(input));
        }
        else if(word[0] == '[')
        {
            skip_rest_of_line(input);
        }
        else // Line contains game moves
        {
            in_game = true;
            if(word.back() == '.')
            {
                move_number = String::split(word, ".")[0] + ". ";
                continue;
            }

            if(board.whose_turn() == Piece_Color::BLACK)
            {
                move_number += "... ";
            }

            if((word == "1/2-1/2" && expected_winner != Winner_Color::NONE) ||
               (word == "1-0" && expected_winner != Winner_Color::WHITE) ||
               (word == "0-1" && expected_winner != Winner_Color::BLACK) ||
               (word == "*" && expected_winner != Winner_Color::NONE))
            {
                const auto line_count = line_number(input, input.tellg());
                std::cerr << "Final result mark (" << word << ") does not match game result. (line: " << line_count << ")\n";
                return false;
            }

            if(word == "1/2-1/2" || word == "1-0" || word == "0-1" || word == "*")
            {
                finished_game = true;
                continue;
            }

            try
            {
                const auto& move_to_play = board.interpret_move(word);
                if( ! check_rule_result("Move: " + move_number + word + ")",
                                        "capture",
                                        word.contains('x'),
                                        board.move_captures(move_to_play),
                                        input))
                {
                    return false;
                }

                result = board.play_move(move_to_play);

                if( ! check_rule_result("Move (" + move_number + word + ")",
                                        "check",
                                        std::string_view("+#").contains(word.back()),
                                        board.king_is_in_check(),
                                        input))
                {
                    return false;
                }

                if( ! check_rule_result("Move (" + move_number + word + ")",
                                        "checkmate",
                                        word.back() == '#',
                                        result.game_has_ended() && result.winner() != Winner_Color::NONE,
                                        input))
                {
                    return false;
                }
            }
            catch(const Illegal_Move&)
            {
                const auto line_count = line_number(input, input.tellg());
                std::cerr << "Move (" << move_number << word << ") is illegal" << " (line: " << line_count << ").\n";
                board.cli_print();
                std::cerr << "\nLegal moves: ";
                for(const auto legal_move : board.legal_moves())
                {
                    std::cerr << legal_move->algebraic(board) << " ";
                }
                std::cerr << '\n' << board.fen() << '\n';
                return false;
            }
        }
    }
}
