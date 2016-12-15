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

#include "Players/Genetic_AI.h"
#include "Players/Human_Player.h"

#include "Game/Game.h"
#include "Game/Board.h"

#include "Exceptions/End_Of_File_Exception.h"

#include "Utility.h"

typedef std::vector<Genetic_AI> Gene_Pool;

static sig_atomic_t signal_activated = 0;
void pause_gene_pool(int);
void write_generation(const std::vector<Gene_Pool>& pools, size_t pool_index, const std::string& genome_file_name);
std::vector<Gene_Pool> load_gene_pool_file(const std::string& load_file);


void gene_pool(const std::string& config_file = "")
{
    // Ctrl-C to pause gene pool
    signal(SIGINT, pause_gene_pool);

    auto config = Configuration_File(config_file);

    // Environment variables
    const size_t maximum_simultaneous_games = config.get_number("maximum simultaneous games");
    const size_t gene_pool_population = config.get_number("gene pool population");
    const size_t gene_pool_count = config.get_number("gene pool count");
    const double draw_kill_probability = config.get_number("draw kill probability");
    const int pool_swap_interval = config.get_number("pool swap interval");

    // Oscillating game time
    const double minimum_game_time = config.get_number("minimum game time"); // seconds
    const double maximum_game_time = config.get_number("maximum game time"); // seconds
    double game_time_increment = config.get_number("game time increment"); // seconds
    double game_time = minimum_game_time;

    // Stats (map: Pool ID --> counts)
    std::map<size_t, int> game_count;
    std::map<size_t, int> white_wins;
    std::map<size_t, int> black_wins;
    std::map<size_t, int> draw_count;
    std::map<size_t, int> most_wins;
    std::map<size_t, Genetic_AI> most_wins_player;

    std::map<size_t, int> most_games_survived;
    std::map<size_t, Genetic_AI> most_games_survived_player;

    std::map<size_t, std::vector<Genetic_AI>> new_blood; // ex nihilo players

    std::map<Genetic_AI, int> wins;
    std::map<Genetic_AI, int> draws;
    std::map<Genetic_AI, int> games_since_last_win;
    std::map<Genetic_AI, int> consecutive_wins;
    std::map<Genetic_AI, size_t> original_pool;

    std::string genome_file_name = config.get_text("gene pool file");
    if(genome_file_name.empty())
    {
        genome_file_name = "gene_pool_record_";
        for(int i = 0; i < 10; ++i)
        {
            genome_file_name += char('a' + Random::random_integer(0, 25));
        }
        genome_file_name += ".txt";
    }

    std::vector<Gene_Pool> pools = load_gene_pool_file(genome_file_name);
    for(size_t i = 0; i < pools.size(); ++i)
    {
        write_generation(pools, i, ""); // mark AIs from file as already written
        for(const auto& ai : pools[i])
        {
            original_pool[ai] = i;
        }
    }
    while(pools.size() < gene_pool_count)
    {
        // create gene pool
        pools.push_back({}); // empty gene pool

        while(pools.back().size() < gene_pool_population)
        {
            pools.back().emplace_back();

            // Scramble up genome for random initial population
            for(int i = 0; i < 100; ++i)
            {
                pools.back().back().mutate();
            }
        }

        for(const auto& ai : pools.back())
        {
            original_pool[ai] = pools.size() - 1;
        }
    }

    // Indices in gene pool to be shuffled for game match-ups
    std::vector<size_t> pool_indices;
    for(size_t i = 0; i < gene_pool_population; ++i)
    {
        pool_indices.push_back(i);
    }

    std::string game_record_file = genome_file_name +  "_games.txt";

    for(size_t pool_index = 0; true; pool_index = (pool_index + 1) % gene_pool_count) // run forever
    {
        auto& pool = pools[pool_index];
        std::sort(pool.begin(), pool.end());
        write_generation(pools, pool_index, genome_file_name);

        // Pause gene pool
        if(signal_activated == 1)
        {
            std::cout << "Gene pool paused. Press Enter to continue ..." << std::endl;
            std::cin.get();
            signal_activated = 0;
        }

        // widths of columns for stats printout
        auto max_id = pool.back().get_id();
        int id_digits = std::floor(std::log10(max_id) + 1);

        // Write overall stats
        std::cout << "\nGene pool ID: " << pool_index
                  << "  Gene pool size: " << pool.size()
                  << "  New blood introduced: " << new_blood[pool_index].size() << " (*)\n"
                  << "Games: " << game_count[pool_index]
                  << "  White wins: " << white_wins[pool_index]
                  << "  Black wins: " << black_wins[pool_index]
                  << "  Draws: " << draw_count[pool_index]
                  << "\nTime: " << int(game_time) << " sec"
                  << "   Gene pool file name: " << genome_file_name << "\n"
                  << std::setw(id_digits + 1)  << "ID"
                  << std::setw(7)  << "Wins"
                  << std::setw(8) << "Streak"
                  << std::setw(7)  << "Draws"
                  << std::setw(9)  << "Streak\n";

        // Write stats for each specimen
        for(const auto& ai : pool)
        {
            std::cout << std::setw(id_digits + 1) << ai.get_id();
            std::cout << std::setw(7)    << wins[ai]
                      << std::setw(8)   << consecutive_wins[ai]
                      << std::setw(7)    << draws[ai]
                      << std::setw(8)   << games_since_last_win[ai]
                      << (std::binary_search(new_blood[pool_index].begin(),
                                             new_blood[pool_index].end(),
                                             ai) ? " *" : "")
                      << (original_pool[ai] != pool_index ? " T" : "") << "\n";
        }
        std::cout << std::endl;

        // The pool_indices list determines the match-ups. After shuffling the list
        // of indices (0 to gene_pool_population - 1), adjacent indices in the pool are
        // matched as opponents.
        pool_indices = Random::shuffle(pool_indices);

        std::vector<std::future<Color>> results; // map from pool_indices index to winner
        for(size_t index = 0; index < gene_pool_population; index += 2)
        {
            auto white_index = pool_indices[index];
            auto black_index = pool_indices[index + 1];

            auto& white = pool[white_index];
            auto& black = pool[black_index];

            results.emplace_back(std::async(play_game, white, black, game_time, 0, game_record_file));

            // Limit the number of simultaneous games by waiting for earlier games to finish
            // before starting a new one.
            if(results.size() >= maximum_simultaneous_games)
            {
                auto in_progress_games = results.size();
                for(const auto& game : results)
                {
                    game.wait();
                    --in_progress_games;
                    if(in_progress_games < maximum_simultaneous_games)
                    {
                        break;
                    }
                }
            }
        }

        // Get results as they come in
        for(size_t index = 0; index < gene_pool_population; index += 2)
        {
            auto& white = pool[pool_indices[index]];
            auto& black = pool[pool_indices[index + 1]];

            std::cout << "Result of " << white.get_id() << " vs. "
                                      << black.get_id() << ": " << std::flush;

            auto winner = results[index/2].get();
            std::cout << color_text(winner) << "! ";

            if(winner == WHITE)
            {
                ++white_wins[pool_index];
            }
            else if(winner == BLACK)
            {
                ++black_wins[pool_index];
            }
            else // DRAW
            {
                draws[white]++;
                draws[black]++;
                games_since_last_win[white]++;
                games_since_last_win[black]++;
                consecutive_wins[white] = 0;
                consecutive_wins[black] = 0;
                ++draw_count[pool_index];
            }

            if(winner != NONE)
            {
                auto& winning_player = (winner == WHITE ? white : black);
                wins[winning_player]++;
                games_since_last_win[winning_player] = 0;
                consecutive_wins[winning_player]++;
                if(wins[winning_player] > most_wins[pool_index])
                {
                    most_wins[pool_index] = wins[winning_player];
                    most_wins_player[pool_index] = winning_player;
                }

                std::cout << "mating " << white.get_id() << " " << black.get_id();
                auto offspring = Genetic_AI(white, black);
                offspring.mutate();
                original_pool[offspring] = pool_index;

                auto& losing_player  = (winner == WHITE ? black : white);
                std::cout << " / killing " << losing_player.get_id() << std::endl;
                losing_player = offspring; // offspring replaces loser
            }
            else
            {
                if(Random::success_probability(draw_kill_probability))
                {
                    auto& pseudo_winner = (Random::coin_flip() ? white : black);
                    std::cout << pseudo_winner.get_id() << " mates with random";
                    auto new_specimen = Genetic_AI();
                    for(int i = 0; i < 100; ++i)
                    {
                        new_specimen.mutate();
                    }
                    auto offspring = Genetic_AI(pseudo_winner, new_specimen);
                    offspring.mutate();
                    new_blood[pool_index].push_back(offspring);
                    original_pool[offspring] = pool_index;

                    auto& pseudo_loser = (pseudo_winner == white ? black : white);
                    std::cout << " / " << pseudo_loser.get_id() << " dies";
                    pseudo_loser = offspring; // offspring replaces loser
                }
                std::cout << std::endl;
            }
        }

        for(const auto& ai : pool)
        {
            auto games_survived = wins[ai] + draws[ai];
            if(games_survived > most_games_survived[pool_index])
            {
                most_games_survived[pool_index] = games_survived;
                most_games_survived_player[pool_index] = ai;
            }
        }

        std::cout << "\nMost wins:     " << most_wins[pool_index]
                  << " by ID " << most_wins_player[pool_index].get_id() << std::endl;
        std::cout <<   "Longest lived: " << most_games_survived[pool_index]
                  << " by ID " << most_games_survived_player[pool_index].get_id() << std::endl;

        game_count[pool_index] += results.size();
        if((game_time >= maximum_game_time && game_time_increment > 0) ||
           (game_time <= minimum_game_time && game_time_increment < 0))
        {
            game_time_increment *= -1;
        }
        game_time += game_time_increment;

        auto win_compare = [&wins, &draws](const auto& x, const auto& y)
                           {
                               if(wins[x] == wins[y])
                               {
                                   return draws[x] < draws[y];
                               }
                               return wins[x] < wins[y];
                           };

        // Transfer best players between gene pools to keep pools
        // from stagnating or amplifying pathological behavior
        if(pools.size() > 1 && pool_index == pools.size() - 1) // all pools have equal number of games
        {
            static auto previous_mod = 0;
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
                                                        win_compare));
                }

                for(size_t source_pool_index = 0; source_pool_index < pools.size(); ++source_pool_index)
                {
                    auto dest_pool_index = (source_pool_index + 1) % pools.size();
                    auto& dest_pool = pools[dest_pool_index];
                    auto& loser = *std::min_element(dest_pool.begin(), dest_pool.end(), win_compare);
                    std::cout << "Sending ID "
                              << winners[source_pool_index].get_id()
                              << " to pool "
                              << dest_pool_index << std::endl;
                    loser = winners[source_pool_index]; // winner replaces loser in destination pool
                }
            }

            previous_mod = this_mod;
        }
    }
}


void pause_gene_pool(int)
{
    if(signal_activated == 1)
    {
        std::cout << std::endl << "Exiting ..." << std::endl;
        exit(1);
    }
    signal_activated = 1;
    std::cout << std::endl << "Waiting for games to end and be recorded ..." << std::endl;
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
            ai.print_genome(ofs);
            written_before[ai] = true;
        }
    }

    ofs << "\nStill Alive: " << pool_index << " : ";
    for(const auto& ai : pool)
    {
        ofs << ai.get_id() << " ";
    }
    ofs << "\n\n";
}

std::vector<Gene_Pool> load_gene_pool_file(const std::string& load_file)
{
    std::ifstream ifs(load_file);
    if( ! ifs)
    {
        return std::vector<Gene_Pool>();
    }

    std::cout << "Loading gene pool file: " << load_file << " ..." << std::endl;
    std::map<int, Genetic_AI> all_players;
    std::map<int, std::string> still_alive;
    while(true)
    {
        try
        {
            auto player = Genetic_AI(ifs);
            all_players[player.get_id()] = player;
        }
        catch(const std::runtime_error& ge)
        {
            std::string line = ge.what();
            if(String::contains(line, "Still Alive: "))
            {
                auto parse = String::split(line, ":");
                still_alive[std::stoi(parse[2])] = parse[3];
            }
            else
            {
                throw;
            }
        }
        catch(const End_Of_File_Exception&)
        {
            break;
        }
    }

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
            result[index_list.first].push_back(all_players[index]);
        }
    }

    return result;
}
