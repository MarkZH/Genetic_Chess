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
        char saved_character = 0;

        while(true)
        {
            const auto c = saved_character ? saved_character : char(input.get());
            saved_character = 0;

            if( ! input)
            {
                const auto line_count = line_number(input, rav_start_position);
                std::cerr << "Reached end of input before end of RAV starting at line " << line_count << ".\n";
                return false;
            }

            switch(c)
            {
                case '(':
                    if(word.empty())
                    {
                        if( ! confirm_rav(input, board_before_last_move))
                        {
                            return false;
                        }
                    }
                    else
                    {
                        saved_character = c;
                    }
                    break;
                case ';':
                    if(word.empty())
                    {
                        skip_rest_of_line(input);
                    }
                    else
                    {
                        saved_character = c;
                    }
                    break;
                case '{':
                    if(word.empty())
                    {
                        skip_braced_comment(input);
                    }
                    else
                    {
                        saved_character = c;
                    }
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

            if(word.empty() || std::isdigit(word.front()))
            {
                // Do nothing (is empty or is a move number)
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

            if(c == ')')
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
        if(std::ranges::any_of(tag_name, String::isspace))
        {
            const auto line_count = line_number(input, brace_position);
            std::cerr << "Header tag name cannot contain spaces: " << tag_name << " (line: " << line_count << ")\n";
            return false;
        }

        if(headers.count(tag_name) != 0)
        {
            const auto line_count = line_number(input, brace_position);
            std::cerr << "Duplicate header tag name: " << tag_name << " (line: " << line_count << ")\n";
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
    std::string word;
    char saved_character = 0;

    const auto valid_result_marks = {"1/2-1/2", "1-0", "0-1", "*"};

    auto expect_checkmate = true;
    auto expect_fifty_move_draw = false;
    auto expect_threefold_draw = false;
    auto in_game = false;
    std::map<std::string, std::string> headers;
    Board board;
    Board board_before_last_move;
    Game_Result result;
    while(true)
    {
        const auto next_character = saved_character ? saved_character : char(input.get());
        saved_character = 0;
        if( ! input && word.empty())
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
        }
        else if(next_character == ';')
        {
            if(word.empty())
            {
                skip_rest_of_line(input);
            }
            else
            {
                saved_character = next_character;
            }
        }
        else if(next_character == '{')
        {
            if(word.empty())
            {
                if( ! skip_braced_comment(input))
                {
                    return false;
                }
            }
            else
            {
                saved_character = next_character;
            }
        }
        else if(next_character == '}')
        {
            const auto line_count = line_number(input, input.tellg());
            std::cerr << "Found closing curly brace before opener (line: " << line_count << ")\n";
            return false;
        }
        else if(next_character == '(')
        {
            if(word.empty())
            {
                if( ! confirm_rav(input, board_before_last_move))
                {
                    return false;
                }
            }
            else
            {
                saved_character = next_character;
            }
        }
        else if(next_character == ')')
        {
            const auto line_count = line_number(input, input.tellg());
            std::cerr << "Found closing RAV parentheses before opener (line: " << line_count << ")\n";
            return false;
        }
        else if(in_game && next_character == '[')
        {
            const auto line_count = line_number(input, input.tellg());
            std::cerr << "Found header line in the middle of another game (line: " << line_count << ")\n";
            return false;
        }
        else if(next_character == '[')
        {
            if( ! add_header_data(input, headers))
            {
                return false;
            }
        }
        else if(input)
        {
            word.push_back(next_character);
            continue;
        }

        if(word.empty())
        {
            continue;
        }

        if( ! in_game)
        {
            const auto result_value = headers["Result"];
            if(std::ranges::find(valid_result_marks, result_value) == valid_result_marks.end())
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

        const auto token = word;
        word.clear();

        if(std::ranges::find(valid_result_marks, token) != valid_result_marks.end())
        {
            if(token != headers["Result"])
            {
                const auto line_count = line_number(input, input.tellg());
                std::cerr << "Final result mark (" << token << ") does not match game result. (line: " << line_count << ")\n";
                return false;
            }

            const auto final_board_result = result.game_ending_annotation();
            if(token != final_board_result)
            {
                const auto line_count = line_number(input, input.tellg());
                std::cerr << "Last move result (" << final_board_result << ") on line " << line_count
                          << " does not match the game-ending tag (" << token << ").\n";
                return false;
            }

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

            expect_checkmate = true;
            expect_fifty_move_draw = false;
            expect_threefold_draw = false;
            in_game = false;
            headers.clear();
            board = Board();
            board_before_last_move = Board();
            result = {};
            ++game_count;
            continue;
        }

        if(std::isdigit(token.front()))
        {
            move_number = String::split(token, ".")[0] + ". ";
            continue;
        }

        if(board.whose_turn() == Piece_Color::BLACK)
        {
            move_number += "... ";
        }

        if( ! board.is_legal_move(token))
        {
            const auto line_count = line_number(input, input.tellg());
            std::cerr << "Move (" << move_number << token << ") is illegal" << " (line: " << line_count << ").\n";
            board.cli_print(std::cerr);
            std::cerr << "\nLegal moves: ";
            for(const auto legal_move : board.legal_moves())
            {
                std::cerr << legal_move->algebraic(board) << " ";
            }
            std::cerr << '\n' << board.fen() << '\n';
            return false;
        }

        const auto& move_to_play = board.interpret_move(token);
        if( ! check_rule_result("Move: " + move_number + token + ")",
                                "capture",
                                String::contains(token, 'x'),
                                board.move_captures(move_to_play),
                                input))
        {
            return false;
        }

        board_before_last_move = board;
        result = board.play_move(move_to_play);

        if( ! check_rule_result("Move (" + move_number + token + ")",
                                "check",
                                String::contains("+#", token.back()),
                                board.king_is_in_check(),
                                input))
        {
            return false;
        }

        if( ! check_rule_result("Move (" + move_number + token + ")",
                                "checkmate",
                                token.back() == '#',
                                result.game_has_ended() && result.winner() != Winner_Color::NONE,
                                input))
        {
            return false;
        }
    }
}
