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

#include "Players/Minimax_AI.h"

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
    bool keep_going();

    std::vector<Minimax_AI> load_gene_pool_file(const std::string& load_file);
    [[noreturn]] void throw_on_bad_still_alive_line(size_t line_number, const std::string& line);

    void pause_gene_pool(int);

    void write_generation(const std::vector<Minimax_AI>& pool, const std::string& genome_file_name, bool force_write_still_alive);

    template<typename Stat_Map>
    void purge_dead_from_map(const std::vector<Minimax_AI>& pool, Stat_Map& stats);

    size_t count_still_alive_lines(const std::string& genome_file_name) noexcept;
    int count_wins(const std::string& file_name, int id);
    std::vector<Minimax_AI> fill_pool(const std::string& genome_file_name, size_t gene_pool_population, const std::string& seed_ai_specification, size_t mutation_rate);
    void load_previous_game_stats(const std::string& game_record_file, Clock::seconds& game_time, std::array<size_t, 3>& color_wins);
    struct best_ai_stats { int id = 0; int wins = 0; double wins_to_beat = 0.0; };
    best_ai_stats recall_previous_best_stats(const std::string& best_file_name, const std::string& game_record_file) noexcept;
    void update_best_stats(best_ai_stats& best_stats, const std::vector<Minimax_AI>& pool, std::map<Minimax_AI, int>& wins, const std::string& best_file_name) noexcept;
    void print_round_header(const std::vector<Minimax_AI>& pool,
                            const std::string& genome_file_name,
                            const std::array<size_t, 3>& color_wins,
                            size_t round_count,
                            size_t first_mutation_interval,
                            size_t second_mutation_interval,
                            size_t mutation_rate,
                            Clock::seconds game_time) noexcept;
    void print_verbose_output(const std::stringstream& result_printer, const std::vector<Minimax_AI>& pool, std::map<Minimax_AI, int>& wins, std::map<Minimax_AI, int>& draws);
}

void gene_pool(const std::string& config_file)
{
    signal(PAUSE_SIGNAL, pause_gene_pool);

    const auto config = Configuration(config_file);

    const auto maximum_simultaneous_games = config.as_positive_number<int>("maximum simultaneous games");
    const auto gene_pool_population = config.as_positive_number<size_t>("gene pool population");
    const auto roaming_distance = config.as_positive_number<double>("roaming distance");
    const auto genome_file_name = config.as_text("gene pool file");

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

    const auto board_fen = config.as_text_or_default("FEN", Board().fen());
    const auto seed_ai_specification = config.as_text_or_default("seed", "");
    const auto verbose_output = config.as_boolean("output volume", "verbose", "quiet");

    if(config.any_unused_parameters())
    {
        std::cout << "There were unused parameters in the file: " << config_file << std::endl;
        config.print_unused_parameters();
        std::cout << "\nPress enter to continue or " << stop_key << " to quit ..." << std::endl;
        std::cin.get();
    }

    auto round_count = count_still_alive_lines(genome_file_name);
    auto pool = fill_pool(genome_file_name, gene_pool_population, seed_ai_specification, first_mutation_rate);

    const auto game_record_file = genome_file_name + "_games.pgn";
    auto game_time = game_time_increment > 0.0s ? minimum_game_time : maximum_game_time;
    std::array<size_t, 3> color_wins{}; // indexed with [Winner_Color]
    load_previous_game_stats(game_record_file, game_time, color_wins);

    std::map<Minimax_AI, int> wins;
    std::map<Minimax_AI, int> draws;

    const auto best_file_name = genome_file_name + "_best_genome.txt";
    auto best_stats = recall_previous_best_stats(best_file_name, game_record_file);

    while(keep_going())
    {
        const auto mutation_phase = round_count++ % (first_mutation_interval + second_mutation_interval);
        const auto mutation_rate = mutation_phase < first_mutation_interval ? first_mutation_rate : second_mutation_rate;

        print_round_header(pool, genome_file_name, color_wins, round_count, first_mutation_interval, second_mutation_interval, mutation_rate, game_time);

        // The shuffled pool list determines the match-ups. After shuffling the list,
        // adjacent AIs are matched as opponents.
        Random::stir_order(pool, roaming_distance);
        std::vector<std::future<Game_Result>> results;
        for(size_t index = 0; index < gene_pool_population; index += 2)
        {
            while(int(results.size()) >= maximum_simultaneous_games)
            {
                const auto in_progress_games =
                    std::count_if(results.begin(),
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
            auto board = Board(board_fen);
            auto clock = Clock(game_time, 0, Clock::seconds(0.0), Time_Reset_Method::ADDITION, board.whose_turn());
            results.emplace_back(std::async(std::launch::async, play_game,
                                            board,
                                            clock,
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

            const auto result = results[index/2].get();
            const auto winner = result.winner();
            result_printer << white.id() << " vs " << black.id() << ": " << color_text(winner) << " (" << result.ending_reason() << ")\n";

            const auto mating_winner = (winner == Winner_Color::NONE ? (Random::coin_flip() ? Winner_Color::WHITE : Winner_Color::BLACK) : winner);
            const auto& winning_player = (mating_winner == Winner_Color::WHITE ? white : black);
            auto& losing_player = (winning_player.id() == white.id() ? black : white);

            auto offspring = Minimax_AI(white, black);
            offspring.mutate(mutation_rate);
            losing_player = offspring;

            ++color_wins[static_cast<int>(winner)];
            ++(winner == Winner_Color::NONE ? draws : wins)[winning_player];
        }

        write_generation(pool, genome_file_name, false);
        purge_dead_from_map(pool, wins);
        purge_dead_from_map(pool, draws);
        update_best_stats(best_stats, pool, wins, best_file_name);

        if(verbose_output)
        {
            print_verbose_output(result_printer, pool, wins, draws);
        }

        game_time = std::clamp(game_time + game_time_increment, minimum_game_time, maximum_game_time);
    }
    std::cout << "Done." << std::endl;
}

namespace
{
    bool keep_going()
    {
    #ifdef _WIN32
        return ! quit_gene_pool;
    #else
        if(auto pause_lock = std::unique_lock(pause_mutex, std::try_to_lock); ! pause_lock.owns_lock())
        {
            std::cout << "\nGene pool paused. Press " << pause_key << " to continue ";
            std::cout << "or " << stop_key << " to quit." << std::endl;
            pause_lock.lock();
        }
        return true;
    #endif // _WIN32
    }

    void pause_gene_pool(int)
    {
        std::cout << "\nGetting to a good stopping point ..." << std::endl;
    #ifdef _WIN32
        quit_gene_pool = true;
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
        }
    #endif // _WIN32
    }

    std::vector<Minimax_AI> fill_pool(const std::string& genome_file_name, size_t gene_pool_population, const std::string& seed_ai_specification, size_t mutation_rate)
    {
        std::cout << "Loading gene pool file: " << genome_file_name << " ..." << std::endl;
        auto pool = load_gene_pool_file(genome_file_name);
        const auto write_new_pools = pool.size() != gene_pool_population;
        if(pool.empty() && ! seed_ai_specification.empty())
        {
            const auto seed_split = String::split(seed_ai_specification, "/");
            if(seed_split.size() > 2)
            {
                throw std::runtime_error("Too many parameters in the seed configuration\nseed = " + seed_ai_specification);
            }
            const auto file_name = seed_split.front();
            const auto seed_id = seed_split.size() == 2 ? String::to_number<int>(seed_split.back()) : find_last_id(file_name);
            const auto seed_ai = Minimax_AI(file_name, seed_id);
            std::cout << "Seeding with #" << seed_ai.id() << " from file " << file_name << std::endl;
            pool = {seed_ai};
        }
        const auto new_ai_index = pool.size();
        pool.resize(gene_pool_population);
        for(auto ai_index = new_ai_index; ai_index < pool.size(); ++ai_index)
        {
            pool[ai_index].mutate(mutation_rate);
        }
        write_generation(pool, genome_file_name, write_new_pools);
        return pool;
    }

    void write_generation(const std::vector<Minimax_AI>& pool, const std::string& genome_file_name, bool force_write_still_alive)
    {
        static std::map<Minimax_AI, bool> written_before;
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

        auto sorted_pool = pool;
        std::sort(sorted_pool.begin(), sorted_pool.end());
        auto needs_still_alive_line = false;
        for(const auto& ai : sorted_pool)
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

    void print_round_header(const std::vector<Minimax_AI>& pool,
                            const std::string& genome_file_name,
                            const std::array<size_t, 3>& color_wins, \
                            const size_t round_count,
                            const size_t first_mutation_interval,
                            const size_t second_mutation_interval,
                            const size_t mutation_rate,
                            const Clock::seconds game_time) noexcept
    {
        std::cout << "\n=======================\n\n"
                  << "Gene pool size: " << pool.size()
                  << "  Gene pool file name: " << genome_file_name
                  << "\nGames: " << std::accumulate(color_wins.begin(), color_wins.end(), size_t{0})
                  << "  White wins: " << color_wins[static_cast<int>(Winner_Color::WHITE)]
                  << "  Black wins: " << color_wins[static_cast<int>(Winner_Color::BLACK)]
                  << "  Draws: " << color_wins[static_cast<int>(Winner_Color::NONE)]
                  << "\nRounds: " << round_count
                  << "  Mutation rate phase: " << round_count % (first_mutation_interval + second_mutation_interval)
                  << " (" << first_mutation_interval << "/" << second_mutation_interval << ")"
                  << "\nMutation rate: " << mutation_rate << "  Game time: " << game_time.count() << " sec\n\n";

    #ifdef _WIN32
        std::cout << "Quit after this round: " << stop_key << "    Abort: " << stop_key << " " << stop_key << "\n" << std::endl;
    #else
        std::cout << "Pause: " << pause_key << "    Abort: " << stop_key << "\n" << std::endl;
    #endif // _WIN32
    }

    void print_verbose_output(const std::stringstream& result_printer, const std::vector<Minimax_AI>& pool, std::map<Minimax_AI, int>& wins, std::map<Minimax_AI, int>& draws)
    {
        std::cout << result_printer.str();

        // widths of columns for stats printout
        const auto largest_id = std::max_element(pool.begin(), pool.end())->id();
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
                      << std::setw(win_column_width) << wins[ai]
                      << std::setw(draw_column_width) << draws[ai] << "\n";
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
        std::cout << "Searching " << game_record_file << " for last game time and stats ..." << std::endl;
        for(std::string line; std::getline(ifs, line);)
        {
            line = String::trim_outer_whitespace(line);
            if(line.starts_with("[TimeControl"))
            {
                game_time = String::to_duration<Clock::seconds>(String::split(line, "\"").at(1));
            }
            else if(line.starts_with("[Result"))
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

    best_ai_stats recall_previous_best_stats(const std::string& best_file_name, const std::string& game_record_file) noexcept
    {
        try
        {
            const auto best = Genetic_AI(best_file_name, find_last_id(best_file_name));
            std::cout << "Searching for previous best AI win counts ..." << std::endl;
            auto wins = count_wins(game_record_file, best.id());
            return {best.id(), wins, double(wins)};
        }
        catch(...)
        {
            return {};
        }
    }

    void update_best_stats(best_ai_stats& best_stats, const std::vector<Minimax_AI>& pool, std::map<Minimax_AI, int>& wins, const std::string& best_file_name) noexcept
    {
        // Slowly reduce the wins required to be recorded as best to allow
        // later AIs that are playing against a better field to be recorded.
        const double decay_constant = 0.99;
        best_stats.wins_to_beat *= decay_constant;

        const auto& winningest_live_ai =
            *std::max_element(pool.begin(), pool.end(),
                              [&wins](const auto& a, const auto& b)
                              {
                                  return wins[a] < wins[b];
                              });
        const auto win_count = wins[winningest_live_ai];

        if(win_count > best_stats.wins_to_beat)
        {
            static const auto temp_best_file_name = best_file_name + ".tmp";
            best_stats.wins_to_beat = win_count;
            best_stats.id = winningest_live_ai.id();
            best_stats.wins = win_count;
            winningest_live_ai.print(temp_best_file_name);
            std::filesystem::rename(temp_best_file_name, best_file_name);
        }

        std::cout << "\nWins to be recorded as best: " << best_stats.wins_to_beat
                  << "\nBest ID: " << best_stats.id << " with " << best_stats.wins << " win" << (best_stats.wins != 1 ? "s" : "") << "\n";
    }

    std::vector<Minimax_AI> load_gene_pool_file(const std::string& load_file)
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

        const auto id_strings = String::split(still_alive);
        std::vector<int> ids;
        try
        {
            std::transform(id_strings.begin(),
                           id_strings.end(),
                           std::back_inserter(ids),
                           [](const auto& s) { return std::stoi(s); });
        }
        catch(...)
        {
            throw_on_bad_still_alive_line(pool_line_number, pool_line);
        }

        auto sorted_ids = ids;
        std::sort(sorted_ids.begin(), sorted_ids.end());


        ifs = std::ifstream(load_file);
        bool search_started_from_beginning_of_file = true;
        std::vector<Minimax_AI> result;
        for(auto id : sorted_ids)
        {
            while(true)
            {
                try
                {
                    result.emplace_back(ifs, id);
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
            }
        }

        auto result_destination = result.begin();
        for(auto id : ids)
        {
            const auto result_source = std::find_if(result_destination, result.end(), [id](const auto& ai) { return ai.id() == id; });
            std::iter_swap(result_destination++, result_source);
        }

        std::vector<int> loaded_ids;
        std::transform(result.begin(), result.end(), std::back_inserter(loaded_ids), [](const auto& ai) { return ai.id(); });
        if(ids != loaded_ids)
        {
            std::cerr << "Mismatch in file IDs vs. loaded IDs.";
            std::cerr << "Still Alive IDs:\n";
            auto osi = std::ostream_iterator<int>(std::cout, " ");
            std::copy(ids.begin(), ids.end(), osi);
            std::cerr << "\nLoaded IDs:\n";
            std::copy(loaded_ids.begin(), loaded_ids.end(), osi);
            throw_on_bad_still_alive_line(pool_line_number, pool_line);
        }

        write_generation(result, "", false); // mark AIs from file as already written
        return result;
    }

    void throw_on_bad_still_alive_line(size_t line_number, const std::string& line)
    {
        throw std::runtime_error("Invalid \"Still Alive\" line (line# " + std::to_string(line_number) + "): " + line);
    }

    template<typename Stat_Map>
    void purge_dead_from_map(const std::vector<Minimax_AI>& pool, Stat_Map& stats)
    {
        Stat_Map new_stats;
        for(const auto& ai : pool)
        {
            new_stats[ai] = stats[ai];
        }
        stats = new_stats;
    }

    size_t count_still_alive_lines(const std::string& genome_file_name) noexcept
    {
        auto genome_file = std::ifstream(genome_file_name);
        if( ! genome_file)
        {
            return 0;
        }

        size_t round_count = 0;
        for(std::string line; std::getline(genome_file, line);)
        {
            line = String::trim_outer_whitespace(line);
            if(line.starts_with("Still Alive"))
            {
                ++round_count;
            }
        }

        return round_count;
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
            const auto is_white_player = line.starts_with("[White");
            const auto is_black_player = line.starts_with("[Black");
            if(is_white_player || is_black_player)
            {
                const auto player_id = String::to_number<int>(String::split(String::split(line, "\"").at(1)).at(2));
                if(player_id == id)
                {
                    while(std::getline(input, line))
                    {
                        line = String::remove_extra_whitespace(line);
                        if(line.starts_with("[Result"))
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
