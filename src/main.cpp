#include <iostream>
#include <memory>
#include <fstream>
#include <cctype>
#include <algorithm>
#include <string>
#include <stdexcept>
#include <utility>
#include <vector>
#include <limits>

#include "Game/Game.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Game_Result.h"
#include "Moves/Move.h"

#include "Players/Minimax_AI.h"
#include "Players/Random_AI.h"

#include "Genes/Gene_Pool.h"

#include "Utility/Exceptions.h"
#include "Utility/String.h"
#include "Utility/Help_Writer.h"

#include "Testing.h"

//! \file

namespace
{
    //! \brief Print the command-line options for this program.
    void print_help();

    //! \brief Skip ahead until the first non-whitespace character.
    //! 
    //! \param input A text input stream.
    //! \param line_number A line counter that is incremented if a newline is encountered.
    void skip_whitespace(std::istream& input, int& line_number) noexcept;

    //! \brief Skip to the end of the current line.
    //! 
    //! \param input A text input stream.
    //! \param line_number A line number counter that is incremented when the stream reaches the next line.
    void skip_rest_of_line(std::istream& input, int& line_number) noexcept;

    //! \brief Skip to the end of the current curly-braced comment. Returns true if successful, and false if the end of the input stream is reached.
    //! 
    //! \param input A text input stream.
    //! \param line_number A line counter that is incremented if a newline is encountered.
    bool skip_braced_comment(std::istream& input, int& line_number) noexcept;

    //! \brief Skip to the end of the current RAV section. Returns true if successful, and false if the end of the input stream is reached.
    //! 
    //! \param input A text input stream.
    //! \param line_number A line counter that is incremented if a newline is encountered.
    bool skip_passed_rav(std::istream& input, int& line_number) noexcept;

    //! \brief Confirm that all moves in a PGN game record are legal moves.
    //!
    //! \param file_name The name of the file with the PGN game records. All games will be examined.
    bool confirm_game_record(const std::string& file_name);

    //! \brief Starts a single game according to command line options.
    //!
    //! \param argc The number of command line options (same as for main(int argc, char *argv[])).
    //! \param argv The command line options (same as for main(int argc, char *argv[])).
    void start_game(const std::vector<std::string>& options);

    //! \brief List all move combinations to a given depth
    //! \param depth The maximum depth to search for moves.
    //!
    //! Prints results to stdout.
    void list_moves(size_t depth) noexcept;

    //! \brief Throws std::invalid_argument if assertion fails
    //!
    //! \param condition A condition that must be true to continue.
    //! \param failure_message A message to display if the assertion fails.
    void argument_assert(bool condition, const std::string& failure_message);
}

//! \brief The starting point for the whole program.
//!
//! \param argc The number of command-line arguments.
//! \param argv The command-line arguments. See print_help() (or run the program
//!        with no arguments or with -help) for a listing of all the options.
//! \returns EXIT_SUCCESS or EXIT_FAILURE.
int main(int argc, char *argv[])
{
    std::vector<std::string> options;
    for(int i = 1; i < argc; ++i)
    {
        if(String::starts_with(argv[i], "-"))
        {
            auto parts = String::split(argv[i], "=", 1);
            if(String::starts_with(parts.front(), "--"))
            {
                parts.front() = parts.front().substr(1);
            }
            options.insert(options.end(), parts.begin(), parts.end());
        }
        else
        {
            options.push_back(argv[i]);
        }
    }

    if(options.empty())
    {
        options.push_back("-help");
    }

    try
    {
        const auto option = options.front();
        if(option == "-gene-pool")
        {
            argument_assert(options.size() >= 2, "Specify a configuration file to run a gene pool.");
            gene_pool(options[1]);
        }
        else if(option == "-confirm")
        {
            argument_assert(options.size() >= 2, "Provide a file containing a game to confirm has all legal moves.");
            return confirm_game_record(options[1]) ? EXIT_SUCCESS : EXIT_FAILURE;
        }
        else if(option == "-test")
        {
            return run_tests() ? EXIT_SUCCESS : EXIT_FAILURE;
        }
        else if(option == "-speed")
        {
            run_speed_tests();
        }
        else if(option == "-perft")
        {
            return run_perft_tests() ? EXIT_SUCCESS : EXIT_FAILURE;
        }
        else if(option == "-list")
        {
            argument_assert(options.size() >= 2, option + " requires a numeric argument.");
            list_moves(String::to_number<size_t>(options[1]));
        }
        else if(option == "-help")
        {
            print_help();
        }
        else
        {
            start_game(options);
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << "\nERROR: " << e.what() << '\n';
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

namespace
{
    void print_help()
    {
        Help_Writer help;
        help.add_title("Genetic Chess");
        help.add_paragraph("Genetic Chess is a program that runs a chess engine to play games or run a gene pool for genetically optimizing the chess engine.");
        help.add_section_title("Command-line Arguments");
        help.add_paragraph("For all arguments, all of the following forms are equivalent:");
        help.add_option("-arg param");
        help.add_option("-arg=param");
        help.add_option("--arg param");
        help.add_option("--arg=param");
        help.add_paragraph("where arg is the name of the argument and param is a single required parameter.");
        help.add_paragraph("Parameters in [square brackets] are required, while parameters in <angle~brackets> are optional.");
        help.add_section_title("Standalone functions");
        help.add_paragraph("These functions are for single program actions. If multiple of these options are specified, only first is run before exiting.");
        help.add_option("-help", "Print this help text and exit.");
        help.add_option("-gene-pool", {"file name"}, "Start a run of a gene pool with parameters from the given file.");
        help.add_option("-confirm", {"file name"}, "Check a file containing PGN game records for any illegal moves or mismarked checks or checkmates.");
        help.add_option("-test", "Run tests to ensure various parts of the program function correctly.");
        help.add_option("-speed", "Run a speed test for gene scoring and board move submission.");
        help.add_option("-perft", "Run a legal move generation test.");
        help.add_section_title("Player options");
        help.add_paragraph("The following options start a game with various players. If two players are specified, the first plays white and the second black. If only one player is specified, the program will wait for an Xboard or UCI command from a GUI to start playing.");
        help.add_option("-genetic", {"file name"}, {"ID number"}, "Select a minimaxing evolved player for a game and load data from the file. If there are multiple genomes in the file, specify an ID number to load, otherwise the last genome in the file will be used.");
        help.add_option("-random", "Select a player that makes random moves for a game.");
        help.add_section_title("Other game options");
        help.add_option("-time", {"number"}, "Specify the time (in seconds) each player has to play the game or to make a set number of moves (see -reset_moves option).");
        help.add_option("-reset-moves", {"number"}, "Specify the number of moves a player must make within the time limit. The clock adds the initial time every time this number of moves is made.");
        help.add_option("-increment-time", {"number"}, "Specify seconds to add to time after each move.");
        help.add_option("-board", {"FEN string"}, "Specify the starting board state using FEN notation. The entire string should be quoted.");
        help.add_option("-event", {"name"}, "An optional name for the game to be played. This name will be written to the PGN game record in the Event tag.");
        help.add_option("-location", {"name"}, "An optional location for the game to be played. This will be written to the PGN game record in the Location tag.");
        help.add_option("-game-file", {"file name"}, "Specify the name of the file where the game record should be written. If none, record is printed to stdout.");
        help.add_option("-uci");
        help.add_option("-xboard", "Show an engine's thinking output in either UCI or Xboard format.");
        help.add_option("-show-board", "Show the board on the command line when playing a local game.");
        help.add_option("-log-comms", "Log UCI/Xboard communications (except engine thinking) to a file.");
        help.add_paragraph("All game options in this section can be overriden by GUI commands except -event, -location, and -game-file.");

        std::cout << help;
    }

    bool check_rule_result(const std::string& rule_source,
                           const std::string& rule_name,
                           const bool expected_ruling,
                           const bool actual_ruling,
                           const int last_move_line_number) noexcept
    {
        const auto pass = expected_ruling == actual_ruling;
        if( ! pass)
        {
            std::cerr << rule_source << " indicates "
                      << (expected_ruling ? "" : "no ")
                      << rule_name << ", but last move did "
                      << (actual_ruling ? "" : "not ")
                      << "trigger rule (line: " << last_move_line_number << ").\n";
        }

        return pass;
    }

    std::string get_pgn_header_value(std::istream& is, int& line_number) noexcept
    {
        std::string rest;
        std::getline(is, rest);
        ++line_number;
        return String::extract_delimited_text(rest, "\"", "\"");
    }

    void skip_whitespace(std::istream& input, int& line_number) noexcept
    {
        while(std::isspace(input.peek()))
        {
            if(input.get() == '\n')
            {
                ++line_number;
            }
        }
    }

    void skip_rest_of_line(std::istream& input, int& line_number) noexcept
    {
        input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        ++line_number;
    }

    bool skip_braced_comment(std::istream& input, int& line_number) noexcept
    {
        const auto brace_comment_start_line_number = line_number;
        while(true)
        {
            const auto c = input.get();
            if( ! input)
            {
                std::cerr << "Unclosed commentary curly brace: line " << brace_comment_start_line_number << ".\n";
                return false;
            }

            if(c == '}')
            {
                break;
            }

            if(c == '\n')
            {
                ++line_number;
            }
        }

        return true;
    }

    bool skip_passed_rav(std::istream& input, int& line_number) noexcept
    {
        const auto rav_start_line_number = line_number;
        input.get();
        auto rav_depth = 1;

        while(rav_depth > 0)
        {
            const auto c = input.get();
            if(!input)
            {
                std::cerr << "Reached end of input before end of RAV: line " << rav_start_line_number << ".\n";
                return false;
            }

            if(c == '(')
            {
                ++rav_depth;
            }
            else if(c == ')')
            {
                --rav_depth;
            }
            else if(c == '\n')
            {
                ++line_number;
            }
            else if(c == ';')
            {
                skip_rest_of_line(input, line_number);
            }
            else if(c == '{')
            {
                if( ! skip_braced_comment(input, line_number))
                {
                    return false;
                }
            }
        }

        return true;
    }

    bool confirm_game_record(const std::string& file_name)
    {
        auto input = std::ifstream(file_name);
        if( ! input)
        {
            throw std::runtime_error("Could not open file " + file_name + " for reading.");
        }

        auto line_number = 1;
        auto last_move_line_number = 0;
        std::string move_number;

        auto expected_winner = Winner_Color::NONE;
        auto expect_checkmate = true;
        auto expect_fifty_move_draw = false;
        auto expect_threefold_draw = false;
        auto in_game = false;
        Board board;
        Game_Result result;
        while(true)
        {
            skip_whitespace(input, line_number);

            if(input.peek() == ';')
            {
                skip_rest_of_line(input, line_number);
                continue;
            }

            if(input.peek() == '{')
            {
                if(skip_braced_comment(input, line_number))
                {
                    continue;
                }
                else
                {
                    return false;
                }
            }

            if(input.peek() == '}')
            {
                std::cerr << "Malformed commentary curly brace (line: " << line_number << ")\n";
                return false;
            }

            if(input.peek() == '(')
            {
                if(skip_passed_rav(input, line_number))
                {
                    continue;
                }
                else
                {
                    return false;
                }
            }

            if(input.peek() == ')')
            {
                std::cerr << "Malformed RAV parentheses (line: " << line_number << ")\n";
                return false;
            }

            // Start header of new game
            if(in_game && input.peek() == '[')
            {
                if( ! check_rule_result("Header",
                                        "50-move draw",
                                        expect_fifty_move_draw,
                                        String::contains(result.ending_reason(), "50"),
                                        last_move_line_number))
                {
                    return false;
                }

                if( ! check_rule_result("Header",
                                        "threefold draw",
                                        expect_threefold_draw,
                                        String::contains(result.ending_reason(), "fold"),
                                        last_move_line_number))
                {
                    return false;
                }

                if( ! check_rule_result("Header",
                                        "checkmate",
                                        expect_checkmate,
                                        String::contains(result.ending_reason(), "mates"),
                                        last_move_line_number))
                {
                    return false;
                }

                move_number = {};

                expected_winner = Winner_Color::NONE;
                expect_checkmate = true;
                expect_fifty_move_draw = false;
                expect_threefold_draw = false;
                in_game = false;
                board = Board();
                result = {};
            }

            std::string word;
            if( ! (input >> word))
            {
                break;
            }

            if(word == "[Result")
            {
                const auto result_tag = get_pgn_header_value(input, line_number);
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
                    std::cerr << "Malformed Result: " << word << " " << result_tag << " (line: " << line_number << ")\n";
                    return false;
                }
            }
            else if(word == "[Termination")
            {
                const auto terminator = get_pgn_header_value(input, line_number);
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
            else if(word == "[FEN")
            {
                board = Board(get_pgn_header_value(input, line_number));
            }
            else if(word[0] == '[')
            {
                skip_rest_of_line(input, line_number);
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
                    std::cerr << "Final result mark (" << word << ") does not match game result. (line: " << line_number << ")\n";
                    return false;
                }

                if(word == "1/2-1/2" || word == "1-0" || word == "0-1" || word == "*")
                {
                    continue;
                }

                try
                {
                    const auto& move_to_play = board.interpret_move(word);
                    last_move_line_number = line_number;
                    if( ! check_rule_result("Move: " + move_number + word + ")",
                                            "capture",
                                            String::contains(word, 'x'),
                                            board.move_captures(move_to_play),
                                            last_move_line_number))
                    {
                        return false;
                    }

                    result = board.play_move(move_to_play);

                    if( ! check_rule_result("Move (" + move_number + word + ")",
                                            "check",
                                            String::contains("+#", word.back()),
                                            board.king_is_in_check(),
                                            last_move_line_number))
                    {
                        return false;
                    }

                    if( ! check_rule_result("Move (" + move_number + word + ")",
                                            "checkmate",
                                            word.back() == '#',
                                            result.game_has_ended() && result.winner() != Winner_Color::NONE,
                                            last_move_line_number))
                    {
                        return false;
                    }
                }
                catch(const Illegal_Move&)
                {
                    std::cerr << "Move (" << move_number << word << ") is illegal."
                                << " (line: " << line_number << ")\n";
                    std::cerr << "Legal moves: ";
                    for(const auto legal_move : board.legal_moves())
                    {
                        std::cerr << legal_move->algebraic(board) << " ";
                    }
                    std::cerr << '\n' << board.fen() << '\n';
                    return false;
                }
            }
        }

        return true;
    }

    void start_game(const std::vector<std::string>& options)
    {
        // Use pointers since each player could be Genetic, Random, etc.
        std::unique_ptr<Player> white;
        std::unique_ptr<Player> black;

        Clock::seconds game_time{};
        size_t moves_per_reset = 0;
        Clock::seconds increment_time{};
        Board board;
        std::string game_file_name;
        std::string event_name;
        std::string location;
        auto thinking_output = Thinking_Output_Type::NO_THINKING;
        auto print_board = false;
        auto enable_logging = false;

        for(size_t i = 0; i < options.size(); ++i)
        {
            const std::string opt = options[i];
            std::unique_ptr<Player> latest;
            if(opt == "-random")
            {
                latest = std::make_unique<Random_AI>();
            }
            else if(opt == "-genetic")
            {
                argument_assert(i + 1 < options.size(), "Genome file needed for player");
                std::string file_name = options[++i];

                try
                {
                    const auto id = i + 1 < options.size() ? options[i + 1] : std::string{};
                    latest = std::make_unique<Minimax_AI>(file_name, String::to_number<int>(id));
                    ++i;
                }
                catch(const std::invalid_argument&) // Could not convert id to an int.
                {
                    latest = std::make_unique<Minimax_AI>(file_name, find_last_id(file_name));
                }
            }
            else if(opt == "-time" && i + 1 < options.size())
            {
                game_time = String::to_duration<Clock::seconds>(options[++i]);
            }
            else if(opt == "-reset-moves" && i + 1 < options.size())
            {
                moves_per_reset = String::to_number<size_t>(options[++i]);
            }
            else if(opt == "-increment-time" && i + 1 < options.size())
            {
                increment_time = String::to_duration<Clock::seconds>(options[++i]);
            }
            else if(opt == "-board" && i + 1 < options.size())
            {
                board = Board(options[++i]);
            }
            else if(opt == "-game-file" && i + 1 < options.size())
            {
                game_file_name = options[++i];
            }
            else if(opt == "-event" && i + 1 < options.size())
            {
                event_name = options[++i];
            }
            else if(opt == "-location" && i + 1 < options.size())
            {
                location = options[++i];
            }
            else if(opt == "-xboard")
            {
                thinking_output = Thinking_Output_Type::XBOARD;
            }
            else if(opt == "-uci")
            {
                thinking_output = Thinking_Output_Type::UCI;
            }
            else if(opt == "-show-board")
            {
                print_board = true;
            }
            else if(opt == "-log-comms")
            {
                enable_logging = true;
            }
            else
            {
                throw std::invalid_argument("Invalid or incomplete game option: " + opt);
            }

            if(latest)
            {
                if( ! white)
                {
                    white = std::move(latest);
                }
                else if( ! black)
                {
                    black = std::move(latest);
                }
                else
                {
                    throw std::invalid_argument("More than two players specified.");
                }
            }
        }

        if( ! white)
        {
            throw std::invalid_argument("At least one player must be specified.");
        }

        if( ! black)
        {
            play_game_with_outsider(*white, event_name, location, game_file_name, enable_logging);
        }
        else
        {
            Player::set_thinking_mode(thinking_output);
            play_game(board,
                      Clock(game_time, moves_per_reset, increment_time, Time_Reset_Method::ADDITION),
                      *white, *black,
                      event_name,
                      location,
                      game_file_name,
                      print_board);
        }
    }

    void list_moves_on_board(const Board& board, std::vector<const Move*>& moves_played, size_t depth) noexcept
    {
        if(depth == 0 || board.legal_moves().empty())
        {
            for(const auto move : moves_played)
            {
                std::cout << move->coordinates() << ' ';
            }
            std::cout << '\n';
            return;
        }

        for(const auto move : board.legal_moves())
        {
            moves_played.push_back(move);
            auto new_board = board;
            new_board.play_move(*move);
            list_moves_on_board(new_board, moves_played, depth - 1);
            moves_played.pop_back();
        }
    }

    void list_moves(const size_t depth) noexcept
    {
        std::vector<const Move*> moves_played;
        Board board;
        list_moves_on_board(board, moves_played, depth);
    }

    void argument_assert(const bool condition, const std::string& failure_message)
    {
        if( ! condition)
        {
            throw std::invalid_argument(failure_message);
        }
    }
}
