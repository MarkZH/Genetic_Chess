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
#include <array>

#include "Players/Genetic_AI.h"
#include "Game/Game.h"
#include "Game/Game_Result.h"
#include "Utility.h"

const auto NO_SIGNAL = 0;
const std::string stop_key = "Ctrl-c";

#ifdef __linux__
const auto PAUSE_SIGNAL = SIGTSTP;
const std::string pause_key = "Ctrl-z";
#elif defined(_WIN32)
const auto PAUSE_SIGNAL = SIGBREAK;
const std::string pause_key = "Ctrl-Break";
#endif

static sig_atomic_t signal_activated = NO_SIGNAL;
static bool gene_pool_paused = false;

void pause_gene_pool(int signal);

void write_generation(const std::vector<Gene_Pool>& pools, size_t pool_index, const std::string& genome_file_name);

template<typename Stat_Map>
void purge_dead_from_map(const std::vector<Gene_Pool>& pools, Stat_Map& stats);

void purge_dead_from_map(const std::vector<Gene_Pool>& pools, std::vector<std::vector<Genetic_AI>>& ai_lists);


void gene_pool(const std::string& config_file = "")
{
    auto config = Configuration_File(config_file);

    // Environment variables
    const auto maximum_simultaneous_games = config.as_number("maximum simultaneous games");
    const auto gene_pool_population = size_t(config.as_number("gene pool population"));
    const auto gene_pool_count = size_t(config.as_number("gene pool count"));
    const auto pool_swap_interval = size_t(config.as_number("pool swap interval"));

    // Oscillating game time
    const double minimum_game_time = config.as_number("minimum game time"); // seconds
    const double maximum_game_time = config.as_number("maximum game time"); // seconds
    double game_time_increment = config.as_number("game time increment"); // seconds
    const bool oscillating_time = config.as_boolean("oscillating time", "yes", "no");
    double game_time = minimum_game_time;

    // Stats (Pool ID --> counts)
    std::vector<int> game_count(gene_pool_count);
    std::array<std::vector<int>, 2> color_wins; // indexed with [Color][pool_index]
    color_wins.fill(std::vector<int>(gene_pool_population, 0));
    std::vector<int> draw_count(gene_pool_count);

    std::map<size_t, int> most_wins;
    std::map<size_t, Genetic_AI> most_wins_player;

    std::map<size_t, int> most_games_survived;
    std::map<size_t, Genetic_AI> most_games_survived_player;

    // Individual Genetic AI stats
    std::map<Genetic_AI, int> wins;
    std::map<Genetic_AI, int> draws;
    std::map<Genetic_AI, int> games_since_last_win;
    std::map<Genetic_AI, int> consecutive_wins;
    std::map<Genetic_AI, size_t> original_pool;

    const int scramble_mutations = 100;

    std::string genome_file_name = config.as_text("gene pool file");
    if(genome_file_name.empty())
    {
        genome_file_name = "gene_pool_record_";
        for(int i = 0; i < 10; ++i)
        {
            genome_file_name += char('a' + Random::random_integer(0, 25));
        }
        genome_file_name += ".txt";
    }

    auto pools = load_gene_pool_file(genome_file_name);
    while(pools.size() < gene_pool_count)
    {
        pools.push_back({});
    }

    for(size_t i = 0; i < pools.size(); ++i)
    {
        while(pools[i].size() < gene_pool_population)
        {
            pools[i].push_back(Genetic_AI());
            pools[i].back().set_origin_pool(int(i));
            pools[i].back().mutate(scramble_mutations);
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

    std::string game_record_file = genome_file_name +  "_games.txt";
    if(auto ifs = std::ifstream(game_record_file))
    {
        // Use game time from last run of this gene pool
        std::cout << "Searching " << game_record_file << " for last game time ..." << std::endl;
        std::string line;
        std::string time_line;
        std::string previous_time_line;
        while(std::getline(ifs, line))
        {
            if(String::contains(line, "TimeControl"))
            {
                if(line != time_line)
                {
                    previous_time_line = time_line;
                    time_line = line;
                }
            }
        }

        if( ! time_line.empty())
        {
            game_time = std::stod(String::split(time_line, "\"").at(1));
            game_time = Math::clamp(game_time, minimum_game_time, maximum_game_time);
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

        std::cout << "Done." << std::endl;
    }

    config.print_unused_parameters();

    // Signal to pause gene pool
    signal(PAUSE_SIGNAL, pause_gene_pool);

    for(size_t pool_index = 0; true; pool_index = (pool_index + 1) % pools.size()) // run forever
    {
        auto& pool = pools[pool_index];

        // Write overall stats
        std::cout << "\nGene pool ID: " << pool_index
                  << "  Gene pool size: " << pool.size()
                  << "\nGames: " << game_count[pool_index]
                  << "  White wins: " << color_wins[WHITE][pool_index]
                  << "  Black wins: " << color_wins[BLACK][pool_index]
                  << "  Draws: " << draw_count[pool_index]
                  << "\nTime: " << game_time << " sec"
                  << "   Gene pool file name: " << genome_file_name << "\n\n";

        std::cout << "Pause: " << pause_key << "    Abort: " << stop_key << "\n" << std::endl;

        // The shuffled pool list determines the match-ups. After shuffling the list,
        // adjacent AIs are matched as opponents.
        Random::shuffle(pool);

        std::vector<std::future<Game_Result>> results; // map from matchups to winners (half the size of pool)
        for(size_t index = 0; index < gene_pool_population; index += 2)
        {
            auto& white = pool[index];
            auto& black = pool[index + 1];
            results.emplace_back(std::async(std::launch::async,
                                            play_game, white, black, game_time, 0, 0, game_record_file));

            // Limit the number of simultaneous games by waiting for earlier games to finish
            // before starting a new one.
            while(true)
            {
                auto in_progress_games = std::count_if(results.begin(),
                                                       results.end(),
                                                       [](const auto& r)
                                                       { return r.wait_for(std::chrono::seconds(0)) != std::future_status::ready; });

                if(in_progress_games >= maximum_simultaneous_games)
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                else
                {
                    break;
                }
            }
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

            if(winner != NONE)
            {
                color_wins[winner][pool_index]++;
                auto& winning_player = (winner == WHITE ? white : black);
                wins[winning_player]++;
                games_since_last_win[winning_player] = 0;
                consecutive_wins[winning_player]++;
                if(wins[winning_player] >= most_wins[pool_index])
                {
                    most_wins[pool_index] = wins[winning_player];
                    if(most_wins_player.count(pool_index))
                    {
                        most_wins_player[pool_index] = winning_player;
                    }
                    else
                    {
                        most_wins_player.emplace(pool_index, winning_player);
                    }
                }
            }
            else
            {
                draws[white]++;
                draws[black]++;
                games_since_last_win[white]++;
                games_since_last_win[black]++;
                consecutive_wins[white] = 0;
                consecutive_wins[black] = 0;
                ++draw_count[pool_index];

                // Draw results in random player being replaced
                winner = (Random::coin_flip() ? WHITE : BLACK);
                std::cout << " --> " << (winner == WHITE ? black : white).id() << " dies";
            }

            auto offspring = Genetic_AI(white, black);
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
        purge_dead_from_map(pools, games_since_last_win);
        purge_dead_from_map(pools, consecutive_wins);
        purge_dead_from_map(pools, original_pool);

        for(const auto& ai : pool)
        {
            auto games_survived = wins[ai] + draws[ai];
            if(games_survived >= most_games_survived[pool_index])
            {
                most_games_survived[pool_index] = games_survived;
                if(most_games_survived_player.count(pool_index))
                {
                    most_games_survived_player[pool_index] = ai;
                }
                else
                {
                    most_games_survived_player.emplace(pool_index, ai);
                }
            }
        }

        // widths of columns for stats printout
        auto max_id = pool.back().id();
        auto id_digits = int(std::floor(std::log10(max_id) + 1));

        // Write results
        std::cout << '\n' << std::setw(id_digits + 1)  << "ID"
                  << std::setw(7)  << "Wins"
                  << std::setw(8) << "Streak"
                  << std::setw(7)  << "Draws"
                  << std::setw(9)  << "Streak\n";

        // Write stats for each specimen
        for(const auto& ai : pool)
        {
            std::cout << std::setw(id_digits + 1) << ai.id();
            std::cout << std::setw(7)    << wins[ai]
                      << std::setw(8)   << consecutive_wins[ai]
                      << std::setw(7)    << draws[ai]
                      << std::setw(8)   << games_since_last_win[ai]
                      << (original_pool[ai] != pool_index ? " T" : "") << "\n";
        }
        std::cout << std::endl;

        std::cout << "Most wins:     " << most_wins[pool_index]
                  << " by ID " << most_wins_player[pool_index].id() << std::endl;
        std::cout <<   "Longest lived: " << most_games_survived[pool_index]
                  << " by ID " << most_games_survived_player[pool_index].id() << std::endl;

        // Record best AI from all pools.
        auto best_ai = pool.front();
        auto best_compare = [&wins, &draws](const auto& x, const auto& y)
                            {
                                return wins[x] - draws[x] < wins[y] - draws[y];
                            };
        for(const auto& search_pool : pools)
        {
            best_ai = std::max(best_ai,
                               *std::max_element(search_pool.begin(),
                                                 search_pool.end(),
                                                 best_compare), best_compare);
        }
        std::ofstream best_file(genome_file_name + "_best_genome.txt");
        best_ai.print(best_file);

        // Pause gene pool
        if(signal_activated == PAUSE_SIGNAL)
        {
            gene_pool_paused = true;
            std::cout << "\nGene pool paused. Press " << pause_key << " to continue" << std::endl;
            std::cout << "or " << stop_key << " to quit." << std::endl;
            while(signal_activated == PAUSE_SIGNAL)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            gene_pool_paused = false;
        }

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
        game_time = Math::clamp(game_time, minimum_game_time, maximum_game_time);

        game_count[pool_index] += int(results.size());

        // Transfer best players between gene pools to keep pools
        // from stagnating or amplifying pathological behavior
        if(pools.size() > 1 && pool_index == pools.size() - 1) // all pools have equal number of games
        {
            static size_t previous_mod = 0;
            auto this_mod = game_count[pool_index] % pool_swap_interval;
            if(this_mod < previous_mod)
            {
                // Replace player with least wins in each pool with clone of winner from pool to left
                std::cout << std::endl;
                std::vector<Genetic_AI> winners;
                for(const auto& source_pool : pools)
                {
                    winners.push_back(*std::max_element(source_pool.begin(),
                                                        source_pool.end(),
                                                        best_compare));
                }

                for(size_t source_pool_index = 0; source_pool_index < pools.size(); ++source_pool_index)
                {
                    auto dest_pool_index = (source_pool_index + 1) % pools.size();
                    auto& dest_pool = pools[dest_pool_index];
                    auto& loser = *std::min_element(dest_pool.begin(), dest_pool.end(), best_compare);
                    std::cout << "Sending ID "
                              << winners[source_pool_index].id()
                              << " to pool "
                              << dest_pool_index << std::endl;
                    loser = winners[source_pool_index]; // winner replaces loser in destination pool
                }
            }

            previous_mod = this_mod;
        }
    }
}

void pause_gene_pool(int signal)
{
    // Second time signal activated
    if(signal == signal_activated)
    {
        signal_activated = NO_SIGNAL;
        if( ! gene_pool_paused)
        {
            std::cout << "\nNo longer pausing." << std::endl;
        }
        return;
    }

    signal_activated = signal;

    if( ! gene_pool_paused)
    {
        std::cout << "\nWaiting for games to end and be recorded before pausing ..." << std::endl;
    }
}

void write_generation(const std::vector<Gene_Pool>& pools, size_t pool_index, const std::string& genome_file_name)
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

    auto pool = pools.at(pool_index);
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

std::vector<Gene_Pool> load_gene_pool_file(const std::string& load_file)
{
    std::ifstream ifs(load_file);
    if( ! ifs)
    {
        std::cout << "Could not open file: " << load_file << std::endl;
        std::cout << "Starting with empty gene pool." << std::endl;
        return std::vector<Gene_Pool>();
    }

    std::cout << "Loading gene pool file: " << load_file << " ..." << std::endl;
    std::map<int, std::string> still_alive;
    std::string line;
    while(std::getline(ifs, line))
    {
        if(String::contains(line, "Still Alive: "))
        {
            auto parse = String::split(line, ":");
            still_alive[std::stoi(parse[1])] = parse[2];
        }
    }
    ifs.close();

    std::vector<Gene_Pool> result(still_alive.size());
    for(const auto& index_list : still_alive)
    {
        for(const auto& number_string : String::split(index_list.second))
        {
            if(number_string.empty())
            {
                continue;
            }

            auto index = std::stoi(number_string);
            result[index_list.first].push_back(Genetic_AI(load_file, index));
        }
        write_generation(result, index_list.first, ""); // mark AIs from file as already written
    }
    std::cout << "Done." << std::endl;

    return result;
}

template<typename Stat_Map>
void purge_dead_from_map(const std::vector<Gene_Pool>& pools, Stat_Map& stats)
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

void purge_dead_from_map(const std::vector<Gene_Pool>& pools, std::vector<std::vector<Genetic_AI>>& ai_lists)
{
    for(size_t i = 0; i < pools.size(); ++i)
    {
        const auto& pool = pools[i];
        auto& ai_list = ai_lists[i];
        std::vector<Genetic_AI> new_ai_list;
        for(const auto& ai : ai_list)
        {
            if(std::find(pool.begin(), pool.end(), ai) != pool.end())
            {
                new_ai_list.push_back(ai);
            }
        }
        ai_list = new_ai_list;
    }
}
