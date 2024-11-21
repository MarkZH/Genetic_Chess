#include "Genes/Gene_Pool.h"

#include <vector>
#include <iostream>
#include <map>
#include <iomanip>
#include <csignal>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <future>
#include <thread>
#include <chrono>
using namespace std::chrono_literals;
#include <array>
#include <cstdio>
#include <filesystem>
#include <string>
#include <numeric>
#include <sstream>
#include <semaphore>
#include <format>

#include "Players/Genetic_AI.h"

#include "Game/Game.h"
#include "Game/Board.h"
#include "Game/Clock.h"
#include "Game/Game_Result.h"

#include "Utility/String.h"
#include "Utility/Configuration.h"
#include "Utility/Random.h"
#include "Utility/Exceptions.h"

namespace
{
    const std::string quit_key = "Ctrl-c";
    const auto QUIT_SIGNAL = SIGINT;
    std::atomic_bool quit_after_round = false;
    bool keep_going(const Clock& stop_time);
    std::atomic_int space_counter = 0;
    void quit_gene_pool(int);

    Clock get_pool_clock(const Configuration& config);
    std::chrono::duration<double> get_start_delay(const Configuration& config);
    std::chrono::duration<double> get_duration(const std::string& parameter);
    std::string future_timestamp(const std::chrono::duration<double>& duration) noexcept;

    std::vector<Genetic_AI> load_gene_pool_file(const std::string& load_file);
    [[noreturn]] void throw_on_bad_still_alive_line(size_t line_number, const std::string& line);

    void record_the_living(const std::vector<Genetic_AI>& pool, const std::string& genome_file_name);

    size_t count_still_alive_lines(const std::string& genome_file_name) noexcept;
    std::vector<Genetic_AI> fill_pool(const std::string& genome_file_name, size_t gene_pool_population, size_t mutation_rate);
    void load_previous_game_stats(const std::string& game_record_file, Clock::seconds& game_time, std::array<size_t, 3>& color_wins);
    Game_Result pool_game(const Board& board,
                          const Clock::seconds game_time,
                          Genetic_AI white,
                          Genetic_AI black,
                          const std::string& game_record_file,
                          std::counting_semaphore<>& limiter) noexcept;
    Genetic_AI best_living_ai(const std::vector<Genetic_AI>& pool) noexcept;
    void record_best_ai(const std::vector<Genetic_AI>& pool, const std::string& best_file_name) noexcept;
    void print_round_header(const std::vector<Genetic_AI>& pool,
                            const std::string& genome_file_name,
                            const std::array<size_t, 3>& color_wins,
                            size_t round_count,
                            size_t first_mutation_interval,
                            size_t second_mutation_interval,
                            size_t mutation_rate,
                            Clock::seconds game_time,
                            const Clock& pool_time) noexcept;
    void print_verbose_output(const std::stringstream& result_printer, const std::vector<Genetic_AI>& pool);
}

void gene_pool(const std::string& config_file)
{
    const auto config = Configuration(config_file);
    const auto maximum_simultaneous_games = config.as_positive_number<int>("maximum simultaneous games");
    const auto gene_pool_population = config.as_positive_number<size_t>("gene pool population");
    if(gene_pool_population % 2 != 0)
    {
        throw std::invalid_argument("Gene pool population must be even so every AI plays every round.");
    }
    const auto genome_file_name = config.as_text("gene pool file");
    if(genome_file_name.empty())
    {
        throw std::invalid_argument("Gene pool file name cannot be blank.");
    }

    const auto first_mutation_rate = config.as_positive_number<size_t>("first mutation rate");
    const auto first_mutation_interval = config.as_positive_number<size_t>("first mutation interval");
    const auto second_mutation_rate = config.as_positive_number<size_t>("second mutation rate");
    const auto second_mutation_interval = config.as_positive_number<size_t>("second mutation interval");

    const auto minimum_game_time = config.as_positive_time_duration<Clock::seconds>("minimum game time");
    const auto maximum_game_time = config.as_positive_time_duration<Clock::seconds>("maximum game time");
    if(maximum_game_time < minimum_game_time)
    {
        std::cerr << "Minimum game time = " << minimum_game_time.count() << "\n";
        std::cerr << "Maximum game time = " << maximum_game_time.count() << "\n";
        throw std::invalid_argument("Maximum game time must be greater than the minimum game time.");
    }
    const auto game_time_increment = config.as_time_duration<Clock::seconds>("game time increment");

    const auto board = Board{config.as_text_or_default("FEN", Board().fen())};
    const auto verbose_output = config.as_boolean("output volume", "verbose", "quiet");

    auto pool_clock = get_pool_clock(config);
    const auto start_delay = get_start_delay(config);
    
    if(config.any_unused_parameters())
    {
        std::cout << "There were unused parameters in the file: " << config_file << '\n';
        config.print_unused_parameters();
        return;
    }

    if(start_delay > 0s)
    {
        std::cout << "Gene pool will start at " << future_timestamp(start_delay) << std::endl;
    }
    std::this_thread::sleep_for(start_delay);

    auto round_count = count_still_alive_lines(genome_file_name);
    auto pool = fill_pool(genome_file_name, gene_pool_population, first_mutation_rate);

    const auto game_record_file = std::format("{}_games.pgn", genome_file_name);
    auto game_time = game_time_increment > 0.0s ? minimum_game_time : maximum_game_time;
    std::array<size_t, 3> color_wins{}; // indexed with [Winner_Color]
    load_previous_game_stats(game_record_file, game_time, color_wins);
    game_time = std::clamp(game_time, minimum_game_time, maximum_game_time);

    const auto best_file_name = std::format("{}_best_genome.txt", genome_file_name);

    pool_clock.start(Piece_Color::WHITE);
    signal(QUIT_SIGNAL, quit_gene_pool);
    while(keep_going(pool_clock))
    {
        const auto mutation_phase = round_count++ % (first_mutation_interval + second_mutation_interval);
        const auto mutation_rate = mutation_phase < first_mutation_interval ? first_mutation_rate : second_mutation_rate;

        print_round_header(pool, genome_file_name, color_wins, round_count, first_mutation_interval, second_mutation_interval, mutation_rate, game_time, pool_clock);

        Random::shuffle(pool);
        std::vector<std::future<Game_Result>> results;
        auto limiter = std::counting_semaphore(maximum_simultaneous_games);
        for(size_t index = 0; index < gene_pool_population; index += 2)
        {
            const auto& white = pool[index];
            const auto& black = pool[index + 1];
            results.emplace_back(std::async(std::launch::async, pool_game, std::cref(board), game_time, white, black, std::cref(game_record_file), std::ref(limiter)));
            std::cout << '=' << std::flush;
        }
        std::cout << std::endl;

        std::stringstream result_printer;
        for(size_t index = 0; index < gene_pool_population; index += 2)
        {
            auto& white = pool[index];
            auto& black = pool[index + 1];
            const auto result = results[index/2].get();
            const auto winner = result.winner();
            result_printer << white.id() << " vs " << black.id() << ": " << color_text(winner) << " (" << result.ending_reason() << ")\n";
            std::cout << '^' << std::flush;
            ++space_counter;

            const auto mating_winner = (winner == Winner_Color::NONE ? (Random::coin_flip() ? Winner_Color::WHITE : Winner_Color::BLACK) : winner);
            auto& winning_player = (mating_winner == Winner_Color::WHITE ? white : black);
            auto& losing_player = (winning_player.id() == white.id() ? black : white);

            auto offspring = Genetic_AI(white, black);
            offspring.mutate(mutation_rate);
            offspring.print(genome_file_name);
            losing_player = offspring;

            ++color_wins[static_cast<int>(winner)];
            if(winner == Winner_Color::NONE)
            {
                winning_player.add_draw();
            }
            else
            {
                winning_player.add_win();
            }
        }
        std::cout << std::endl;
        space_counter = 0;

        std::sort(pool.begin(), pool.end());
        record_the_living(pool, genome_file_name);
        record_best_ai(pool, best_file_name);

        if(verbose_output)
        {
            print_verbose_output(result_printer, pool);
        }

        game_time = std::clamp(game_time + game_time_increment, minimum_game_time, maximum_game_time);
    }
    std::cout << "Done.\n";
}

namespace
{
    bool keep_going(const Clock& pool_clock)
    {
        return ! (quit_after_round || pool_clock.running_time_expired());
    }

    void quit_gene_pool(int)
    {
        std::cout << "\nGetting to a good stopping point ...\n" << std::string(space_counter, ' ');
        quit_after_round = true;
    }

    std::vector<Genetic_AI> fill_pool(const std::string& genome_file_name, size_t gene_pool_population, size_t mutation_rate)
    {
        auto pool = load_gene_pool_file(genome_file_name);
        if(pool.size() != gene_pool_population)
        {
            pool.reserve(gene_pool_population);
            while(pool.size() < gene_pool_population)
            {
                pool.emplace_back();
                pool.back().mutate(mutation_rate);
                pool.back().print(genome_file_name);
            }
            pool.resize(gene_pool_population);
            record_the_living(pool, genome_file_name);
        }

        return pool;
    }

    void record_the_living(const std::vector<Genetic_AI>& pool, const std::string& genome_file_name)
    {
        auto ofs = std::ofstream(genome_file_name, std::ios::app);
        if( ! ofs)
        {
            throw std::runtime_error(std::format("Could not open gene pool file for writing: {}", genome_file_name));
        }

        ofs << "Still Alive: ";
        for(const auto& ai : pool)
        {
            ofs << ai.id() << " ";
        }
        ofs << "\n\n";
    }

    void print_round_header(const std::vector<Genetic_AI>& pool,
                            const std::string& genome_file_name,
                            const std::array<size_t, 3>& color_wins,
                            const size_t round_count,
                            const size_t first_mutation_interval,
                            const size_t second_mutation_interval,
                            const size_t mutation_rate,
                            const Clock::seconds game_time,
                            const Clock& pool_time) noexcept
    {
        std::cout << "\n\nGene pool size: " << pool.size()
                  << "  Gene pool file name: " << genome_file_name
                  << "\nGames: " << std::accumulate(color_wins.begin(), color_wins.end(), size_t{0})
                  << "  White wins: " << color_wins[static_cast<int>(Winner_Color::WHITE)]
                  << "  Black wins: " << color_wins[static_cast<int>(Winner_Color::BLACK)]
                  << "  Draws: " << color_wins[static_cast<int>(Winner_Color::NONE)]
                  << "\nRounds: " << round_count
                  << "  Mutation rate phase: " << round_count % (first_mutation_interval + second_mutation_interval)
                  << " (" << first_mutation_interval << "/" << second_mutation_interval << ")"
                  << "\nMutation rate: " << mutation_rate << "  Game time: " << game_time.count() << " sec"
                  << "\nFinish time: " << future_timestamp(pool_time.running_time_left())  << "\n\n";

        const auto best_living = best_living_ai(pool);

        std::cout << "Best living ID : " << best_living.id() << "    Wins: " << best_living.wins() << "\n\n";
        std::cout << "Quit after this round: " << quit_key << "\n\n";
    }

    void print_verbose_output(const std::stringstream& result_printer, const std::vector<Genetic_AI>& pool)
    {
        std::cout << result_printer.str();

        // widths of columns for stats printout
        const auto largest_id = std::ranges::max_element(pool)->id();
        const auto id_column_width = int(std::to_string(largest_id).size() + 1);
        const auto win_column_width = 7;
        const auto draw_column_width = 7;

        // Write stat headers
        std::cout << "\n"
                  << std::setw(id_column_width) << "ID"
                  << std::setw(win_column_width) << "Wins"
                  << std::setw(draw_column_width) << "Draws" << "\n";

        // Write stats for each specimen
        for(const auto& ai : pool)
        {
            std::cout << std::setw(id_column_width) << ai.id()
                      << std::setw(win_column_width) << ai.wins()
                      << std::setw(draw_column_width) << ai.draws() << "\n";
        }
    }

    void load_previous_game_stats(const std::string& game_record_file, Clock::seconds& game_time, std::array<size_t, 3>& color_wins)
    {
        auto ifs = std::ifstream(game_record_file);
        if( ! ifs)
        {
            return;
        }

        // Use game time from last run of this gene pool
        std::cout << "Searching " << game_record_file << " for last game time and stats ...\n";
        for(std::string line; std::getline(ifs, line);)
        {
            line = String::trim_outer_whitespace(line);
            if(line.starts_with("[TimeControl"))
            {
                game_time = String::to_duration<Clock::seconds>(String::extract_delimited_text(line, '"', '"'));
            }
            else if(line.starts_with("[Result"))
            {
                auto result = String::extract_delimited_text(line, '"', '"');
                if(result == "1-0")
                {
                    color_wins[static_cast<int>(Winner_Color::WHITE)]++;
                }
                else if(result == "0-1")
                {
                    color_wins[static_cast<int>(Winner_Color::BLACK)]++;
                }
                else if(result == "1/2-1/2")
                {
                    color_wins[static_cast<int>(Winner_Color::NONE)]++;
                }
                else
                {
                    throw std::invalid_argument(std::format("Bad PGN Result line: {}", line));
                }
            }
        }
    }

    Game_Result pool_game(const Board& board,
                          const Clock::seconds game_time,
                          Genetic_AI white,
                          Genetic_AI black,
                          const std::string& game_record_file,
                          std::counting_semaphore<>& limiter) noexcept
    {
        limiter.acquire();
        const auto result = play_game(board, Clock{game_time}, white, black, "Gene pool", "Local computer", game_record_file, false);
        limiter.release();
        return result;
    }

    void record_best_ai(const std::vector<Genetic_AI>& pool, const std::string& best_file_name) noexcept
    {
        const auto temp_best_file_name = std::format("{}.tmp", best_file_name);
        best_living_ai(pool).print(temp_best_file_name);
        std::filesystem::rename(temp_best_file_name, best_file_name);
    }

    Genetic_AI best_living_ai(const std::vector<Genetic_AI>& pool) noexcept
    {
        return *std::max_element(pool.begin(), pool.end(),
                                 [](const auto& a, const auto& b)
                                 {
                                     return a.wins() < b.wins();
                                 });
    }

    Clock get_pool_clock(const Configuration& config)
    {
        const auto time_limit = "time limit";
        if( ! config.has_parameter(time_limit))
        {
            return Clock(Clock::seconds(std::numeric_limits<double>::infinity()));
        }
        
        return Clock(get_duration(config.as_text(time_limit)));
    }

    std::chrono::duration<double> get_start_delay(const Configuration& config)
    {
        const auto start_delay = "start delay";
        if(!config.has_parameter(start_delay))
        {
            return {};
        }

        return get_duration(config.as_text(start_delay));
    }

    std::chrono::duration<double> get_duration(const std::string& time_text)
    {
        const auto time_spec = String::split(time_text);
        if(time_spec.size() != 2)
        {
            throw std::invalid_argument(std::format("Invalid time limit. Must be of form <number> <unit>. Got: {}", time_text));
        }

        const auto number = std::stod(time_spec[0]);
        const auto unit = time_spec[1];
        const auto hour_names = {"hours", "hour", "hrs", "hr", "h"};
        const auto minute_names = {"minutes", "minute", "mins", "min", "m"};
        const auto second_names = {"seconds", "second", "secs", "sec", "s"};

        const auto contains = [](const auto& list, const auto& value)
            {
                return std::ranges::find(list, value) != list.end();
            };

        if(contains(hour_names, unit))
        {
            return Clock::hours(number);
        }
        else if(contains(minute_names, unit))
        {
            return Clock::minutes(number);
        }
        else if(contains(second_names, unit))
        {
            return Clock::seconds(number);
        }
        else
        {
            throw std::invalid_argument(std::format("Invalid time unit: {}", unit));
        }
    }

    std::string future_timestamp(const std::chrono::duration<double>& duration) noexcept
    {
        if(std::isfinite(duration.count()))
        {
            const auto future = std::chrono::system_clock::now() + std::chrono::duration_cast<std::chrono::system_clock::duration>(duration);
            return String::date_and_time_format(future, "%Y-%m-%d %H:%M:%S");
        }
        else
        {
            return "Never";
        }
    }

    std::vector<Genetic_AI> load_gene_pool_file(const std::string& load_file)
    {
        std::ifstream ifs(load_file);
        if( ! ifs)
        {
            std::cout << "Starting new gene pool and writing to: " << load_file << '\n';
            return {};
        }

        std::cout << "Loading gene pool file: " << load_file << " ...\n";
        std::string still_alive;
        size_t pool_line_number = 0;
        std::string pool_line;

        size_t line_number = 0;
        for(std::string line; std::getline(ifs, line);)
        {
            ++line_number;
            if(String::contains(line, "Still Alive"))
            {
                try
                {
                    auto parse = String::split(line, ":", 1);
                    still_alive = parse.at(1);
                    pool_line_number = line_number;
                    pool_line = line;
                }
                catch(...)
                {
                    throw_on_bad_still_alive_line(line_number, line);
                }
            }
        }

        if(pool_line.empty())
        {
            std::cout << "No \"Still Alive\" lines found. Starting with empty gene pool.";
            return {};
        }

        const auto id_strings = String::split(still_alive);
        std::vector<int> ids;
        try
        {
            std::ranges::transform(id_strings, std::back_inserter(ids), String::to_number<int>);
        }
        catch(...)
        {
            throw_on_bad_still_alive_line(pool_line_number, pool_line);
        }

        auto sorted_ids = ids;
        std::ranges::sort(sorted_ids);

        std::map<int, Genetic_AI> loaded_ais;
        for(auto id : sorted_ids)
        {
            while(true)
            {
                const auto search_started_from_beginning_of_file = ifs.tellg() == 0;
                try
                {
                    loaded_ais.insert_or_assign(id, Genetic_AI{ifs, id});
                    break;
                }
                catch(const Genome_Creation_Error& e)
                {
                    if(search_started_from_beginning_of_file)
                    {
                        std::cerr << e.what() << load_file << "\n";
                        throw_on_bad_still_alive_line(pool_line_number, pool_line);
                    }
                    else
                    {
                        ifs = std::ifstream(load_file);
                    }
                }
            }
        }

        std::vector<Genetic_AI> result;
        std::ranges::transform(ids, std::back_inserter(result), [&loaded_ais](const auto id) { return loaded_ais.at(id); });

        return result;
    }

    void throw_on_bad_still_alive_line(size_t line_number, const std::string& line)
    {
        throw std::runtime_error(std::format("Invalid \"Still Alive\" line (line# {}): {}", line_number, line));
    }

    size_t count_still_alive_lines(const std::string& genome_file_name) noexcept
    {
        auto genome_file = std::ifstream(genome_file_name);
        if( ! genome_file)
        {
            return 0;
        }

        std::cout << "Counting number of previous rounds...\n";
        size_t round_count = 0;
        for(std::string line; std::getline(genome_file, line);)
        {
            if(line.starts_with("Still Alive"))
            {
                ++round_count;
            }
        }

        return round_count;
    }
}
