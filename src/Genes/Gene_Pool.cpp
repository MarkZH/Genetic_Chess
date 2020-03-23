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
#include <utility>
#include <string>

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

    using Gene_Pool_Set = std::vector<std::vector<Genetic_AI>>;

    Gene_Pool_Set load_gene_pool_file(const std::string& load_file);
    [[noreturn]] void throw_on_bad_still_alive_line(size_t line_number, const std::string& line);

    void pause_gene_pool(int);

    void write_generation(const Gene_Pool_Set& pools, const std::string& genome_file_name, bool force_write_still_alive);

    template<typename Stat_Map>
    void purge_dead_from_map(const Gene_Pool_Set& pools, Stat_Map& stats);

    int count_wins(const std::string& file_name, int id);
}

void gene_pool(const std::string& config_file)
{
    // Signal to pause gene pool
    signal(PAUSE_SIGNAL, pause_gene_pool);

    auto config = Configuration(config_file);

    // Environment variables
    const auto maximum_simultaneous_games = config.as_positive_number<size_t>("maximum simultaneous games");
    const auto gene_pool_population = config.as_positive_number<size_t>("gene pool population");
    const auto gene_pool_count = config.as_positive_number<size_t>("gene pool count");
    const auto pool_swap_interval = config.as_positive_number<size_t>("pool swap interval");
    const auto mating_reproduction = config.as_boolean("reproduction type", "mating", "cloning");
    const auto genome_file_name = config.as_text("gene pool file");
    const auto scramble_mutations = config.as_positive_number<int>("initial mutations");

    // Oscillating game time
    const auto minimum_game_time = Clock::seconds{config.as_positive_number<double>("minimum game time")};
    const auto maximum_game_time = Clock::seconds{config.as_positive_number<double>("maximum game time")};
    if(maximum_game_time < minimum_game_time)
    {
        std::cerr << "Minimum game time = " << minimum_game_time.count() << "\n";
        std::cerr << "Maximum game time = " << maximum_game_time.count() << "\n";
        throw std::invalid_argument("Maximum game time must be greater than the minimum game time.");
    }
    auto game_time_increment = Clock::seconds{config.as_number<double>("game time increment")};
    const auto oscillating_time = config.as_boolean("oscillating time", "yes", "no");

    auto seed_ai_specification = config.has_parameter("seed") ? config.as_text("seed") : std::string{};

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

    std::cout << "Loading gene pool file: " << genome_file_name << " ..." << std::endl;
    auto pools = load_gene_pool_file(genome_file_name);
    auto write_new_pools = pools.empty() || pools.front().size() != gene_pool_population;
    if(pools.empty() && ! seed_ai_specification.empty())
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
        pools = Gene_Pool_Set{gene_pool_count, {gene_pool_population, seed_ai}};
    }
    pools.resize(gene_pool_count);
    for(size_t i = 0; i < pools.size(); ++i)
    {
        auto new_ai_index = pools[i].size();
        pools[i].resize(gene_pool_population);
        for(auto ai_index = new_ai_index; ai_index < pools[i].size(); ++ai_index)
        {
            pools[i][ai_index].mutate(scramble_mutations);
        }
    }
    write_generation(pools, genome_file_name, write_new_pools);

    size_t last_pool = gene_pool_count;
    size_t rounds = 0; // Count of complete gene pool rounds where all pools have played a set of games
    if(auto genome_file = std::ifstream(genome_file_name))
    {
        std::string line;
        size_t line_number = 0;
        while(std::getline(genome_file, line))
        {
            line = String::trim_outer_whitespace(line);
            ++line_number;

            if(String::starts_with(line, "Still Alive"))
            {
                try
                {
                    auto alive_split = String::split(line, ":");
                    last_pool = String::to_number<size_t>(alive_split.at(1));
                    if(last_pool == gene_pool_count - 1)
                    {
                        ++rounds;
                    }
                }
                catch(const std::exception&)
                {
                    throw_on_bad_still_alive_line(line_number, line);
                }
            }
        }
    }
    auto rounds_since_last_swap = rounds % pool_swap_interval;
    const auto starting_pool = (last_pool + 1) % gene_pool_count;

    const auto game_record_file = genome_file_name +  "_games.pgn";
    auto game_time = minimum_game_time;

    std::cout << "Searching for previous best AI win counts ..." << std::endl;
    const auto best_file_name = genome_file_name + "_best_genome.txt";
    auto best_id = 0;
    auto wins_to_beat = 0.0;
    try
    {
        auto best = Genetic_AI(best_file_name, find_last_id(best_file_name));
        best_id = best.id();
        wins_to_beat = count_wins(game_record_file, best_id);
    }
    catch(...)
    {
    }

    if(auto ifs = std::ifstream(game_record_file))
    {
        // Use game time from last run of this gene pool
        std::cout << "Searching " << game_record_file << " for last game time and stats ..." << std::endl;
        std::string line;
        std::string time_line;
        std::string previous_time_line;
        while(std::getline(ifs, line))
        {
            line = String::trim_outer_whitespace(line);
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
                    color_wins[static_cast<unsigned>(Piece_Color::WHITE)]++;
                }
                else if(result == "0-1")
                {
                    color_wins[static_cast<unsigned>(Piece_Color::BLACK)]++;
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
            game_time = String::to_duration<Clock::seconds>(String::split(time_line, "\"").at(1));
            game_time = std::clamp(game_time, minimum_game_time, maximum_game_time);
        }

        auto previous_game_time = game_time;
        if( ! previous_time_line.empty())
        {
            previous_game_time = String::to_duration<Clock::seconds>(String::split(previous_time_line, "\"").at(1));
        }

        if(previous_game_time > game_time)
        {
            game_time_increment = -std::chrono::abs(game_time_increment);
        }

        game_time = std::clamp(game_time + game_time_increment, minimum_game_time, maximum_game_time);

        std::cout << "Done." << std::endl;
    }

    gene_pool_started = true;
    for(size_t pool_index = starting_pool; true; pool_index = (pool_index + 1) % pools.size()) // run forever
    {
        // Pause gene pool
        #ifdef _WIN32
        if(quit_gene_pool)
        {
            std::cout << "Done." << std::endl;
            break;
        }
        #else
        if(auto pause_lock = std::unique_lock(pause_mutex, std::try_to_lock); ! pause_lock.owns_lock())
        {
            std::cout << "\nGene pool paused. Press " << pause_key << " to continue" << std::endl;
            std::cout << "or " << stop_key << " to quit." << std::endl;
            pause_lock.lock();
        }
        #endif // _WIN32

        auto& pool = pools[pool_index];

        // Write overall stats
        std::cout << "\n=======================\n\n"
                  << "Gene pool ID: " << pool_index
                  << "  Gene pool size: " << pool.size()
                  << "  Rounds since pool swaps: " << rounds_since_last_swap << "/" << pool_swap_interval
                  << "\nGames: " << color_wins[static_cast<unsigned>(Piece_Color::WHITE)] + color_wins[static_cast<unsigned>(Piece_Color::BLACK)] + draw_count
                  << "  White wins: " << color_wins[static_cast<unsigned>(Piece_Color::WHITE)]
                  << "  Black wins: " << color_wins[static_cast<unsigned>(Piece_Color::BLACK)]
                  << "  Draws: " << draw_count
                  << "\nTime: " << game_time.count() << " sec"
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
            results.emplace_back(std::async(std::launch::async, play_game,
                                            Board{},
                                            Clock(game_time),
                                            white, black,
                                            false,
                                            "Gene pool",
                                            "Local computer",
                                            game_record_file));
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
            std::cout << color_text(winner) << " (" << result.ending_reason() << ")" << std::endl;

            if(winner != Winner_Color::NONE)
            {
                auto winning_player = (winner == Winner_Color::WHITE ? white : black);
                color_wins[static_cast<unsigned>(winner)]++;
                wins[winning_player]++;

                auto offspring = mating_reproduction ? Genetic_AI(white, black) : Genetic_AI(winning_player, winning_player);
                offspring.mutate();

                auto& losing_player  = (winner == Winner_Color::WHITE ? black : white);
                losing_player = offspring; // offspring replaces loser
            }
            else
            {
                draws[white]++;
                draws[black]++;
                ++draw_count;
            }
        }

        std::sort(pool.begin(), pool.end());
        write_generation(pools, genome_file_name, false);

        purge_dead_from_map(pools, wins);
        purge_dead_from_map(pools, draws);

        // widths of columns for stats printout
        auto id_digits = std::to_string(pool.back().id()).size();

        // Write stat headers
        std::cout << '\n' << std::setw(id_digits + 1)  << "ID"
                  << std::setw(7) << "Wins"
                  << std::setw(7) << "Draws" << "\n";

        // Write stats for each specimen
        for(const auto& ai : pool)
        {
            std::cout << std::setw(id_digits + 1) << ai.id();
            std::cout << std::setw(7) << wins[ai]
                      << std::setw(7) << draws[ai] << "\n";
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
                ai.print(temp_best_file_name);
                std::filesystem::rename(temp_best_file_name, best_file_name);
            }
        }

        std::cout << "\nWins to be recorded as best: " << wins_to_beat
                  << "\nBest ID: " << best_id << "\n";

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
                game_time_increment = 0s;
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

            for(auto& new_pool : pools)
            {
                std::sort(new_pool.begin(), new_pool.end());
            }
            write_generation(pools, genome_file_name, true);

            std::cout << "\n=======================\n" << std::endl;
        }
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

    void write_generation(const Gene_Pool_Set& pools, const std::string& genome_file_name, bool force_write_still_alive)
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

        for(size_t i = 0; i < pools.size(); ++i)
        {
            auto needs_still_alive_line = false;
            const auto& pool = pools.at(i);
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
                ofs << "\nStill Alive: " << i << " : ";
                for(const auto& ai : pool)
                {
                    ofs << ai.id() << " ";
                }
                ofs << "\n\n" << std::flush;
            }
        }

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

        std::map<size_t, std::string> still_alive; // pool number -> ID list
        std::map<size_t, size_t> pool_line_numbers; // pool number --> gene pool file line number (error reporting)
        std::map<size_t, std::string> pool_lines; // pool number --> line in gene pool file (error reporting)

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
                    auto pool_number = String::to_number<size_t>(parse.at(1));
                    still_alive[pool_number] = parse.at(2);
                    pool_line_numbers[pool_number] = line_number;
                    pool_lines[pool_number] = line;
                }
                catch(...)
                {
                    throw_on_bad_still_alive_line(line_number, line);
                }
            }
        }

        if(pool_lines.empty())
        {
            std::cout << "No \"Still Alive\" lines found. Starting with empty gene pool.";
            return {};
        }

        std::map<int, size_t> pool_assignments; // ID --> pool number
        for(const auto& [pool_number, id_list] : still_alive)
        {
            for(const auto& id_string : String::split(id_list))
            {
                try
                {
                    pool_assignments[std::stoi(id_string)] = pool_number;
                }
                catch(...)
                {
                    throw_on_bad_still_alive_line(pool_line_numbers[pool_number], pool_lines[pool_number]);
                }
            }
        }

        ifs = std::ifstream(load_file);
        bool search_started_from_beginning_of_file = true;
        Gene_Pool_Set result(still_alive.size());
        for(auto [id, pool_number] : pool_assignments)
        {
            while(true)
            {
                try
                {
                    result[pool_number].emplace_back(ifs, id);
                    search_started_from_beginning_of_file = false;
                    break;
                }
                catch(const Genetic_AI_Creation_Error& e)
                {
                    if(search_started_from_beginning_of_file)
                    {
                        std::cerr << e.what() << load_file << "\n";
                        throw_on_bad_still_alive_line(pool_line_numbers[pool_number], pool_lines[pool_number]);
                    }
                    else
                    {
                        ifs = std::ifstream(load_file);
                        search_started_from_beginning_of_file = true;
                        continue;
                    }
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

    int count_wins(const std::string& file_name, int id)
    {
        auto input = std::ifstream(file_name);
        if( ! input)
        {
            return 0;
        }

        std::string line;
        auto win_count = 0;
        while(std::getline(input, line))
        {
            line = String::remove_extra_whitespace(line);
            auto is_white_player = String::starts_with(line, "[White");
            auto is_black_player = String::starts_with(line, "[Black");
            if(is_white_player || is_black_player)
            {
                auto player_id = std::stoi(String::split(String::split(line, "\"").at(1)).at(2));
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
                                break;
                            }
                        }
                    }
                }
            }
        }

        return win_count;
    }
}
