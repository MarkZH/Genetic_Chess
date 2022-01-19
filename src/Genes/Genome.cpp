#include "Genes/Genome.h"

#include <string>
#include <array>
#include <iostream>
#include <algorithm>
#include <numeric>

#include "Game/Color.h"
#include "Game/Clock.h"
#include "Game/Board.h"

#include "Utility/Random.h"
#include "Utility/String.h"
#include "Utility/Exceptions.h"

#include "Genes/Gene.h"
#include "Genes/Total_Force_Gene.h"
#include "Genes/Freedom_To_Move_Gene.h"
#include "Genes/Pawn_Advancement_Gene.h"
#include "Genes/Passed_Pawn_Gene.h"
#include "Genes/Opponent_Pieces_Targeted_Gene.h"
#include "Genes/Sphere_of_Influence_Gene.h"
#include "Genes/Look_Ahead_Gene.h"
#include "Genes/King_Confinement_Gene.h"
#include "Genes/King_Protection_Gene.h"
#include "Genes/Castling_Possible_Gene.h"
#include "Genes/Piece_Strength_Gene.h"
#include "Genes/Stacked_Pawns_Gene.h"
#include "Genes/Pawn_Islands_Gene.h"
#include "Genes/Checkmate_Material_Gene.h"
#include "Genes/Pawn_Structure_Gene.h"

namespace
{
    const auto search_method_names =
        []()
        {
            std::array<std::string, 2> names;
            names[static_cast<int>(Search_Method::MINIMAX)] = "Minimax";
            names[static_cast<int>(Search_Method::ITERATIVE_DEEPENING)] = "Iterative Deepening";
            return names;
        }();

        int next_id = 0;
}

Genome::Genome() noexcept :
    id_number(next_id++),
    genome{
        std::make_unique<Piece_Strength_Gene>(),
        std::make_unique<Look_Ahead_Gene>(),
        std::make_unique<Total_Force_Gene>(nullptr),
        std::make_unique<Freedom_To_Move_Gene>(),
        std::make_unique<Pawn_Advancement_Gene>(),
        std::make_unique<Passed_Pawn_Gene>(),
        std::make_unique<Opponent_Pieces_Targeted_Gene>(nullptr),
        std::make_unique<Sphere_of_Influence_Gene>(),
        std::make_unique<King_Confinement_Gene>(),
        std::make_unique<King_Protection_Gene>(),
        std::make_unique<Castling_Possible_Gene>(),
        std::make_unique<Stacked_Pawns_Gene>(),
        std::make_unique<Pawn_Islands_Gene>(),
        std::make_unique<Checkmate_Material_Gene>(),
        std::make_unique<Pawn_Structure_Gene>()
    }
{
    renormalize_priorities();
    reset_piece_strength_gene();

    assert(gene_reference<Piece_Strength_Gene>().name() == "Piece Strength Gene");
    assert(gene_reference<Look_Ahead_Gene>().name() == "Look Ahead Gene");
}

Genome::Genome(const Genome& other) noexcept : id_number(other.id()), searching_method(other.search_method())
{
    std::transform(other.genome.begin(), other.genome.end(),
                   genome.begin(),
                   [](const auto& gene)
                   {
                       return gene->duplicate();
                   });
    reset_piece_strength_gene();
}

Genome::Genome(std::istream& is, int id_in) : Genome()
{
    id_number = id_in;
    --next_id;

    if( ! is)
    {
        throw Genome_Creation_Error("Could not read: ");
    }

    for(std::string line; std::getline(is, line);)
    {
        line = String::strip_comments(line, "#");
        if(!String::starts_with(line, "ID"))
        {
            continue;
        }

        const auto param_value = String::split(line, ":", 1);
        if(param_value.size() != 2 || String::trim_outer_whitespace(param_value[0]) != "ID")
        {
            continue;
        }

        if(id_number == String::to_number<int>(param_value[1]))
        {
            auto add_details = [this](const auto& e)
            {
                return "Error in creating Genome #" + std::to_string(id()) + "\n" + e.what() + "\nFile: ";
            };

            try
            {
                read_from(is);
            }
            catch(const Missing_Genome_Data& e)
            {
                throw Missing_Genome_Data(add_details(e));
            }
            catch(const Duplicate_Genome_Data& e)
            {
                throw Duplicate_Genome_Data(add_details(e));
            }
            catch(const Genome_Creation_Error& e)
            {
                throw Genome_Creation_Error(add_details(e));
            }

            next_id = std::max(next_id, id() + 1);
            return;
        }
    }

    throw Genome_Creation_Error("Could not locate ID " + std::to_string(id_number) + " inside file ");
}

void Genome::reset_piece_strength_gene() noexcept
{
    auto piece_strength_gene = &gene_reference<Piece_Strength_Gene>();
    for(auto& gene : genome)
    {
        gene->reset_piece_strength_gene(piece_strength_gene);
    }
}

void Genome::renormalize_priorities() noexcept
{
    for(auto stage : {Game_Stage::OPENING, Game_Stage::ENDGAME})
    {
        const auto norm =
            std::accumulate(genome.begin(), genome.end(), 0.0,
                            [stage](auto sum, const auto& gene)
                            {
                                return sum + (gene->has_priority() ? std::abs(gene->priority(stage)) : 0.0);
                            });
        if(norm > 0.0)
        {
            for(auto& gene : genome)
            {
                gene->scale_priority(stage, 1.0/norm);
            }
        }
    }
}

Genome& Genome::operator=(const Genome& other) noexcept
{
    id_number = other.id();
    std::transform(other.genome.begin(), other.genome.end(),
                   genome.begin(),
                   [](const auto& gene)
                   {
                       return gene->duplicate();
                   });
    reset_piece_strength_gene();
    searching_method = other.search_method();
    return *this;
}

Genome::Genome(const Genome& A, const Genome& B) noexcept : id_number(next_id++)
{
    std::transform(A.genome.begin(), A.genome.end(), B.genome.begin(),
                   genome.begin(),
                   [](const auto& gene_a, const auto& gene_b)
                   {
                       return (Random::coin_flip() ? gene_a : gene_b)->duplicate();
                   });

    reset_piece_strength_gene();
    renormalize_priorities();

    searching_method = Random::coin_flip() ? A.search_method() : B.search_method();
}

void Genome::read_from(std::istream& is)
{
    for(std::string line; std::getline(is, line);)
    {
        line = String::strip_comments(line, "#");

        if(line.empty())
        {
            continue;
        }

        if(line == "END")
        {
            renormalize_priorities();
            return;
        }

        const auto line_split = String::split(line, ":", 1);
        if(line_split.size() != 2)
        {
            throw Genome_Creation_Error("No colon in parameter line: " + line);
        }

        if(String::trim_outer_whitespace(line_split[0]) == "Name")
        {
            const auto gene_name = String::remove_extra_whitespace(line_split[1]);
            const auto found_gene = std::find_if(genome.begin(), genome.end(),
                                                 [&gene_name](const auto& gene)
                                                 {
                                                     return gene->name() == gene_name;
                                                 });
            if(found_gene != genome.end())
            {
                (*found_gene)->read_from(is);
            }
            else
            {
                throw Genome_Creation_Error("Unrecognized gene name: " + gene_name + "\nin line: " + line);
            }
        }
        else if(String::trim_outer_whitespace(line_split[0]) == "Search Method")
        {
            const auto search_method_name_input = String::trim_outer_whitespace(line_split[1]);
            if(search_method_name_input == search_method_name(Search_Method::MINIMAX))
            {
                searching_method = Search_Method::MINIMAX;
            }
            else if(search_method_name_input == search_method_name(Search_Method::ITERATIVE_DEEPENING))
            {
                searching_method = Search_Method::ITERATIVE_DEEPENING;
            }
            else
            {
                throw Genome_Creation_Error("Unrecognized search method: " + line);
            }
        }
        else
        {
            throw Genome_Creation_Error("Bad line in genome file (expected Name): " + line);
        }
    }

    throw Genome_Creation_Error("Reached end of file before END of genome.");
}

double Genome::score_board(const Board& board, const Piece_Color perspective, size_t depth) const noexcept
{
    const auto progress_in_game = game_progress(board);
    return std::accumulate(genome.begin(), genome.end(), 0.0,
                           [&](auto sum, const auto& gene)
                           {
                               return sum + gene->evaluate(board, perspective, depth, progress_in_game);
                           });
}

double Genome::evaluate(const Board& board, const Piece_Color perspective, size_t depth) const noexcept
{
    return score_board(board, perspective, depth) - score_board(board, opposite(perspective), depth);
}

void Genome::mutate(const size_t mutation_count) noexcept
{
    for(size_t i = 0; i < mutation_count; ++i)
    {
        mutate();
    }
}

void Genome::mutate() noexcept
{
    // Create copies of genes equal in number to the number of mutatable components.
    std::vector<Gene*> genes;
    for(const auto& gene : genome)
    {
        genes.insert(genes.end(), gene->mutatable_components(), gene.get());
    }

    // Pick randomly from the list so every component has an equal chance for mutation.
    Random::random_element(genes)->mutate();
    renormalize_priorities();

    if(Random::success_probability(1, 1000))
    {
        searching_method = searching_method == Search_Method::MINIMAX ?
                           Search_Method::ITERATIVE_DEEPENING :
                           Search_Method::MINIMAX;
    }
}

int Genome::id() const noexcept
{
    return id_number;
}

std::string Genome::name() const noexcept
{
    return "Genetic Chess #" + std::to_string(id());
}

std::string Genome::author() const noexcept
{
    return "Mark Harrison";
}

void Genome::print(std::ostream& os) const noexcept
{
    os << "ID: " << id() << '\n';
    os << "Search Method: " << search_method_name() << "\n\n";
    for(const auto& gene : genome)
    {
        gene->print(os);
    }
    os << "END\n" << std::endl;
}

Clock::seconds Genome::time_to_examine(const Board& board, const Clock& clock) const noexcept
{
    return gene_reference<Look_Ahead_Gene>().time_to_examine(board, clock);
}

double Genome::speculation_time_factor(const double game_progress) const noexcept
{
    return gene_reference<Look_Ahead_Gene>().speculation_time_factor(game_progress);
}

double Genome::branching_factor(const double game_progress) const noexcept
{
    return gene_reference<Look_Ahead_Gene>().branching_factor(game_progress);
}

double Genome::expected_number_of_moves_left(const Board& board) const noexcept
{
    return gene_reference<Look_Ahead_Gene>().expected_moves_left(board);
}

double Genome::game_progress(const Board& board) const noexcept
{
    return gene_reference<Piece_Strength_Gene>().game_progress(board);
}

Search_Method Genome::search_method() const noexcept
{
    return searching_method;
}

std::string Genome::search_method_name() const noexcept
{
    return search_method_name(search_method());
}

std::string Genome::search_method_name(const Search_Method search_method) noexcept
{
    return search_method_names[static_cast<int>(search_method)];
}

const std::array<double, 6>& Genome::piece_values() const noexcept
{
    return gene_reference<Piece_Strength_Gene>().piece_values();
}
