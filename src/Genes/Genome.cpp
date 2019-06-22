#include "Genes/Genome.h"

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include <numeric>

#include "Game/Color.h"

#include "Utility/Random.h"
#include "Utility/String.h"

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
#include "Genes/Null_Gene.h"

#include "Exceptions/Genetic_AI_Creation_Error.h"

class Board;

size_t Genome::piece_strength_gene_index = size_t(-1);
size_t Genome::look_ahead_gene_index = size_t(-1);
size_t Genome::mutation_rate_gene_index = size_t(-1);

Genome::Genome()
{
    // Regulator genes
    genome.emplace_back(std::make_unique<Piece_Strength_Gene>());
    if(piece_strength_gene_index < genome.size())
    {
        if(piece_strength_gene_index != genome.size() - 1)
        {
            throw Genetic_AI_Creation_Error("Different genomes have different piece strength index values.");
        }
    }
    else
    {
        piece_strength_gene_index = genome.size() - 1;
    }

    genome.emplace_back(std::make_unique<Look_Ahead_Gene>());
    if(look_ahead_gene_index < genome.size())
    {
        if(look_ahead_gene_index != genome.size() - 1)
        {
            throw Genetic_AI_Creation_Error("Different genomes have different look ahead index values.");
        }
    }
    else
    {
        look_ahead_gene_index = genome.size() - 1;
    }

    genome.emplace_back(std::make_unique<Mutation_Rate_Gene>());
    if(mutation_rate_gene_index < genome.size())
    {
        if(mutation_rate_gene_index != genome.size() - 1)
        {
            throw Genetic_AI_Creation_Error("Different genomes have different mutation rate index values.");
        }
    }
    else
    {
        mutation_rate_gene_index = genome.size() - 1;
    }

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
    genome.emplace_back(std::make_unique<Null_Gene>());
}

Genome::Genome(const Genome& other)
{
    std::transform(other.genome.begin(), other.genome.end(),
                   std::back_inserter(genome),
                   [](const auto& gene)
                   {
                       return gene->duplicate();
                   });
    reset_piece_strength_gene();
}

void Genome::reset_piece_strength_gene()
{
    auto piece_strength_gene = static_cast<const Piece_Strength_Gene*>(genome[piece_strength_gene_index].get());
    for(auto& gene : genome)
    {
        gene->reset_piece_strength_gene(piece_strength_gene);
    }
}

Genome& Genome::operator=(Genome other)
{
    std::swap(genome, other.genome);
    reset_piece_strength_gene();

    return *this;
}

Genome::Genome(const Genome& A, const Genome& B)
{
    std::transform(A.genome.begin(), A.genome.end(), B.genome.begin(),
                   std::back_inserter(genome),
                   [](const auto& gene_a, const auto& gene_b)
                   {
                       return (Random::coin_flip() ? gene_a : gene_b)->duplicate();
                   });

    reset_piece_strength_gene();
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
            return;
        }

        auto line_split = String::split(line, ":", 1);
        if(line_split.size() != 2)
        {
            throw Genetic_AI_Creation_Error("No colon in parameter line: " + line);
        }

        if(String::trim_outer_whitespace(line_split[0]) == "Name")
        {
            auto gene_name = String::trim_outer_whitespace(line_split[1]);
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

double Genome::score_board(const Board& board, Color perspective, size_t depth) const
{
    return std::accumulate(genome.begin(), genome.end(), 0.0,
                           [&](auto sum, const auto& gene)
                           {
                               return sum + gene->evaluate(board, perspective, depth);
                           });
}

double Genome::evaluate(const Board& board, Color perspective, size_t depth) const
{
    return score_board(board, perspective, depth) - score_board(board, opposite(perspective), depth);
}

void Genome::mutate()
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
    }
}

void Genome::print(std::ostream& os) const
{
    for(const auto& gene : genome)
    {
        gene->print(os);
    };
}

double Genome::time_to_examine(const Board& board, const Clock& clock) const
{
    return static_cast<const Look_Ahead_Gene*>(genome[look_ahead_gene_index].get())->time_to_examine(board, clock);
}

double Genome::speculation_time_factor(const Board& board) const
{
    return static_cast<const Look_Ahead_Gene*>(genome[look_ahead_gene_index].get())->speculation_time_factor(board);
}

double Genome::components_to_mutate() const
{
    return static_cast<const Mutation_Rate_Gene*>(genome[mutation_rate_gene_index].get())->mutation_count();
}
