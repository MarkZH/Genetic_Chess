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

#ifndef _WIN32
#include <mutex>
#endif // _WIN32

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
    const std::string stop_key = "Ctrl-c";

#ifdef _WIN32
    const auto PAUSE_SIGNAL = SIGINT;
    auto quit_gene_pool = false;
#else
    const auto PAUSE_SIGNAL = SIGTSTP;
    const std::string pause_key = "Ctrl-z";
    std::mutex pause_mutex;
#endif
    bool gene_pool_started = false;

    using Gene_Pool = std::vector<Genetic_AI>;

    Gene_Pool load_gene_pool_file(const std::string& load_file);
    [[noreturn]] void throw_on_bad_still_alive_line(size_t line_number, const std::string& line);

    void pause_gene_pool(int);

    void write_generation(const Gene_Pool& pool, const std::string& genome_file_name, bool force_write_still_alive);

    template<typename Stat_Map>
    void purge_dead_from_map(const Gene_Pool& pool, Stat_Map& stats);

    int count_wins(const std::string& file_name, int id);
}

void gene_pool(const std::string& config_file)
{
    // Signal to pause gene pool
    signal(PAUSE_SIGNAL, pause_gene_pool);

    auto config = Configuration(config_file);

    const auto maximum_simultaneous_games = config.as_positive_number<int>("maximum simultaneous games");
    const auto gene_pool_population = config.as_positive_number<size_t>("gene pool population");
    if(gene_pool_population % 2 != 0)
    {
        throw std::invalid_argument("Gene pool population must be even. Current value = " + std::to_string(gene_pool_population));
    }
    const auto roaming_distance = config.as_positive_number<double>("roaming distance");
    const auto genome_file_name = config.as_text("gene pool file");

    const auto first_mutation_rate = config.as_positive_number<size_t>("first mutation rate");
    const auto first_mutation_interval = config.as_positive_number<size_t>("first mutation interval");
    const auto second_mutation_rate = config.as_positive_number<size_t>("second mutation rate");
    const auto second_mutation_interval = config.as_positive_number<size_t>("second mutation interval");

    auto mutation_rate = first_mutation_rate;
    const auto mutation_period = first_mutation_interval + second_mutation_interval;

    const auto minimum_game_time = config.as_positive_time_duration<Clock::seconds>("minimum game time");
    const auto maximum_game_time = config.as_positive_time_duration<Clock::seconds>("maximum game time");
    if(maximum_game_time < minimum_game_time)
    {
        std::cerr << "Minimum game time = " << minimum_game_time.count() << "\n";
        std::cerr << "Maximum game time = " << maximum_game_time.count() << "\n";
        throw std::invalid_argument("Maximum game time must be greater than the minimum game time.");
    }
    const auto game_time_increment = config.as_time_duration<Clock::seconds>("game time increment");

    auto seed_ai_specification = config.has_parameter("seed") ? config.as_text("seed") : std::string{};
    auto verbose_output = config.as_boolean("output volume", "verbose", "quiet");

    if(config.any_unused_parameters())
    {
        std::cout << "There were unused parameters in the file: " << config_file << std::endl;
        config.print_unused_parameters();
        std::cout << "\nPress enter to continue or " << stop_key << " to quit ..." << std::endl;
        std::cin.get();
    }

    std::array<size_t, 3> color_wins{}; // indexed with [Winner_Color]
    std::map<Genetic_AI, int> wins;
    std::map<Genetic_AI, int> draws;

    std::cout << "Loading gene pool file: " << genome_file_name << " ..." << std::endl;
    auto pool = load_gene_pool_file(genome_file_name);
    auto write_new_pools = pool.size() != gene_pool_population;
    if(pool.empty() && ! seed_ai_specification.empty())
    {
        auto seed_split = String::split(seed_ai_specification, "/");
        if(seed_split.size() > 2)
        {
            throw std::runtime_error("Too many parameters in the seed configuration\nseed = " + seed_ai_specification);
        }
        auto file_name = seed_split.front();
        auto seed_id = seed_split.size() == 2 ? String::to_number<int>(seed_split.back()) : find_last_id(file_name);
        auto seed_ai = Genetic_AI(file_name, seed_id);
        std::cout << "Seeding with #" << seed_ai.id() << " from file " << file_name << std::endl;
        pool = {seed_ai};
    }
    auto new_ai_index = pool.size();
    pool.resize(gene_pool_population);
    for(auto ai_index = new_ai_index; ai_index < pool.size(); ++ai_index)
    {
        pool[ai_index].mutate(mutation_rate);
    }
    write_generation(pool, genome_file_name, write_new_pools);

    size_t round_count = 0; // Count of complete gene pool rounds where all pools have played a set of games
    if(auto genome_file = std::ifstream(genome_file_name))
    {
        for(std::string line; std::getline(genome_file, line);)
        {
            line = String::trim_outer_whitespace(line);
            if(String::starts_with(line, "Still Alive"))
            {
                ++round_count;
            }
        }
    }

    const auto game_record_file = genome_file_name + "_games.pgn";
    auto game_time = game_time_increment > 0.0s ? minimum_game_time : maximum_game_time;

    std::cout << "Searching for previous best AI win counts ..." << std::endl;
    const auto best_file_name = genome_file_name + "_best_genome.txt";
    auto best_id = 0;
    auto best_id_wins = 0;
    auto wins_to_beat = 0.0;
    try
    {
        auto best = Genetic_AI(best_file_name, find_last_id(best_file_name));
        best_id = best.id();
        best_id_wins = count_wins(game_record_file, best_id);
        wins_to_beat = best_id_wins;
    }
    catch(...)
    {
    }

    if(auto ifs = std::ifstream(game_record_file))
    {
        // Use game time from last run of this gene pool
        std::cout << "Searching " << game_record_file << " for last game time and stats ..." << std::endl;
        for(std::string line; std::getline(ifs, line);)
        {
            line = String::trim_outer_whitespace(line);
            if(String::starts_with(line, "[TimeControl"))
            {
                game_time = String::to_duration<Clock::seconds>(String::split(line, "\"").at(1));
            }
            else if(String::starts_with(line, "[Result"))
            {
                auto result = String::split(line, "\"").at(1);
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
                    throw std::invalid_argument("Bad PGN Result line: " + line);
                }
            }
        }

        std::cout << "Done." << std::endl;
    }

    gene_pool_started = true;
    while(true)
    {
        // Pause gene pool
    #ifdef _WIN32
        if(quit_gene_pool)
        {
            std::cout << "Done." << std::endl;
            break;
        }
    #else
        if(auto pause_lock = std::unique_lock(pause_mutex, std::try_to_lock); !pause_lock.owns_lock())
        {
            std::cout << "\nGene pool paused. Press " << pause_key << " to continue ";
            std::cout << "or " << stop_key << " to quit." << std::endl;
            pause_lock.lock();
        }
    #endif // _WIN32

        // Write overall stats
        std::cout << "\n=======================\n\n"
                  << "Gene pool size: " << pool.size()
                  << "  Gene pool file name: " << genome_file_name
                  << "\nGames: " << std::accumulate(color_wins.begin(), color_wins.end(), size_t{0})
                  << "  White wins: " << color_wins[static_cast<int>(Winner_Color::WHITE)]
                  << "  Black wins: " << color_wins[static_cast<int>(Winner_Color::BLACK)]
                  << "  Draws: " << color_wins[static_cast<int>(Winner_Color::NONE)]
                  << "\nRounds: " << round_count
                  << "  Rounds since high mutation interval: " << round_count % mutation_period
                  << " (" << first_mutation_interval << "/" << second_mutation_interval << ")"
                  << "\nMutation rate: " << mutation_rate << "  Game time: " << game_time.count() << " sec\n\n";

    #ifdef _WIN32
        std::cout << "Quit after this round: " << stop_key << "    Abort: " << stop_key << " " << stop_key << "\n" << std::endl;
    #else
        std::cout << "Pause: " << pause_key << "    Abort: " << stop_key << "\n" << std::endl;
    #endif // _WIN32

        // The shuffled pool list determines the match-ups. After shuffling the list,
        // adjacent AIs are matched as opponents.
        Random::stir_order(pool, roaming_distance);
        std::vector<std::future<Game_Result>> results;
        for(size_t index = 0; index < gene_pool_population; index += 2)
        {
            while(int(results.size()) >= maximum_simultaneous_games)
            {
                auto in_progress_games = std::count_if(results.begin(),
                                                       results.end(),
                                                       [](const auto& r)
                                                       {
                                                           return r.wait_for(100ms) != std::future_status::ready;
                                                       });

                if(in_progress_games < maximum_simultaneous_games)
                {
                    break;
                }
            }

            const auto& white = pool[index];
            const auto& black = pool[index + 1];
            results.emplace_back(std::async(std::launch::async, play_game,
                                            Board{},
                                            Clock(game_time),
                                            std::cref(white), std::cref(black),
                                            "Gene pool",
                                            "Local computer",
                                            game_record_file));
        }

        std::stringstream result_printer;
        for(size_t index = 0; index < gene_pool_population; index += 2)
        {
            auto& white = pool[index];
            auto& black = pool[index + 1];

            auto result = results[index/2].get();
            auto winner = result.winner();
            result_printer << white.id() << " vs " << black.id() << ": " << color_text(winner) << " (" << result.ending_reason() << ")\n";

            // A player survives if it wins or draws as black
            const auto& winning_player = (winner == Winner_Color::WHITE ? white : black);
            auto& losing_player = (winning_player.id() == white.id() ? black : white);

            auto offspring = Genetic_AI(white, black);
            offspring.mutate(mutation_rate);
            losing_player = offspring;

            ++color_wins[static_cast<int>(winner)];
            ++(winner == Winner_Color::NONE ? draws : wins)[winning_player];
        }

        auto sorted_pool = pool;
        std::sort(sorted_pool.begin(), sorted_pool.end());
        write_generation(sorted_pool, genome_file_name, false);

        purge_dead_from_map(pool, wins);
        purge_dead_from_map(pool, draws);

        if(verbose_output)
        {
            std::cout << result_printer.str();

            // widths of columns for stats printout
            auto id_column_width = std::to_string(sorted_pool.back().id()).size() + 1;
            auto win_column_width = 7;
            auto draw_column_width = 7;

            // Write stat headers
            std::cout << "\n"
                      << std::setw(id_column_width) << "ID"
                      << std::setw(win_column_width) << "Wins"
                      << std::setw(draw_column_width) << "Draws" << "\n";

            // Write stats for each specimen
            for(const auto& ai : sorted_pool)
            {
                std::cout << std::setw(id_column_width) << ai.id()
                          << std::setw(win_column_width) << wins[ai]
                          << std::setw(draw_column_width) << draws[ai] << "\n";
            }
        }

        // Record best AI from all pools.
        const double decay_constant = 0.99;
        wins_to_beat *= decay_constant;
        for(const auto& [ai, win_count] : wins)
        {
            if(win_count > wins_to_beat)
            {
                static const auto temp_best_file_name = best_file_name + ".tmp";

                wins_to_beat = win_count;
                best_id = ai.id();
                best_id_wins = win_count;
                ai.print(temp_best_file_name);
                std::filesystem::rename(temp_best_file_name, best_file_name);
            }
        }

        std::cout << "\nWins to be recorded as best: " << wins_to_beat
                  << "\nBest ID: " << best_id << " with " << best_id_wins << " win" << (best_id_wins != 1 ? "s" : "") << "\n";

        ++round_count;
        game_time = std::clamp(game_time + game_time_increment, minimum_game_time, maximum_game_time);

        const auto mutation_phase = round_count % mutation_period;
        mutation_rate = mutation_phase < first_mutation_interval ? first_mutation_rate : second_mutation_rate;
    }
}

namespace
{
    void pause_gene_pool(int)
    {
    #ifdef _WIN32
        if(gene_pool_started)
        {
            quit_gene_pool = true;
            std::cout << "\nQuitting program after current games are finished ..." << std::endl;
        }
        else
        {
            exit(1);
        }
    #else
        static auto pause_lock = std::unique_lock(pause_mutex, std::defer_lock);

        if(pause_lock.owns_lock())
        {
            pause_lock.unlock();
            std::cout << "\nResuming ..." << std::endl;
        }
        else
        {
            pause_lock.lock();
            if(gene_pool_started)
            {
                std::cout << "\nWaiting for games to end and be recorded before pausing ..." << std::endl;
            }
            else
            {
                std::cout << "\n\nWill pause once the loading of files finishes ..." << std::endl;
            }
        }
    #endif // _WIN32
    }

    void write_generation(const Gene_Pool& pool, const std::string& genome_file_name, bool force_write_still_alive)
    {
        static std::map<Genetic_AI, bool> written_before;
        static std::string last_file_name;
        static std::ofstream ofs;
        if(last_file_name != genome_file_name)
        {
            ofs.close();
            ofs.open(genome_file_name, std::ios::app);
            last_file_name = genome_file_name;
        }

        if( ! genome_file_name.empty() && ! ofs)
        {
            throw std::runtime_error("Could not write to file:" + genome_file_name);
        }

        auto needs_still_alive_line = false;
        for(const auto& ai : pool)
        {
            if( ! written_before[ai])
            {
                ai.print(ofs);
                written_before[ai] = true;
                needs_still_alive_line = true;
            }
        }

        if(needs_still_alive_line || force_write_still_alive)
        {
            ofs << "\nStill Alive: ";
            for(const auto& ai : pool)
            {
                ofs << ai.id() << " ";
            }
            ofs << "\n\n" << std::flush;
        }

        purge_dead_from_map(pool, written_before);
    }

    Gene_Pool load_gene_pool_file(const std::string& load_file)
    {
        std::ifstream ifs(load_file);
        if( ! ifs)
        {
            std::cout << "Could not open file: " << load_file << std::endl;
            std::cout << "Starting with empty gene pool." << std::endl;
            return {};
        }

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

        ifs = std::ifstream(load_file);
        bool search_started_from_beginning_of_file = true;
        Gene_Pool result;
        for(auto id_string : String::split(still_alive))
        {
            while(true)
            {
                try
                {
                    result.emplace_back(ifs, std::stoi(id_string));
                    search_started_from_beginning_of_file = false;
                    break;
                }
                catch(const Genetic_AI_Creation_Error& e)
                {
                    if(search_started_from_beginning_of_file)
                    {
                        std::cerr << e.what() << load_file << "\n";
                        throw_on_bad_still_alive_line(pool_line_number, pool_line);
                    }
                    else
                    {
                        ifs = std::ifstream(load_file);
                        search_started_from_beginning_of_file = true;
                        continue;
                    }
                }
                catch(...)
                {
                    throw_on_bad_still_alive_line(pool_line_number, pool_line);
                }
            }
        }

        write_generation(result, "", false); // mark AIs from file as already written
        return result;
    }

    void throw_on_bad_still_alive_line(size_t line_number, const std::string& line)
    {
        throw std::runtime_error("Invalid \"Still Alive\" line (line# " + std::to_string(line_number) + "): " + line);
    }

    template<typename Stat_Map>
    void purge_dead_from_map(const Gene_Pool& pool, Stat_Map& stats)
    {
        Stat_Map new_stats;
        for(const auto& ai : pool)
        {
            new_stats[ai] = stats[ai];
        }
        stats = new_stats;
    }

    int count_wins(const std::string& file_name, int id)
    {
        auto input = std::ifstream(file_name);
        if( ! input)
        {
            return 0;
        }

        auto win_count = 0;
        for(std::string line; std::getline(input, line);)
        {
            line = String::remove_extra_whitespace(line);
            auto is_white_player = String::starts_with(line, "[White");
            auto is_black_player = String::starts_with(line, "[Black");
            if(is_white_player || is_black_player)
            {
                auto player_id = String::to_number<int>(String::split(String::split(line, "\"").at(1)).at(2));
                if(player_id == id)
                {
                    while(std::getline(input, line))
                    {
                        line = String::remove_extra_whitespace(line);
                        if(String::starts_with(line, "[Result"))
                        {
                            if((is_white_player && String::contains(line, "1-0")) ||
                               (is_black_player && String::contains(line, "0-1")))
                            {
                                ++win_count;
                            }
                            break;
                        }
                    }
                }
            }
        }

        return win_count;
    }
}
