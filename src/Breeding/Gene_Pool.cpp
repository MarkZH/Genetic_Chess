#include "Breeding/Gene_Pool.h"

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
#include <utility>

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

#include "Exceptions/Bad_Still_Alive_Line.h"

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

    using Gene_Pool_Set = std::vector<std::vector<Genetic_AI>>;

    Gene_Pool_Set load_gene_pool_file(const std::string& load_file);

    void pause_gene_pool(int);

    void write_generation(const Gene_Pool_Set& pools, size_t pool_index, const std::string& genome_file_name);

    template<typename Stat_Map>
    void purge_dead_from_map(const Gene_Pool_Set& pools, Stat_Map& stats);
}

void gene_pool(const std::string& config_file)
{
    // Signal to pause gene pool
    signal(PAUSE_SIGNAL, pause_gene_pool);

    auto config = Configuration(config_file);

    // Environment variables
    const auto maximum_simultaneous_games = size_t(config.as_positive_number("maximum simultaneous games"));
    const auto gene_pool_population = size_t(config.as_positive_number("gene pool population"));
    const auto gene_pool_count = size_t(config.as_positive_number("gene pool count"));
    const auto pool_swap_interval = size_t(config.as_number("pool swap interval"));
    const auto sexual_reproduction = config.as_boolean("reproduction type", "sexual", "cloning");
    const auto genome_file_name = config.as_text("gene pool file");

    const int scramble_mutations = 1; // initial number of mutations when creating a new Genetic AI
    size_t rounds_since_last_swap = 0; // Count of complete gene pool rounds where all pools have played a set of games

    // Oscillating game time
    const double minimum_game_time = config.as_number("minimum game time"); // seconds
    const double maximum_game_time = config.as_number("maximum game time"); // seconds
    if(maximum_game_time < minimum_game_time)
    {
        std::cerr << "Minimum game time = " << minimum_game_time << "\n";
        std::cerr << "Maximum game time = " << maximum_game_time << "\n";
        throw std::invalid_argument("Maximum game time must be greater than the minimum game time.");
    }
    double game_time_increment = config.as_number("game time increment"); // seconds
    const bool oscillating_time = config.as_boolean("oscillating time", "yes", "no");
    double game_time = minimum_game_time;

    if(config.any_unused_parameters())
    {
        std::cout << "There were unused parameters in the file: " << config_file << std::endl;
        config.print_unused_parameters();
        std::cout << "\nPress enter to continue or " << stop_key << " to quit ..." << std::endl;
        std::cin.get();
    }

    std::array<size_t, 2> color_wins{}; // indexed with [Color]
    size_t draw_count = 0;

    // Individual Genetic AI stats
    std::map<Genetic_AI, int> wins;
    std::map<Genetic_AI, int> draws;
    std::map<Genetic_AI, size_t> original_pool;

    std::cout << "Loading gene pool file: " << genome_file_name << " ..." << std::endl;
    auto pools = load_gene_pool_file(genome_file_name);
    pools.resize(gene_pool_count);

    for(size_t i = 0; i < pools.size(); ++i)
    {
        while(pools[i].size() < gene_pool_population)
        {
            pools[i].push_back(Genetic_AI(scramble_mutations));
        }

        while(pools[i].size() > gene_pool_population)
        {
            pools[i].pop_back();
        }

        for(const auto& ai : pools[i])
        {
            original_pool[ai] = i;
        }
    }

    for(size_t i = 0; i < pools.size(); ++i)
    {
        write_generation(pools, i, genome_file_name);
    }

    size_t starting_pool = 0;
    if(auto genome_file = std::ifstream(genome_file_name))
    {
        std::string line;
        size_t line_number = 0;
        auto previous_line_was_still_alive = false;
        while(std::getline(genome_file, line))
        {
            line = String::trim_outer_whitespace(line);
            ++line_number;
            if(line.empty())
            {
                continue;
            }

            if(String::starts_with(line, "Still Alive"))
            {
                if(previous_line_was_still_alive)
                {
                    continue; // Still alive line was written after loading a previous session
                }

                try
                {
                    auto alive_split = String::split(line, ":");
                    auto pool_number = String::string_to_size_t(alive_split.at(1));
                    starting_pool = (pool_number + 1) % gene_pool_count;
                    if(starting_pool == 0)
                    {
                        rounds_since_last_swap = (rounds_since_last_swap + 1)%pool_swap_interval;
                    }
                }
                catch(const std::exception&)
                {
                    throw Bad_Still_Alive_Line(line_number, line);
                }

                previous_line_was_still_alive = true;
            }
            else
            {
                previous_line_was_still_alive = false;
            }
        }
    }

    std::string game_record_file = genome_file_name +  "_games.pgn";
    if(auto ifs = std::ifstream(game_record_file))
    {
        // Use game time from last run of this gene pool
        std::cout << "Searching " << game_record_file << " for last game time and stats ..." << std::endl;
        std::string line;
        std::string time_line;
        std::string previous_time_line;
        while(std::getline(ifs, line))
        {
            if(String::starts_with(line, "[TimeControl"))
            {
                if(line != time_line)
                {
                    previous_time_line = time_line;
                    time_line = line;
                }
            }
            else if(String::starts_with(line, "[Result"))
            {
                auto result = String::split(line, "\"").at(1);
                if(result == "1-0")
                {
                    color_wins[WHITE]++;
                }
                else if(result == "0-1")
                {
                    color_wins[BLACK]++;
                }
                else if(result == "1/2-1/2")
                {
                    draw_count++;
                }
                else
                {
                    throw std::invalid_argument("Bad PGN Result line: " + line);
                }
            }
        }

        if( ! time_line.empty())
        {
            game_time = std::stod(String::split(time_line, "\"").at(1));
            game_time = std::clamp(game_time, minimum_game_time, maximum_game_time);
        }

        double previous_game_time = game_time;
        if( ! previous_time_line.empty())
        {
            previous_game_time = std::stod(String::split(previous_time_line, "\"").at(1));
        }

        if(previous_game_time > game_time)
        {
            game_time_increment = -std::abs(game_time_increment);
        }

        game_time = std::clamp(game_time + game_time_increment, minimum_game_time, maximum_game_time);

        std::cout << "Done." << std::endl;
    }

    for(size_t pool_index = starting_pool; true; pool_index = (pool_index + 1) % pools.size()) // run forever
    {
        auto& pool = pools[pool_index];

        // Write overall stats
        std::cout << "\n=======================\n\n"
                  << "Gene pool ID: " << pool_index
                  << "  Gene pool size: " << pool.size()
                  << "  Rounds since pool swaps: " << rounds_since_last_swap << "/" << pool_swap_interval
                  << "\nGames: " << color_wins[WHITE] + color_wins[BLACK] + draw_count
                  << "  White wins: " << color_wins[WHITE]
                  << "  Black wins: " << color_wins[BLACK]
                  << "  Draws: " << draw_count
                  << "\nTime: " << game_time << " sec"
                  << "   Gene pool file name: " << genome_file_name << "\n\n";

        #ifdef _WIN32
        std::cout << "Quit after this round: " << stop_key << "    Abort: " << stop_key << " " << stop_key << "\n" << std::endl;
        #else
        std::cout << "Pause: " << pause_key << "    Abort: " << stop_key << "\n" << std::endl;
        #endif // _WIN32

        // The shuffled pool list determines the match-ups. After shuffling the list,
        // adjacent AIs are matched as opponents.
        Random::shuffle(pool);

        std::vector<std::future<Game_Result>> results; // map from matchups to winners (half the size of pool)
        for(size_t index = 0; index < gene_pool_population; index += 2)
        {
            // Limit the number of simultaneous games by waiting for earlier games to finish
            // before starting a new one.
            while(true)
            {
                auto in_progress_games = size_t(std::count_if(results.begin(),
                                                              results.end(),
                                                              [](const auto& r)
                                                              { return r.wait_for(0s) != std::future_status::ready; }));

                if(gene_pool_population > 2*maximum_simultaneous_games &&
                   in_progress_games >= maximum_simultaneous_games)
                {
                    std::this_thread::sleep_for(100ms);
                }
                else
                {
                    break;
                }
            }

            const auto& white = pool[index];
            const auto& black = pool[index + 1];
            results.emplace_back(std::async(std::launch::async,
                                            play_game, Board{}, Clock(game_time), white, black, false, game_record_file));
        }

        // Get results as they come in
        for(size_t index = 0; index < gene_pool_population; index += 2)
        {
            auto& white = pool[index];
            auto& black = pool[index + 1];

            std::cout << white.id() << " vs "
                      << black.id() << ": " << std::flush;

            auto result = results[index/2].get();
            auto winner = result.winner();
            std::cout << color_text(winner) << " (" << result.ending_reason() << ")";

            const auto& winning_player = (winner == WHITE ? white : black);
            if(winner != NONE)
            {
                color_wins[winner]++;
                wins[winning_player]++;
            }
            else
            {
                draws[white]++;
                draws[black]++;
                ++draw_count;

                winner = (Random::coin_flip() ? WHITE : BLACK);
                std::cout << " --> " << (winner == WHITE ? white : black).id() << " lives";
            }

            auto offspring = sexual_reproduction ? Genetic_AI(white, black) : Genetic_AI(winning_player, winning_player);
            offspring.mutate();
            original_pool[offspring] = pool_index;

            auto& losing_player  = (winner == WHITE ? black : white);
            losing_player = offspring; // offspring replaces loser
            std::cout << std::endl;
        }

        std::sort(pool.begin(), pool.end());
        write_generation(pools, pool_index, genome_file_name);

        purge_dead_from_map(pools, wins);
        purge_dead_from_map(pools, draws);
        purge_dead_from_map(pools, original_pool);

        // widths of columns for stats printout
        auto id_digits = int(std::floor(std::log10(pool.back().id()) + 1));

        // Write stat headers
        std::cout << '\n' << std::setw(id_digits + 1)  << "ID"
                  << std::setw(7) << "Wins"
                  << std::setw(8) << "Draws\n";

        // Write stats for each specimen
        for(const auto& ai : pool)
        {
            std::cout << std::setw(id_digits + 1) << ai.id();
            std::cout << std::setw(7) << wins[ai]
                      << std::setw(7) << draws[ai]
                      << (original_pool[ai] != pool_index ? " T" : "") << "\n";
        }

        // Record best AI from all pools.
        auto best_ai = pools.front().front();
        for(const auto& search_pool : pools)
        {
            // Taking the oldest AI (smallest ID) as the best
            best_ai = std::min(best_ai, *std::min_element(search_pool.begin(), search_pool.end()));
        }
        auto best_file_name = genome_file_name + "_best_genome.txt";
        auto temp_best_file_name = best_file_name + ".tmp";
        best_ai.print(temp_best_file_name);
        std::filesystem::rename(temp_best_file_name, best_file_name);

        // Update game time
        game_time += game_time_increment;
        if(game_time > maximum_game_time || game_time < minimum_game_time)
        {
            if(oscillating_time)
            {
                game_time_increment *= -1;
            }
            else
            {
                game_time_increment = 0;
            }
        }
        game_time = std::clamp(game_time, minimum_game_time, maximum_game_time);

        // Mix up the populations of all the gene pools
        if(pools.size() > 1 && pool_index == pools.size() - 1 && ++rounds_since_last_swap >= pool_swap_interval)
        {
            rounds_since_last_swap = 0;

            std::cout << "\n=======================\n\n";
            std::cout << "Shuffling pools ... ";

            std::vector<Genetic_AI> all_players;
            for(const auto& gene_pool : pools)
            {
                all_players.insert(all_players.end(), gene_pool.begin(), gene_pool.end());
            }

            Random::shuffle(all_players);
            pools.clear();
            auto begin_iter = all_players.begin();
            while(begin_iter != all_players.end())
            {
                auto end_iter = std::next(begin_iter, gene_pool_population);
                pools.emplace_back(begin_iter, end_iter);
                begin_iter = end_iter;
            }

            for(size_t new_pool_index = 0; new_pool_index < pools.size(); ++new_pool_index)
            {
                std::sort(pools[new_pool_index].begin(), pools[new_pool_index].end());
                write_generation(pools, new_pool_index, genome_file_name);
            }

            std::cout << "\n=======================\n" << std::endl;
        }

        // Pause gene pool
        #ifdef _WIN32
        if(quit_gene_pool)
        {
            std::cout << "Done." << std::endl;
            break;
        }
        #else
        auto gene_pool_pause_lock = std::unique_lock(pause_mutex, std::try_to_lock);
        if( ! gene_pool_pause_lock.owns_lock())
        {
            std::cout << "\nGene pool paused. Press " << pause_key << " to continue" << std::endl;
            std::cout << "or " << stop_key << " to quit." << std::endl;
            gene_pool_pause_lock.lock();
        }
        #endif // _WIN32
    }
}

namespace
{
    void pause_gene_pool(int)
    {
        #ifdef _WIN32
        quit_gene_pool = true;
        std::cout << "\nQuitting program after current games are finished ..." << std::endl;
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
            std::cout << "\nWaiting for games to end and be recorded before pausing ..." << std::endl;
        }
        #endif // _WIN32
    }

    void write_generation(const Gene_Pool_Set& pools, size_t pool_index, const std::string& genome_file_name)
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

        const auto& pool = pools.at(pool_index);
        for(const auto& ai : pool)
        {
            if( ! written_before[ai])
            {
                ai.print(ofs);
                written_before[ai] = true;
            }
        }

        ofs << "\nStill Alive: " << pool_index << " : ";
        for(const auto& ai : pool)
        {
            ofs << ai.id() << " ";
        }
        ofs << "\n\n" << std::flush;

        purge_dead_from_map(pools, written_before);
    }

    Gene_Pool_Set load_gene_pool_file(const std::string& load_file)
    {
        std::ifstream ifs(load_file);
        if( ! ifs)
        {
            std::cout << "Could not open file: " << load_file << std::endl;
            std::cout << "Starting with empty gene pool." << std::endl;
            return {};
        }

        std::map<size_t, std::string> still_alive;
        std::map<size_t, size_t> pool_line_numbers;
        std::map<size_t, std::string> pool_lines;

        std::string line;
        size_t line_number = 0;
        while(std::getline(ifs, line))
        {
            ++line_number;
            if(String::contains(line, "Still Alive"))
            {
                try
                {
                    auto parse = String::split(line, ":", 2);
                    auto pool_number = String::string_to_size_t(parse.at(1));
                    still_alive[pool_number] = parse.at(2);
                    pool_line_numbers[pool_number] = line_number;
                    pool_lines[pool_number] = line;
                }
                catch(const std::exception&)
                {
                    throw Bad_Still_Alive_Line(line_number, line);
                }
            }
        }

        auto largest_pool_number = still_alive.rbegin()->first;
        Gene_Pool_Set result(largest_pool_number + 1);
        for(const auto& [pool_number, ai_index_list] : still_alive)
        {
            for(const auto& ai_index_string : String::split(ai_index_list))
            {
                try
                {
                    result[pool_number].emplace_back(load_file, std::stoi(ai_index_string));
                }
                catch(const std::invalid_argument&)
                {
                    throw Bad_Still_Alive_Line(pool_line_numbers[pool_number], pool_lines[pool_number]);
                }
            }
        }

        for(size_t index = 0; index < result.size(); ++index)
        {
            write_generation(result, index, ""); // mark AIs from file as already written
        }

        return result;
    }

    template<typename Stat_Map>
    void purge_dead_from_map(const Gene_Pool_Set& pools, Stat_Map& stats)
    {
        Stat_Map new_stats;
        for(const auto& pool : pools)
        {
            for(const auto& ai : pool)
            {
                new_stats[ai] = stats[ai];
            }
        }
        stats = new_stats;
    }
}
