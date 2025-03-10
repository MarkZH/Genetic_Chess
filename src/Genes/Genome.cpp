#include "Genes/Genome.h"

#include <string>
#include <array>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <functional>
#include <print>
#include <format>

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
#include "Genes/Checkmate_Material_Gene.h"
#include "Genes/Pawn_Structure_Gene.h"
#include "Genes/Move_Sorting_Gene.h"

namespace
{
    int next_id = 0;
}

Genome::Genome() noexcept :
    id_number(next_id++),
    genome{
        std::make_unique<Piece_Strength_Gene>(),
        std::make_unique<Look_Ahead_Gene>(),
        std::make_unique<Move_Sorting_Gene>(),
        std::make_unique<Total_Force_Gene>(nullptr),
        std::make_unique<Freedom_To_Move_Gene>(),
        std::make_unique<Pawn_Advancement_Gene>(),
        std::make_unique<Passed_Pawn_Gene>(),
        std::make_unique<Opponent_Pieces_Targeted_Gene>(nullptr),
        std::make_unique<Sphere_of_Influence_Gene>(),
        std::make_unique<King_Confinement_Gene>(),
        std::make_unique<King_Protection_Gene>(),
        std::make_unique<Castling_Possible_Gene>(),
        std::make_unique<Checkmate_Material_Gene>(),
        std::make_unique<Pawn_Structure_Gene>()
    }
{
    reset_piece_strength_gene();

    assert(gene_reference<Piece_Strength_Gene>().name() == "Piece Strength Gene");
    assert(gene_reference<Look_Ahead_Gene>().name() == "Look Ahead Gene");
    assert(gene_reference<Move_Sorting_Gene>().name() == "Move Sorting Gene");
}

Genome::Genome(const Genome& other) noexcept : id_number(other.id())
{
    std::ranges::transform(other.genome, genome.begin(), std::mem_fn(&Gene::duplicate));
    reset_piece_strength_gene();
}

Genome::Genome(std::istream& is, int id_in) : Genome()
{
    id_number = id_in;

    // Genome() increments next_id, but this Genome doesn't use it.
    // Save it for the next new Genome.
    --next_id;

    if( ! is)
    {
        throw Genome_Creation_Error("Could not read: ");
    }

    for(std::string line; std::getline(is, line);)
    {
        line = String::strip_comments(line, "#");
        if( ! line.starts_with("ID"))
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
                return std::format("Error in creating Genome #{}\n{}\nFile: ", id(), e.what());
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
            catch(const std::exception& e)
            {
                throw Genome_Creation_Error(add_details(e));
            }

            next_id = std::max(next_id, id() + 1);
            return;
        }
    }

    throw Genome_Creation_Error(std::format("Could not locate ID {} inside file ", id_number));
}

void Genome::reset_piece_strength_gene() noexcept
{
    auto piece_strength_gene = &gene_reference<Piece_Strength_Gene>();
    for(auto& gene : genome)
    {
        gene->reset_piece_strength_gene(piece_strength_gene);
    }
}

Genome& Genome::operator=(const Genome& other) noexcept
{
    id_number = other.id();
    std::ranges::transform(other.genome, genome.begin(), std::mem_fn(&Gene::duplicate));
    reset_piece_strength_gene();
    return *this;
}

Genome::Genome(const Genome& A, const Genome& B) noexcept : id_number(next_id++)
{
    std::ranges::transform(A.genome, B.genome,
                           genome.begin(),
                           [](const auto& gene_a, const auto& gene_b)
                           {
                               return (Random::coin_flip() ? gene_a : gene_b)->duplicate();
                           });

    reset_piece_strength_gene();
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
            return;
        }

        const auto line_split = String::split(line, ":", 1);
        if(line_split.size() != 2)
        {
            throw Genome_Creation_Error(std::format("No colon in parameter line: {}", line));
        }

        if(String::trim_outer_whitespace(line_split[0]) == "Name")
        {
            const auto gene_name = String::remove_extra_whitespace(line_split[1]);
            const auto found_gene =
                std::ranges::find_if(genome, [&gene_name](const auto& gene) { return gene->name() == gene_name; });
            if(found_gene != genome.end())
            {
                (*found_gene)->read_from(is);
            }
            else
            {
                throw Genome_Creation_Error(std::format("Unrecognized gene name: {}\nin line: {}", gene_name, line));
            }
        }
        else
        {
            throw Genome_Creation_Error(std::format("Bad line in genome file (expected Name): {}", line));
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
    // Create copies of genes equal in number to the number of mutatable components.
    std::vector<Gene*> genes;
    for(const auto& gene : genome)
    {
        genes.insert(genes.end(), gene->mutatable_components(), gene.get());
    }

    // Pick randomly from the list so every component has an equal chance for mutation.
    for(size_t i = 0; i < mutation_count; ++i)
    {
        Random::random_element(genes)->mutate();
    }
}

int Genome::id() const noexcept
{
    return id_number;
}

std::string Genome::name() const noexcept
{
    return std::format("Genetic Chess #{}", id());
}

std::string Genome::author() const noexcept
{
    return "Mark Harrison";
}

void Genome::print(std::ostream& os) const noexcept
{
    std::println(os, "ID: {}", id());
    for(const auto& gene : genome)
    {
        gene->print(os);
    }
    std::print(os, "END\n\n");
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

double Genome::game_progress(const Board& board) const noexcept
{
    return gene_reference<Piece_Strength_Gene>().game_progress(board);
}

const std::array<double, 6>& Genome::piece_values() const noexcept
{
    return gene_reference<Piece_Strength_Gene>().piece_values();
}
