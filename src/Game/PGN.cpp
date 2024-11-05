#include "Game/PGN.h"

#include "Game/Board.h"
#include "Game/Move.h"
#include "Game/Game_Result.h"
#include "Game/Color.h"
#include "Game/Clock.h"

#include "Players/Player.h"

#include "Utility/Exceptions.h"
#include "Utility/String.h"

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <sstream>
#include <mutex>
#include <format>

namespace
{
    //! Print the line number of the input stream at the current position.
    int line_number(std::istream& input, std::streampos error_position) noexcept;

    void check_rule_result(const std::string& rule_source,
                           const std::string& rule_name,
                           const bool expected_ruling,
                           const bool actual_ruling,
                           std::istream& input)
    {
        if(expected_ruling != actual_ruling)
        {
            const auto line_count = line_number(input, input.tellg());
            throw PGN_Error(std::format("{} indicates {}{}, but last move did {}trigger rule (line: {}).",
                                        rule_source,
                                        expected_ruling ? "" : "no ",
                                        rule_name,
                                        actual_ruling ? "" : "not ",
                                        line_count));
        }
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

    //! \brief Skip to the end of the current curly-braced comment. Throws an exception if the end of the input stream is reached.
    //! 
    //! \param input A text input stream. The position within the input stream should be passed the opening curly brace.
    void skip_braced_comment(std::istream& input)
    {
        const auto stream_position = input.tellg();
        skip_passed_character(input, '}');
        if( ! input)
        {
            const auto line_count = line_number(input, stream_position);
            throw PGN_Error(std::format("Reached end of input before closing curly brace: line {}.", line_count));
        }
    }

    //! \brief Skip to the end of the current RAV section. Throws an exception if the end of the input stream is reached.
    //! 
    //! \param input A text input stream. The position within the input stream should be passed the opening parenthesis.
    void confirm_rav(std::istream& input, Board board)
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
                throw PGN_Error(std::format("Reached end of input before end of RAV starting at line {}.", line_count));
            }

            switch(c)
            {
                case '(':
                    if(word.empty())
                    {
                        confirm_rav(input, board_before_last_move);
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
                throw PGN_Error(std::format("Unable to parse token '{}' in RAV starting at line {}.", word, line_count));
            }

            if(c == ')')
            {
                return;
            }

            word.clear();
        }
    }

    void add_header_data(std::ifstream& input, std::map<std::string, std::string>& headers)
    {
        const auto brace_position = input.tellg();

        std::string tag_name;
        std::getline(input, tag_name, '"');
        tag_name = String::trim_outer_whitespace(tag_name);
        if(std::ranges::any_of(tag_name, String::isspace))
        {
            const auto line_count = line_number(input, brace_position);
            throw PGN_Error(std::format("Header tag name cannot contain spaces: {} (line: {})", tag_name, line_count));
        }

        if(headers.count(tag_name) != 0)
        {
            const auto line_count = line_number(input, brace_position);
            throw PGN_Error(std::format("Duplicate header tag name: {} (line: {})", tag_name, line_count));
        }

        std::string tag_value;
        std::getline(input, tag_value, '"');
        headers[tag_name] = String::remove_extra_whitespace(tag_value);
        skip_passed_character(input, ']');

        if( ! input)
        {
            const auto line_count = line_number(input, brace_position);
            throw PGN_Error(std::format("Malformed header tag (line: {})", line_count));
        }
    }
}

void PGN::confirm_game_record(const std::string& file_name)
{
    auto input = std::ifstream(file_name);
    if( ! input)
    {
        throw std::runtime_error(std::format("Could not open file {} for reading.", file_name));
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
                throw PGN_Error("File ended in middle of game.");
            }
            else
            {
                std::cout << "Found " << game_count << " " << (game_count == 1 ? "game" : "games") << ".\n";
                return;
            }
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
                skip_braced_comment(input);
            }
            else
            {
                saved_character = next_character;
            }
        }
        else if(next_character == '}')
        {
            const auto line_count = line_number(input, input.tellg());
            throw PGN_Error(std::format("Found closing curly brace before opener (line: {}).", line_count));
        }
        else if(next_character == '(')
        {
            if(word.empty())
            {
                confirm_rav(input, board_before_last_move);
            }
            else
            {
                saved_character = next_character;
            }
        }
        else if(next_character == ')')
        {
            const auto line_count = line_number(input, input.tellg());
            throw PGN_Error(std::format("Found closing RAV parentheses before opener (line: {})", line_count));
        }
        else if(in_game && next_character == '[')
        {
            const auto line_count = line_number(input, input.tellg());
            throw PGN_Error(std::format("Found header line in the middle of another game (line: {})", line_count));
        }
        else if(next_character == '[')
        {
            add_header_data(input, headers);
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
                throw PGN_Error(std::format("Malformed Result tag: {} (headers end at line: {})", result_value, line_count));
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
                throw PGN_Error(std::format("Final result mark ({}) does not match game result. (line: {})", token, line_count));
            }

            const auto final_board_result = result.game_ending_annotation();
            if(token != final_board_result)
            {
                const auto line_count = line_number(input, input.tellg());
                throw PGN_Error(std::format("Last move result ({}) on line {} does not match the game-ending tag ({}).",
                                            final_board_result, line_count, token));
            }

            check_rule_result("Header",
                              "50-move draw",
                              expect_fifty_move_draw,
                              String::contains(result.ending_reason(), "50"),
                              input);

            check_rule_result("Header",
                              "threefold draw",
                              expect_threefold_draw,
                              String::contains(result.ending_reason(), "fold"),
                              input);

            check_rule_result("Header",
                              "checkmate",
                              expect_checkmate,
                              String::contains(result.ending_reason(), "mates"),
                              input);

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
            move_number = std::format("{}. ", String::split(token, ".")[0]);
            continue;
        }

        if(board.whose_turn() == Piece_Color::BLACK)
        {
            move_number += "... ";
        }

        if( ! board.is_legal_move(token))
        {
            const auto line_count = line_number(input, input.tellg());
            auto message = std::ostringstream();
            message << "Move (" << move_number << token << ") is illegal" << " (line: " << line_count << ").\n";
            board.cli_print(message);
            message << "\nLegal moves: ";
            for(const auto legal_move : board.legal_moves())
            {
                message << legal_move->algebraic(board) << " ";
            }
            message << '\n' << board.fen() << '\n';
            throw PGN_Error(message.str());
        }

        const auto& move_to_play = board.interpret_move(token);
        const auto pgn_location = std::format("Move ({}{})", move_number, token);
        check_rule_result(pgn_location,
                          "capture",
                          String::contains(token, 'x'),
                          board.move_captures(move_to_play),
                          input);

        board_before_last_move = board;
        result = board.play_move(move_to_play);

        check_rule_result(pgn_location,
                          "check",
                          String::contains("+#", token.back()),
                          board.king_is_in_check(),
                          input);

        check_rule_result(pgn_location,
                          "checkmate",
                          token.back() == '#',
                          result.game_has_ended() && result.winner() != Winner_Color::NONE,
                          input);
    }
}

void PGN::print_game_record(const Board& board,
                            const std::vector<const Move*>& game_record_listing,
                            const Player& white,
                            const Player& black,
                            const std::string& file_name,
                            const Game_Result& result,
                            const Clock& game_clock,
                            const std::string& event_name,
                            const std::string& location) noexcept
{
    static std::mutex write_lock;
    const auto write_lock_guard = std::lock_guard(write_lock);

    static int game_number = 0;
    static std::string last_used_file_name;
    if(game_number == 0 || file_name != last_used_file_name)
    {
        game_number = 1;
        last_used_file_name = file_name;
        std::ifstream ifs(file_name);
        for(std::string line; std::getline(ifs, line);)
        {
            if(line.starts_with("[Round"))
            {
                const auto round_number = String::to_number<int>(String::extract_delimited_text(line, '"', '"'));
                if(round_number >= game_number)
                {
                    game_number = round_number + 1;
                }
            }
        }
    }

    auto header_text = std::ostringstream();

    PGN::print_game_header_line(header_text, "Event", event_name);
    PGN::print_game_header_line(header_text, "Site", location);
    PGN::print_game_header_line(header_text, "Date", String::date_and_time_format(game_clock.game_start_date_and_time(), "%Y.%m.%d"));
    PGN::print_game_header_line(header_text, "Round", game_number++);
    PGN::print_game_header_line(header_text, "White", white.name());
    PGN::print_game_header_line(header_text, "Black", black.name());

    const auto last_move_result = board.move_result();
    const auto& actual_result = last_move_result.game_has_ended() ? last_move_result : result;
    PGN::print_game_header_line(header_text, "Result", actual_result.game_ending_annotation());

    PGN::print_game_header_line(header_text, "Time", String::date_and_time_format(game_clock.game_start_date_and_time(), "%H:%M:%S"));

    PGN::print_game_header_line(header_text, "TimeControl", game_clock.time_control_string());
    PGN::print_game_header_line(header_text, "TimeLeftWhite", game_clock.time_left(Piece_Color::WHITE).count());
    PGN::print_game_header_line(header_text, "TimeLeftBlack", game_clock.time_left(Piece_Color::BLACK).count());

    if(!actual_result.ending_reason().empty() && !String::contains(actual_result.ending_reason(), "mates"))
    {
        PGN::print_game_header_line(header_text, "GameEnding", actual_result.ending_reason());
    }

    const auto starting_fen = board.original_fen();
    if(starting_fen != Board().fen())
    {
        PGN::print_game_header_line(header_text, "SetUp", 1);
        PGN::print_game_header_line(header_text, "FEN", starting_fen);
    }

    auto game_text = std::ostringstream();
    auto commentary_board = Board(starting_fen);
    auto previous_move_had_comment = false;
    for(const auto next_move : game_record_listing)
    {
        if(commentary_board.whose_turn() == Piece_Color::WHITE || commentary_board.played_ply_count() == 0 || previous_move_had_comment)
        {
            const auto step = commentary_board.all_ply_count() / 2 + 1;
            game_text << " " << step << ".";
            if(commentary_board.whose_turn() == Piece_Color::BLACK)
            {
                game_text << "..";
            }
        }

        game_text << " " << next_move->algebraic(commentary_board);
        const auto& current_player = (commentary_board.whose_turn() == Piece_Color::WHITE ? white : black);
        const auto commentary = String::trim_outer_whitespace(current_player.commentary_for_next_move(commentary_board));
        if(!commentary.empty())
        {
            game_text << " " << commentary;
        }
        commentary_board.play_move(*next_move);
        previous_move_had_comment = ! commentary.empty();
    }
    game_text << " " << actual_result.game_ending_annotation();

    const auto pgn_text = std::format("{}\n{}\n\n\n", header_text.str(), String::word_wrap(game_text.str(), 80));

    if(file_name.empty())
    {
        std::cout << pgn_text;
    }
    else
    {
        std::ofstream ofs(file_name, std::ios::app);
        ofs << pgn_text;
    }

    assert(commentary_board.fen() == board.fen());
}
