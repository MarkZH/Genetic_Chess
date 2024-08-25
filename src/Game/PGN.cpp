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
#include <map>

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

    //! \brief Skip passed the given character
    //! 
    //! \param input A text input stream.
    //! \param stop_character The text character to search for and advance beyond.
    void skip_passed_character(std::istream& input, const char stop_character) noexcept
    {
        input.ignore(std::numeric_limits<std::streamsize>::max(), stop_character);
    }

    //! \brief Skip to the end of the current line.
    //! 
    //! \param input A text input stream.
    void skip_rest_of_line(std::istream& input) noexcept
    {
        skip_passed_character(input, '\n');
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
        skip_passed_character(input, '}');
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
    bool confirm_rav(std::istream& input, Board board) noexcept
    {
        const auto rav_start_position = input.tellg();
        auto token_start = input.tellg();
        auto board_before_last_move = board;
        std::string word;

        while(true)
        {
            const auto c = char(input.get());

            if( ! input)
            {
                const auto line_count = line_number(input, rav_start_position);
                std::cerr << "Reached end of input before end of RAV starting at line " << line_count << ".\n";
                return false;
            }

            const auto rav_is_complete = (c == ')');

            switch(c)
            {
                case '(':
                    if( ! confirm_rav(input, board_before_last_move))
                    {
                        return false;
                    }
                    break;
                case ';':
                    skip_rest_of_line(input);
                    break;
                case '{':
                    skip_braced_comment(input);
                    break;
                case ')':
                case ' ':
                case '\t':
                case '\n':
                    break;
                default:
                    if(word.empty())
                    {
                        token_start = input.tellg();
                    }
                    word.push_back(c);
                    continue;
            }

            if(word.empty())
            {
                if(rav_is_complete)
                {
                    return true;
                }
                else
                {
                    continue;
                }
            }
            else if(String::contains(word, "."))
            {
                // Move number, i.e., 1.
            }
            else if(board.is_legal_move(word))
            {
                board_before_last_move = board;
                board.play_move(word);
            }
            else
            {
                const auto line_count = line_number(input, token_start);
                std::cerr << "Unable to parse token '" << word << "' in RAV starting at line " << line_count << ".\n";
                return false;
            }

            if(rav_is_complete)
            {
                return true;
            }

            word.clear();
        }
    }

    bool add_header_data(std::ifstream& input, std::map<std::string, std::string>& headers) noexcept
    {
        const auto brace_position = input.tellg();

        std::string tag_name;
        std::getline(input, tag_name, '"');
        tag_name = String::trim_outer_whitespace(tag_name);
        if(tag_name != String::remove_extra_whitespace(tag_name))
        {
            const auto line_count = line_number(input, brace_position);
            std::cerr << "Header tag name cannot contain spaces (line: " << line_count << ")\n";
            return false;
        }

        std::string tag_value;
        std::getline(input, tag_value, '"');
        headers[tag_name] = String::remove_extra_whitespace(tag_value);
        skip_passed_character(input, ']');

        if( ! input)
        {
            const auto line_count = line_number(input, brace_position);
            std::cerr << "Malformed header tag (line: " << line_count << ")\n";
            return false;
        }

        return true;
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

    const auto valid_result_marks = {"1/2-1/2", "1-0", "0-1", "*"};

    auto expect_checkmate = true;
    auto expect_fifty_move_draw = false;
    auto expect_threefold_draw = false;
    auto in_game = false;
    auto finished_game = false;
    std::map<std::string, std::string> headers;
    Board board;
    Board board_before_last_move;
    Game_Result result;
    while(true)
    {
        if(finished_game)
        {
            if( ! check_rule_result("Header",
                                    "50-move draw",
                                    expect_fifty_move_draw,
                                    String::contains(result.ending_reason(), "50"),
                                    input))
            {
                return false;
            }

            if( ! check_rule_result("Header",
                                    "threefold draw",
                                    expect_threefold_draw,
                                    String::contains(result.ending_reason(), "fold"),
                                    input))
            {
                return false;
            }

            if( ! check_rule_result("Header",
                                    "checkmate",
                                    expect_checkmate,
                                    String::contains(result.ending_reason(), "mates"),
                                    input))
            {
                return false;
            }

            move_number = {};

            expect_checkmate = true;
            expect_fifty_move_draw = false;
            expect_threefold_draw = false;
            in_game = false;
            finished_game = false;
            headers.clear();
            board = Board();
            board_before_last_move = Board();
            result = {};
            ++game_count;
        }

        const auto next_character = char(input.get());
        if( ! input)
        {
            if(in_game)
            {
                std::cerr << "File ended in middle of game.\n";
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
            if(confirm_rav(input, board_before_last_move))
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

        if(next_character == '[')
        {
            if( ! add_header_data(input, headers))
            {
                return false;
            }

            continue;
        }

        if( ! in_game)
        {
            const auto result_value = headers["Result"];
            if(std::find(valid_result_marks.begin(), valid_result_marks.end(), result_value) == valid_result_marks.end())
            {
                const auto line_count = line_number(input, input.tellg());
                std::cerr << "Malformed Result tag: " << result_value << " (headers end at line: " << line_count << ")\n";
                return false;
            }

            if(result_value == "1/2-1/2" || result_value == "*")
            {
                expect_checkmate = false;
            }

            const auto terminator = headers["GameEnding"];
            if( ! terminator.empty())
            {
                expect_checkmate = false;
                if(String::contains(terminator, "fold"))
                {
                    expect_threefold_draw = true;
                }
                else if(String::contains(terminator, "50"))
                {
                    expect_fifty_move_draw = true;
                }
            }

            const auto fen = headers["FEN"];
            if( ! fen.empty())
            {
                board = Board(fen);
            }
        }

        in_game = true;

        std::string word;
        input >> word;
        word = next_character + word;

        if(word.back() == '.')
        {
            move_number = String::split(word, ".")[0] + ". ";
            continue;
        }

        if(board.whose_turn() == Piece_Color::BLACK)
        {
            move_number += "... ";
        }

        if(std::find(valid_result_marks.begin(), valid_result_marks.end(), word) != valid_result_marks.end())
        {
            if(word != headers["Result"])
            {
                const auto line_count = line_number(input, input.tellg());
                std::cerr << "Final result mark (" << word << ") does not match game result. (line: " << line_count << ")\n";
                return false;
            }

            const auto final_board_result = result.game_ending_annotation();
            if(word != final_board_result)
            {
                const auto line_count = line_number(input, input.tellg());
                std::cerr << "Last move result (" << final_board_result << ") on line " << line_count
                          << " does not match the game-ending tag (" << word << ").\n";
                return false;
            }

            finished_game = true;
            continue;
        }

        try
        {
            const auto& move_to_play = board.interpret_move(word);
            if( ! check_rule_result("Move: " + move_number + word + ")",
                                    "capture",
                                    String::contains(word, 'x'),
                                    board.move_captures(move_to_play),
                                    input))
            {
                return false;
            }

            board_before_last_move = board;
            result = board.play_move(move_to_play);

            if( ! check_rule_result("Move (" + move_number + word + ")",
                                    "check",
                                    String::contains("+#", word.back()),
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
            board.cli_print(std::cerr);
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
