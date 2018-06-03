#include "Breeding/Think_Tank.h"

#include <vector>
#include <iostream>
#include <map>
#include <iomanip>
#include <csignal>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <future>
#include <numeric>

#include "Players/Neural_AI.h"
#include "Game/Game.h"
#include "Game/Game_Result.h"
#include "Utility.h"

static sig_atomic_t signal_activated = 0;
void pause_think_tank(int);
void write_generation(const std::vector<Think_Tank>& tanks, size_t tank_index, const std::string& genome_file_name);

template<typename Stat>
void purge_dead_from_map(const std::vector<Think_Tank>& tanks, std::map<Neural_AI, Stat>& stats);

void purge_dead_from_map(const std::vector<Think_Tank>& tanks, std::map<size_t, std::vector<Neural_AI>>& ai_lists);


void think_tank(const std::string& config_file = "")
{
    auto config = Configuration_File(config_file);

    // Environment variables
    const auto maximum_simultaneous_games = size_t(config.get_number("maximum simultaneous games"));
    const auto think_tank_population = size_t(config.get_number("think tank population"));
    const auto think_tank_count = size_t(config.get_number("think tank count"));
    const auto draw_kill_probability = double(config.get_number("draw kill probability"));
    const auto tank_swap_interval = size_t(config.get_number("tank swap interval"));

    // Oscillating game time
    const double minimum_game_time = config.get_number("minimum game time"); // seconds
    const double maximum_game_time = config.get_number("maximum game time"); // seconds
    double game_time_increment = config.get_number("game time increment"); // seconds
    const bool oscillating_time = (String::lowercase(config.get_text("oscillating time")) == "yes");
    double game_time = minimum_game_time;

    // Stats (map: tank ID --> counts)
    std::map<size_t, size_t> game_count;
    std::map<size_t, size_t> white_wins;
    std::map<size_t, size_t> black_wins;
    std::map<size_t, size_t> draw_count;
    std::map<size_t, size_t> most_wins;
    std::map<size_t, Neural_AI> most_wins_player;

    std::map<size_t, size_t> most_games_survived;
    std::map<size_t, Neural_AI> most_games_survived_player;

    std::map<size_t, std::vector<Neural_AI>> new_blood; // ex nihilo players
    std::map<size_t, size_t> new_blood_count;

    std::map<Neural_AI, size_t> wins;
    std::map<Neural_AI, size_t> draws;
    std::map<Neural_AI, size_t> games_since_last_win;
    std::map<Neural_AI, size_t> consecutive_wins;
    std::map<Neural_AI, size_t> original_tank;

    const int scramble_mutations = 100000;

    std::string genome_file_name = config.get_text("think tank file");
    if(genome_file_name.empty())
    {
        genome_file_name = "think_tank_record_";
        for(int i = 0; i < 10; ++i)
        {
            genome_file_name += char('a' + Random::random_integer(0, 25));
        }
        genome_file_name += ".txt";
    }

    auto tanks = load_think_tank_file(genome_file_name);
    while(tanks.size() < think_tank_count)
    {
        tanks.push_back({});
    }

    for(size_t i = 0; i < tanks.size(); ++i)
    {
        while(tanks[i].size() < think_tank_population)
        {
            tanks[i].push_back(Neural_AI());
            tanks[i].back().mutate(scramble_mutations);
        }

        while(tanks[i].size() > think_tank_population)
        {
            tanks[i].pop_back();
        }

        for(const auto& ai : tanks[i])
        {
            original_tank[ai] = i;
        }

        write_generation(tanks, i, genome_file_name);
    }

    std::string game_record_file = genome_file_name +  "_games.txt";
    if(auto ifs = std::ifstream(game_record_file))
    {
        // Use game time from last run of this think tank
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

    // Ctrl-C to pause think tank
    signal(SIGINT, pause_think_tank);

    // Indices in think tank to be shuffled for game match-ups
    std::vector<size_t> tank_indices(think_tank_population);
    std::iota(tank_indices.begin(), tank_indices.end(), 0);

    for(size_t tank_index = 0; true; tank_index = (tank_index + 1) % tanks.size()) // run forever
    {
        auto& tank = tanks[tank_index];

        // Write overall stats
        std::cout << "\nThink tank ID: " << tank_index
                  << "  Think tank size: " << tank.size()
                  << "  New blood introduced: " << new_blood_count[tank_index] << " (*)\n"
                  << "Games: " << game_count[tank_index]
                  << "  White wins: " << white_wins[tank_index]
                  << "  Black wins: " << black_wins[tank_index]
                  << "  Draws: " << draw_count[tank_index]
                  << "\nTime: " << game_time << " sec"
                  << "   Think tank file name: " << genome_file_name << "\n\n";

        // The tank_indices list determines the match-ups. After shuffling the list
        // of indices (0 to think_tank_population - 1), adjacent indices in the tank are
        // matched as opponents.
        Random::shuffle(tank_indices);

        std::vector<std::future<Game_Result>> results; // map from tank_indices index to winner
        for(size_t index = 0; index < think_tank_population; index += 2)
        {
            auto white_index = tank_indices[index];
            auto black_index = tank_indices[index + 1];

            auto& white = tank[white_index];
            auto& black = tank[black_index];

            // Limit the number of simultaneous games by waiting for earlier games to finish
            // before starting a new one.
            auto in_progress_games = results.size();
            for(const auto& game : results)
            {
                if(in_progress_games < maximum_simultaneous_games)
                {
                    break;
                }
                game.wait();
                --in_progress_games;
            }

            results.emplace_back(std::async(play_game, white, black, game_time, 0, 0, game_record_file));
        }

        // Get results as they come in
        for(size_t index = 0; index < think_tank_population; index += 2)
        {
            auto& white = tank[tank_indices[index]];
            auto& black = tank[tank_indices[index + 1]];

            std::cout << white.get_id() << " vs "
                      << black.get_id() << ": " << std::flush;

            auto result = results[index/2].get();
            auto winner = result.get_winner();
            std::cout << color_text(winner) << " (" << result.get_ending_reason() << ")";

            if(winner == WHITE)
            {
                ++white_wins[tank_index];
            }
            else if(winner == BLACK)
            {
                ++black_wins[tank_index];
            }
            else // DRAW
            {
                draws[white]++;
                draws[black]++;
                games_since_last_win[white]++;
                games_since_last_win[black]++;
                consecutive_wins[white] = 0;
                consecutive_wins[black] = 0;
                ++draw_count[tank_index];
            }

            if(winner != NONE)
            {
                auto& winning_player = (winner == WHITE ? white : black);
                wins[winning_player]++;
                games_since_last_win[winning_player] = 0;
                consecutive_wins[winning_player]++;
                if(wins[winning_player] >= most_wins[tank_index])
                {
                    most_wins[tank_index] = wins[winning_player];
                    most_wins_player[tank_index] = winning_player;
                }

                auto offspring = winning_player;
                offspring.mutate();
                original_tank[offspring] = tank_index;

                auto& losing_player  = (winner == WHITE ? black : white);
                losing_player = offspring; // offspring replaces loser
            }
            else
            {
                if(Random::success_probability(draw_kill_probability))
                {
                    auto heads = Random::coin_flip();
                    auto& pseudo_winner = (heads ? white : black);
                    auto& pseudo_loser  = (heads ? black : white);
                    pseudo_loser = pseudo_winner;
                    pseudo_loser.mutate();
                    std::cout << "\n    --> " << pseudo_winner.get_id() << " survives" << " / "
                              << pseudo_loser.get_id() << " dies";
                    new_blood[tank_index].push_back(pseudo_loser);
                    ++new_blood_count[tank_index];
                    original_tank[pseudo_loser] = tank_index;
                }
            }
            std::cout << std::endl;
        }

        std::sort(tank.begin(), tank.end());
        write_generation(tanks, tank_index, genome_file_name);

        purge_dead_from_map(tanks, wins);
        purge_dead_from_map(tanks, draws);
        purge_dead_from_map(tanks, games_since_last_win);
        purge_dead_from_map(tanks, consecutive_wins);
        purge_dead_from_map(tanks, original_tank);
        purge_dead_from_map(tanks, new_blood);

        for(const auto& ai : tank)
        {
            auto games_survived = wins[ai] + draws[ai];
            if(games_survived >= most_games_survived[tank_index])
            {
                most_games_survived[tank_index] = games_survived;
                most_games_survived_player[tank_index] = ai;
            }
        }

        // widths of columns for stats printout
        auto max_id = tank.back().get_id();
        auto id_digits = int(std::floor(std::log10(max_id) + 1));

        // Write results
        std::cout << '\n' << std::setw(id_digits + 1)  << "ID"
                  << std::setw(7)  << "Wins"
                  << std::setw(8) << "Streak"
                  << std::setw(7)  << "Draws"
                  << std::setw(9)  << "Streak\n";

        // Write stats for each specimen
        for(const auto& ai : tank)
        {
            std::cout << std::setw(id_digits + 1) << ai.get_id();
            std::cout << std::setw(7)    << wins[ai]
                      << std::setw(8)   << consecutive_wins[ai]
                      << std::setw(7)    << draws[ai]
                      << std::setw(8)   << games_since_last_win[ai]
                      << (std::binary_search(new_blood[tank_index].begin(),
                                             new_blood[tank_index].end(),
                                             ai) ? " *" : "")
                      << (original_tank[ai] != tank_index ? " T" : "") << "\n";
        }
        std::cout << std::endl;

        std::cout << "Most wins:     " << most_wins[tank_index]
                  << " by ID " << most_wins_player[tank_index].get_id() << std::endl;
        std::cout <<   "Longest lived: " << most_games_survived[tank_index]
                  << " by ID " << most_games_survived_player[tank_index].get_id() << std::endl;

        // Record best AI from all tanks.
        auto best_ai = tank.front();
        auto best_compare = [&wins, &draws](const auto& x, const auto& y)
                            {
                                return wins[x] - draws[x] < wins[y] - draws[y];
                            };
        for(const auto& search_tank : tanks)
        {
            best_ai = std::max(best_ai,
                               *std::max_element(search_tank.begin(),
                                                 search_tank.end(),
                                                 best_compare), best_compare);
        }
        std::ofstream best_file(genome_file_name + "_best_genome.txt");
        best_ai.print(best_file);

        // Pause think tank
        if(signal_activated == 1)
        {
            std::cout << "Think tank paused. Press Enter to continue ..." << std::endl;
            std::cin.get();
        }

        if(signal_activated >= 2)
        {
            return;
        }
        else
        {
            signal_activated = 0;
        }

        game_count[tank_index] += results.size();
        if((game_time >= maximum_game_time && game_time_increment > 0) ||
           (game_time <= minimum_game_time && game_time_increment < 0))
        {
            game_time_increment *= -1;
        }

        if(game_time_increment > 0 || oscillating_time)
        {
            game_time += game_time_increment;
        }

        auto win_compare = [&wins, &draws](const auto& x, const auto& y)
                           {
                               if(wins[x] == wins[y])
                               {
                                   return draws[x] < draws[y];
                               }
                               return wins[x] < wins[y];
                           };

        // Transfer best players between think tanks to keep tanks
        // from stagnating or amplifying pathological behavior
        if(tanks.size() > 1 && tank_index == tanks.size() - 1) // all tanks have equal number of games
        {
            static size_t previous_mod = 0;
            auto this_mod = game_count[tank_index] % tank_swap_interval;
            if(this_mod < previous_mod)
            {
                // Replace player with least wins in each tank with clone of winner from tank to left
                std::cout << std::endl;
                std::vector<Neural_AI> winners;
                for(const auto& source_tank : tanks)
                {
                    winners.push_back(*std::max_element(source_tank.begin(),
                                                        source_tank.end(),
                                                        win_compare));
                }

                for(size_t source_tank_index = 0; source_tank_index < tanks.size(); ++source_tank_index)
                {
                    auto dest_tank_index = (source_tank_index + 1) % tanks.size();
                    auto& dest_tank = tanks[dest_tank_index];
                    auto& loser = *std::min_element(dest_tank.begin(), dest_tank.end(), win_compare);
                    std::cout << "Sending ID "
                              << winners[source_tank_index].get_id()
                              << " to tank "
                              << dest_tank_index << std::endl;
                    loser = winners[source_tank_index]; // winner replaces loser in destination tank
                }
            }

            previous_mod = this_mod;
        }
    }
}


void pause_think_tank(int)
{
    ++signal_activated;
    if(signal_activated == 1)
    {
        std::cout << std::endl << "Waiting for games to end and be recorded before pausing..." << std::endl;
    }
    else if(signal_activated == 2)
    {
        std::cout << std::endl << "Waiting for games to end and be recorded before exiting ..." << std::endl;
    }
    else
    {
        std::cout << std::endl << "Exiting ..." << std::endl;
        exit(1);
    }
}

void write_generation(const std::vector<Think_Tank>& tanks, size_t tank_index, const std::string& genome_file_name)
{
    static std::map<Neural_AI, bool> written_before;
    static std::string last_file_name;
    static std::ofstream ofs;
    if(last_file_name != genome_file_name)
    {
        ofs.close();
        ofs.open(genome_file_name, std::ios::app);
        last_file_name = genome_file_name;
    }

    auto tank = tanks.at(tank_index);
    for(const auto& ai : tank)
    {
        if( ! written_before[ai])
        {
            ai.print(ofs);
            written_before[ai] = true;
        }
    }

    ofs << "\nStill Alive: " << tank_index << " : ";
    for(const auto& ai : tank)
    {
        ofs << ai.get_id() << " ";
    }
    ofs << "\n\n";

    purge_dead_from_map(tanks, written_before);
}

std::vector<Think_Tank> load_think_tank_file(const std::string& load_file)
{
    std::ifstream ifs(load_file);
    if( ! ifs)
    {
        std::cout << "Could not open file: " << load_file << std::endl;
        std::cout << "Starting with empty think tank." << std::endl;
        return std::vector<Think_Tank>();
    }

    std::cout << "Loading think tank file: " << load_file << " ..." << std::endl;
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

    std::vector<Think_Tank> result(still_alive.size());
    for(const auto& index_list : still_alive)
    {
        for(const auto& number_string : String::split(index_list.second))
        {
            if(number_string.empty())
            {
                continue;
            }

            auto index = std::stoi(number_string);
            result[index_list.first].push_back(Neural_AI(load_file, index));
        }
        write_generation(result, index_list.first, ""); // mark AIs from file as already written
    }
    std::cout << "Done." << std::endl;

    return result;
}

template<typename Stat>
void purge_dead_from_map(const std::vector<Think_Tank>& tanks, std::map<Neural_AI, Stat>& stats)
{
    auto stat_iter = stats.begin();
    while(stat_iter != stats.end())
    {
        bool ai_found = false;
        for(const auto& tank : tanks)
        {
            if(std::find(tank.begin(), tank.end(), stat_iter->first) != tank.end())
            {
                ai_found = true;
                break;
            }
        }

        if( ! ai_found)
        {
            stat_iter = stats.erase(stat_iter);
        }
        else
        {
            ++stat_iter;
        }
    }
}

void purge_dead_from_map(const std::vector<Think_Tank>& tanks, std::map<size_t, std::vector<Neural_AI>>& ai_lists)
{
    for(size_t i = 0; i < tanks.size(); ++i)
    {
        const auto& tank = tanks[i];
        auto& ai_list = ai_lists[i];

        ai_list.erase(std::remove_if(ai_list.begin(),
                                     ai_list.end(),
                                     [&tank](const Neural_AI& g)
                                     {
                                         return std::find(tank.begin(), tank.end(), g) == tank.end();
                                     }),
                      ai_list.end());
    }
}
