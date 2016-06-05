#include "Genes/Gene_Pool.h"

#include <vector>
#include <iostream>
#include <map>
#include <iomanip>
#include <csignal>
#include <fstream>
#include <cmath>
#include <algorithm>

#include "Players/Genetic_AI.h"
#include "Players/Human_Player.h"

#include "Game/Game.h"
#include "Game/Board.h"

#include "Exceptions/Generic_Exception.h"
#include "Exceptions/End_Of_File_Exception.h"

#include "Utility.h"

sig_atomic_t signal_activated = 0;
void signal_handler(int)
{
    if(signal_activated == 1)
    {
        std::cout << "Exiting ..." << std::endl;
        exit(1);
    }
    signal_activated = 1;
    std::cout << "   Waiting for games to end ..." << std::endl;
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
    auto default_signal_handler = signal(SIGINT, signal_handler);
    std::vector<Genetic_AI> pool;

    // Environment variables
    const size_t population = 20;

    // Oscillating game time
    const double minimum_game_time = 10; // seconds
    const double maximum_game_time = 10; // seconds
    double game_time_increment = 0.5; // seconds
    double game_time = minimum_game_time;

    int game_count = 0;
    int white_wins = 0;
    int black_wins = 0;
    int draw_count = 0;

    int winning_streak = 0;
    int winning_streak_id = -1;

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

    pool = load_gene_pool_file(genome_file_name);
    write_generation(pool, "");
    while(pool.size() < population)
    {
        pool.emplace_back();

        // Scramble up genome for random initial population
        for(int i = 0; i < 100; ++i)
        {
            pool.back().mutate();
        }

        write_generation(pool, genome_file_name);
    }

    std::string game_record_file = genome_file_name +  "_games.txt";

    while(true)
    {
        int id_digits = std::floor(std::log10(pool.back().get_id()) + 1);
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
        std::cout << "Longest winning streak: " << winning_streak << " by ID " << winning_streak_id << std::endl;

        int white_index = Random::random_integer(0, pool.size() - 1);
        int black_index = Random::random_integer(0, pool.size() - 1);
        while(white_index == black_index)
        {
            black_index = Random::random_integer(0, pool.size() - 1);
        }

        auto& white = pool[white_index];
        auto& black = pool[black_index];

        std::cout << "play game " << white.get_id() << " " << black.get_id() << std::endl;
        auto winner = play_game(white, black, game_time, 0, game_record_file);
        std::cout << "\n";

        ++game_count;
        game_time += game_time_increment;
        if(game_time <= minimum_game_time || game_time >= maximum_game_time)
        {
            game_time_increment *= -1;
            if(game_time <= minimum_game_time)
            {
                game_time = minimum_game_time;
            }
            else
            {
                game_time = maximum_game_time;
            }
        }

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
            std::cout << "mate " << white.get_id() << " " << black.get_id() << std::endl;
            pool.emplace_back(white, black);
            pool.back().mutate();

            auto& winning_player = (winner == WHITE ? white : black);
            std::cout << winning_player.get_id() << " wins" << std::endl;
            wins[winning_player.get_id()]++;
            if(wins[winning_player.get_id()] > winning_streak)
            {
                winning_streak = wins[winning_player.get_id()];
                winning_streak_id = winning_player.get_id();
            }

            auto losing_index = (winner == WHITE ? black_index : white_index);
            auto losing_id = pool[losing_index].get_id();
            std::cout << losing_id << " dies" << std::endl;
            pool.erase(pool.begin() + losing_index);
            wins.erase(losing_id);
            draws.erase(losing_id);

            write_generation(pool, genome_file_name);
        }
        else
        {
            std::cout << white.get_id() << " " << black.get_id() << " draw!" << std::endl;
            if(Random::success_probability(0.05))
            {
                auto pseudo_winner_index = Random::coin_flip() ? white_index : black_index;
                auto pseudo_loser_index = (pseudo_winner_index == white_index) ? black_index : white_index;
                auto new_specimen = Genetic_AI();
                for(int i = 0; i < 100; ++i)
                {
                    new_specimen.mutate();
                }
                std::cout << pool[pseudo_loser_index].get_id() << " dies" << std::endl;
                std::cout << pool[pseudo_winner_index].get_id() << " RANDOM mates" << std::endl;
                pool.emplace_back(pool[pseudo_winner_index], new_specimen);
                pool.erase(pool.begin() + pseudo_loser_index);
                new_blood.push_back(pool.back().get_id());
            }
        }

        // Interrupt gene pool to play against top AI
        if(signal_activated == 1)
        {
            std::cout << "Do you want to play against the top AI? [yN] ";
            auto yn = std::cin.get();
            if(yn == 'y' || yn == 'Y')
            {
                signal_activated = 0;
                signal(SIGINT, default_signal_handler);

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
                signal(SIGINT, signal_handler);
            }
        }
    }
}
