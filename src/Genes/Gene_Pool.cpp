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
#include <random>

#include "Players/Genetic_AI.h"
#include "Players/Human_Player.h"

#include "Game/Game.h"
#include "Game/Board.h"

#include "Exceptions/Generic_Exception.h"
#include "Exceptions/End_Of_File_Exception.h"

#include "Utility.h"

// Declarations to silence warnings
void signal_handler(int);
void write_generation(const std::vector<Genetic_AI>& pool,
                      const std::string& genome_file_name);
std::vector<Genetic_AI> load_gene_pool_file(const std::string& load_file);


sig_atomic_t signal_activated = 0;
void signal_handler(int)
{
    if(signal_activated == 1)
    {
        std::cout << std::endl << "Exiting ..." << std::endl;
        exit(1);
    }
    signal_activated = 1;
    std::cout << std::endl << "Waiting for games to end ..." << std::endl;
}

void write_generation(const std::vector<Genetic_AI>& pool,
                      const std::string& genome_file_name)
{
    static std::map<int, bool> written_before;
    static std::string last_file_name;
    static std::ofstream ofs;
    if(last_file_name != genome_file_name)
    {
        ofs.close();
        ofs.open(genome_file_name, std::ios::app);
        last_file_name = genome_file_name;
    }

    for(const auto& ai : pool)
    {
        int id = ai.get_id();
        if( ! written_before[id])
        {
            ai.print_genome(ofs);
            written_before[id] = true;
        }
    }

    ofs << "\nStill Alive: ";
    for(const auto& ai : pool)
    {
        ofs << ai.get_id() << " ";
    }
    ofs << "\n\n";
}

std::vector<Genetic_AI> load_gene_pool_file(const std::string& load_file)
{
    std::vector<Genetic_AI> all_players;

    std::ifstream ifs(load_file);
    if( ! ifs)
    {
        return all_players;
    }

    std::string still_alive;
    while(true)
    {
        try
        {
            all_players.emplace_back(ifs);
        }
        catch(const Generic_Exception& ge)
        {
            std::string line = ge.what();
            if(String::contains(line, "Still Alive: "))
            {
                still_alive = String::split(line, "Still Alive: ")[1];
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

    std::vector<Genetic_AI> result;
    for(const auto& number_string : String::split(still_alive, " "))
    {
        if(number_string.empty())
        {
            continue;
        }

        auto index = std::stoi(number_string);
        for(const auto& player : all_players)
        {
            if(player.get_id() == index)
            {
                result.push_back(player);
                break;
            }
        }
    }

    return result;
}


void gene_pool(const std::string& load_file = "")
{
    // Infrastructure and recording
    signal(SIGINT, signal_handler);
    static auto urng = std::mt19937_64(std::chrono::system_clock::now().time_since_epoch().count());
    const size_t maximum_simultaneous_games = 8;

    // Environment variables
    const size_t population_size = 16;
    const double draw_kill_probability = 0.05;

    // Oscillating game time
    const double minimum_game_time = 10; // seconds
    const double maximum_game_time = 120; // seconds
    double game_time_increment = 0.1; // seconds
    double game_time = minimum_game_time;

    int game_count = 0;
    int white_wins = 0;
    int black_wins = 0;
    int draw_count = 0;

    int most_wins = 0;
    int most_wins_id = -1;
    int most_games_survived = 0;
    int most_games_survived_id = -1;

    std::map<int, int> wins;
    std::map<int, int> draws;
    std::vector<int> new_blood;

    std::string genome_file_name;
    if(load_file.empty())
    {
        genome_file_name = "gene_pool_record_";
        for(int i = 0; i < 10; ++i)
        {
            genome_file_name += char('a' + Random::random_integer(0, 25));
        }
        genome_file_name += ".txt";
    }
    else
    {
        genome_file_name = load_file;
    }

    // create gene pool
    std::vector<Genetic_AI> pool = load_gene_pool_file(genome_file_name);
    write_generation(pool, ""); // mark AIs from file as already written
    while(pool.size() < population_size)
    {
        pool.emplace_back();

        // Scramble up genome for random initial population
        for(int i = 0; i < 100; ++i)
        {
            pool.back().mutate();
        }
    }

    // Indices in gene pool to be shuffled for game matchups
    std::vector<size_t> pool_indices;
    for(size_t i = 0; i < population_size; ++i)
    {
        pool_indices.push_back(i);
    }

    std::string game_record_file = genome_file_name +  "_games.txt";

    while(true)
    {
        write_generation(pool, genome_file_name);

        // widths of columns for stats printout
        auto max_id = pool.front().get_id();
        for(const auto& ai : pool)
        {
            max_id = std::max(max_id, ai.get_id());
        }
        int id_digits = std::floor(std::log10(max_id) + 1);
        int parent_width = std::max(2*(id_digits + 1) + 1, 9);

        std::cout << "\nGene pool size: " << pool.size()
                  << "  New blood introduced: " << new_blood.size() << " (*)\n"
                  << "Games: " << game_count
                  << "  White wins: " << white_wins
                  << "  Black wins: " << black_wins
                  << "  Draws: " << draw_count
                  << "\nTime: " << int(game_time) << " sec"
                  << "   Gene pool file name: " << genome_file_name << "\n"
                  << std::setw(id_digits + 1)  << "ID"
                  << std::setw(parent_width) << "Parents"
                  << std::setw(5)  << "W"
                  << std::setw(6)  << "D\n";

        for(const auto& ai : pool)
        {
            std::cout << std::setw(id_digits + 1) << ai.get_id();
            std::cout.unsetf(std::ios_base::floatfield);
            std::cout << std::setprecision(12);
            std::ostringstream oss;
            for(int id : ai.get_parents())
            {
                oss << " " << id;
            }
            std::cout << std::setw(parent_width) << oss.str()
                      << std::setw(5)    << wins[ai.get_id()]
                      << std::setw(5)    << draws[ai.get_id()]
                      << (std::find(new_blood.begin(),
                                    new_blood.end(),
                                    ai.get_id()) != new_blood.end() ? " *" : "") << "\n";
        }
        std::cout << std::endl;

        // The pool_indices list determines the matchups. After shuffling the list
        // of indices (0 to population_size - 1), adjacent indices in the pool are
        // matched as opponents.
        std::shuffle(pool_indices.begin(), pool_indices.end(), urng);
        std::vector<std::future<Color>> results; // map from pool_indices index to winner
        for(size_t index = 0; index < population_size; index += 2)
        {
            auto white_index = pool_indices[index];
            auto black_index = pool_indices[index + 1];

            auto& white = pool[white_index];
            auto& black = pool[black_index];

            results.emplace_back(std::async(play_game, white, black, game_time, 0, game_record_file));

            // Limit the number of simultaneous games by waiting for earlier games to finsih
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
        for(size_t index = 0; index < population_size; index += 2)
        {
            auto white_index = pool_indices[index];
            auto& white = pool[white_index];
            auto black_index = pool_indices[index + 1];
            auto black = pool[black_index];

            std::cout << "Result of " << white.get_id() << " vs. "
                                      << black.get_id() << ": " << std::flush;

            auto winner = results[index/2].get();
            std::cout << color_text(winner) << "! ";

            if(winner == WHITE)
            {
                ++white_wins;
            }
            else if(winner == BLACK)
            {
                ++black_wins;
            }
            else // DRAW
            {
                draws[white.get_id()]++;
                draws[black.get_id()]++;
                ++draw_count;
            }

            if(winner != NONE)
            {
                auto& winning_player = (winner == WHITE ? white : black);
                wins[winning_player.get_id()]++;
                if(wins[winning_player.get_id()] > most_wins)
                {
                    most_wins = wins[winning_player.get_id()];
                    most_wins_id = winning_player.get_id();
                }

                auto loser_index = (winner == WHITE ? black_index : white_index);
                auto loser_id = pool[loser_index].get_id();

                std::cout << "mating " << white.get_id() << " " << black.get_id();
                pool[loser_index] = Genetic_AI(white, black);
                pool[loser_index].mutate();
                std::cout << " / killing " << loser_id << std::endl;

                wins.erase(loser_id);
                draws.erase(loser_id);
            }
            else
            {
                if(Random::success_probability(draw_kill_probability))
                {
                    auto pseudo_winner_index = (Random::coin_flip() ? white_index : black_index);
                    auto pseudo_loser_index = (pseudo_winner_index == white_index) ? black_index : white_index;
                    auto new_specimen = Genetic_AI();
                    for(int i = 0; i < 100; ++i)
                    {
                        new_specimen.mutate();
                    }
                    std::cout << pool[pseudo_loser_index].get_id() << " dies / ";
                    std::cout << pool[pseudo_winner_index].get_id() << " mates with random";
                    pool[pseudo_loser_index] = Genetic_AI(pool[pseudo_winner_index], new_specimen);
                    new_blood.push_back(pool[pseudo_loser_index].get_id());
                }
                std::cout << std::endl;
            }
        }

        for(const auto& ai : pool)
        {
            auto games_survived = wins[ai.get_id()] + draws[ai.get_id()];
            if(games_survived > most_games_survived)
            {
                most_games_survived = games_survived;
                most_games_survived_id = ai.get_id();
            }
        }

        std::cout << "\nMost wins:     " << most_wins           << " by ID " << most_wins_id << std::endl;
        std::cout <<   "Longest lived: " << most_games_survived << " by ID " << most_games_survived_id << std::endl;

        game_count += results.size();
        if((game_time >= maximum_game_time && game_time_increment > 0) ||
           (game_time <= minimum_game_time && game_time_increment < 0))
        {
            game_time_increment *= -1;
        }
        game_time += game_time_increment;

        // Interrupt gene pool to play against top AI
        if(signal_activated == 1)
        {
            std::cout << "Do you want to play against the top AI? [yN] ";
            std::string yn;
            std::getline(std::cin, yn);
            if(yn[0] == 'y' || yn[0] == 'Y')
            {
                auto& top_player = pool.front();
                for(const auto& player : pool)
                {
                    if(wins[player.get_id()] > wins[top_player.get_id()])
                    {
                        top_player = player;
                    }
                }
                auto human_color = NONE;
                auto winning_color = NONE;
                if(Random::coin_flip())
                {
                    human_color = WHITE;
                    winning_color = play_game(Human_Player(), top_player, 60*20, 40, "human_challenge.pgn");
                }
                else
                {
                    human_color = BLACK;
                    winning_color = play_game(top_player, Human_Player(), 60*20, 40, "human_challenge.pgn");
                }
                // Stalemate?
                std::cout << "You " << (human_color == winning_color ? "won" : "lost") << "!" << std::endl;
                std::cin.get();
            }

            signal_activated = 0;
        }
    }
}
