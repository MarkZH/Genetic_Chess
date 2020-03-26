#include "Genes/Genome.h"

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <numeric>

#include "Game/Color.h"
#include "Game/Clock.h"

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
#include "Genes/Mutation_Rate_Gene.h"

class Board;

namespace
{
     constexpr auto piece_strength_gene_index = size_t(0);
     constexpr auto look_ahead_gene_index = size_t(1);
     constexpr auto mutation_rate_gene_index = size_t(2);
}

Genome::Genome() noexcept
{
    // Regulator genes
    genome.emplace_back(std::make_unique<Piece_Strength_Gene>());
    assert(genome[piece_strength_gene_index]->name() == "Piece Strength Gene");
    genome.emplace_back(std::make_unique<Look_Ahead_Gene>());
    assert(genome[look_ahead_gene_index]->name() == "Look Ahead Gene");
    genome.emplace_back(std::make_unique<Mutation_Rate_Gene>());
    assert(genome[mutation_rate_gene_index]->name() == "Mutation Rate Gene");

    // Normal genes
    auto psg = static_cast<const Piece_Strength_Gene*>(genome[piece_strength_gene_index].get());

    genome.emplace_back(std::make_unique<Total_Force_Gene>(psg));
    genome.emplace_back(std::make_unique<Freedom_To_Move_Gene>());
    genome.emplace_back(std::make_unique<Pawn_Advancement_Gene>());
    genome.emplace_back(std::make_unique<Passed_Pawn_Gene>());
    genome.emplace_back(std::make_unique<Opponent_Pieces_Targeted_Gene>(psg));
    genome.emplace_back(std::make_unique<Sphere_of_Influence_Gene>());
    genome.emplace_back(std::make_unique<King_Confinement_Gene>());
    genome.emplace_back(std::make_unique<King_Protection_Gene>());
    genome.emplace_back(std::make_unique<Castling_Possible_Gene>());
    genome.emplace_back(std::make_unique<Stacked_Pawns_Gene>());
    genome.emplace_back(std::make_unique<Pawn_Islands_Gene>());
    genome.emplace_back(std::make_unique<Checkmate_Material_Gene>());

    renormalize_priorities();
}

Genome::Genome(const Genome& other) noexcept
{
    std::transform(other.genome.begin(), other.genome.end(),
                   std::back_inserter(genome),
                   [](const auto& gene)
                   {
                       return gene->duplicate();
                   });
    reset_piece_strength_gene();
}

void Genome::reset_piece_strength_gene() noexcept
{
    auto piece_strength_gene = static_cast<const Piece_Strength_Gene*>(genome[piece_strength_gene_index].get());
    for(auto& gene : genome)
    {
        gene->reset_piece_strength_gene(piece_strength_gene);
    }
}

void Genome::renormalize_priorities() noexcept
{
    auto norm = std::accumulate(genome.begin(), genome.end(), 0.0,
                                [](auto sum, const auto& gene)
                                {
                                    return sum + std::abs(gene->priority());
                                });
    if(norm > 0.0)
    {
        for(auto& gene : genome)
        {
            gene->scale_priority(1.0/norm);
        }
    }
}

Genome& Genome::operator=(const Genome& other) noexcept
{
    std::transform(other.genome.begin(), other.genome.end(),
                   genome.begin(),
                   [](const auto& gene)
                   {
                       return gene->duplicate();
                   });
    reset_piece_strength_gene();
    return *this;
}

Genome::Genome(const Genome& A, const Genome& B) noexcept
{
    std::transform(A.genome.begin(), A.genome.end(), B.genome.begin(),
                   std::back_inserter(genome),
                   [](const auto& gene_a, const auto& gene_b)
                   {
                       return (Random::coin_flip() ? gene_a : gene_b)->duplicate();
                   });

    reset_piece_strength_gene();
    renormalize_priorities();
}

void Genome::read_from(std::istream& is)
{
    std::string line;
    while(std::getline(is, line))
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

        auto line_split = String::split(line, ":", 1);
        if(line_split.size() != 2)
        {
            throw Genetic_AI_Creation_Error("No colon in parameter line: " + line);
        }

        if(String::trim_outer_whitespace(line_split[0]) == "Name")
        {
            auto gene_name = String::remove_extra_whitespace(line_split[1]);
            bool gene_found = false;
            for(auto& gene : genome)
            {
                if(gene->name() == gene_name)
                {
                    gene->read_from(is);
                    gene_found = true;
                    break;
                }
            }

            if( ! gene_found)
            {
                throw Genetic_AI_Creation_Error("Unrecognized gene name: " + gene_name + "\nin line: " + line);
            }
        }
        else
        {
            throw Genetic_AI_Creation_Error("Bad line in genome file (expected Name): " + line);
        }
    }

    throw Genetic_AI_Creation_Error("Reached end of file before END of genome.");
}

double Genome::score_board(const Board& board, Piece_Color perspective, size_t depth) const noexcept
{
    return std::accumulate(genome.begin(), genome.end(), 0.0,
                           [&](auto sum, const auto& gene)
                           {
                               return sum + gene->evaluate(board, perspective, depth);
                           });
}

double Genome::evaluate(const Board& board, Piece_Color perspective, size_t depth) const noexcept
{
    return score_board(board, perspective, depth) - score_board(board, opposite(perspective), depth);
}

void Genome::mutate() noexcept
{
    // Create copies of genes based on the number of internal components
    // that are mutatable
    std::vector<Gene*> genes;
    for(const auto& gene : genome)
    {
        genes.insert(genes.end(), gene->mutatable_components(), gene.get());
    }

    // Pick randomly from the list of copies to make sure genes with
    // more components don't lack for mutations.
    auto mutation_count = components_to_mutate();
    for(auto mutations = 0; mutations < mutation_count; ++mutations)
    {
        Random::random_element(genes)->mutate();
        renormalize_priorities();
    }
}

void Genome::print(std::ostream& os) const noexcept
{
    for(const auto& gene : genome)
    {
        gene->print(os);
    };
}

Clock::seconds Genome::time_to_examine(const Board& board, const Clock& clock) const noexcept
{
    return gene_reference<Look_Ahead_Gene, look_ahead_gene_index>().time_to_examine(board, clock);
}

double Genome::speculation_time_factor() const noexcept
{
    return gene_reference<Look_Ahead_Gene, look_ahead_gene_index>().speculation_time_factor();
}

const std::array<double, 6>& Genome::piece_values() const noexcept
{
    return gene_reference<Piece_Strength_Gene, piece_strength_gene_index>().piece_values();
}

int Genome::components_to_mutate() const noexcept
{
    return gene_reference<Mutation_Rate_Gene, mutation_rate_gene_index>().mutation_count();
}
