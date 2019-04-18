#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

#include "Genes/Genome.h"

#include "Game/Board.h"
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

size_t Genome::piece_strength_gene_index = -1;
size_t Genome::look_ahead_gene_index = -1;
size_t Genome::mutation_rate_gene_index = -1;

//! Create a genome with default-constructed (neutral behavior) Genes.
Genome::Genome()
{
    // Regulator genes
    genome.emplace_back(std::make_unique<Piece_Strength_Gene>());
    if(piece_strength_gene_index < genome.size())
    {
        if(piece_strength_gene_index != genome.size() - 1)
        {
            throw std::logic_error("Different genomes have different piece strength index values.");
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
            throw std::logic_error("Different genomes have different look ahead index values.");
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
            throw std::logic_error("Different genomes have different mutation rate index values.");
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

//! Clone a genome

//! \param other The originating genome.
Genome::Genome(const Genome& other)
{
    for(const auto& gene : other.genome)
    {
        genome.emplace_back(gene->duplicate());
    }

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

//! Inject another genome's data into this one (i.e., assignment operator)

//! \param other The originating genome.
Genome& Genome::operator=(const Genome& other)
{
    if(this == &other)
    {
        return *this;
    }

    genome.clear();
    for(const auto& gene : other.genome)
    {
        genome.emplace_back(gene->duplicate());
    }

    reset_piece_strength_gene();

    return *this;
}

//! Create a new genome from two existing genomes via sexual reproduction

//! Create each Gene by copying from either parent with a 50-50 probability.
//! \param A The first parent.
//! \param B The second parent.
Genome::Genome(const Genome& A, const Genome& B)
{
    for(size_t i = 0; i < A.genome.size(); ++i)
    {
        auto& donor = (Random::coin_flip() ? A : B);
        genome.emplace_back(donor.genome[i]->duplicate());
    }

    reset_piece_strength_gene();
}

//! Read genome data from an input stream (std::ifstream, std::cin, etc.).

//! \param is The input stream.
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
            throw std::runtime_error("No colon in parameter line: " + line);
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
                throw std::runtime_error("Unrecognized gene name: " + gene_name + "\nin line: " + line);
            }
        }
        else
        {
            throw std::runtime_error("Bad line in genome file (expected Name): " + line);
        }
    }

    throw std::runtime_error("Reached end of file before END of genome.");
}

double Genome::score_board(const Board& board, Color perspective, size_t depth) const
{
    double score = 0.0;
    for(const auto& gene : genome)
    {
        score += gene->evaluate(board, perspective, depth);
    }

    return score;
}

//! Evaluate a board position and return a numerical value.

//! The higher the value, the greater the probability that the board position
//! will lead to victory for the player doing the scoring.

//! \param board The board position to be evaluated.
//! \param perspective The player for whom a higher score means a greater chance of victory.
//! \param depth How man moves in the future the current board state is compared to the
//!        original board that represents the current state of the game.
double Genome::evaluate(const Board& board, Color perspective, size_t depth) const
{
    return score_board(board, perspective, depth) - score_board(board, opposite(perspective), depth);
}

//! Apply a random set of mutations to the entire genome.

//! The severity of the mutation is controlled by the Mutation_Rate_Gene.
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
        genes[Random::random_integer(0, int(genes.size()) - 1)]->mutate();
    }
}

//! Print the genome data to the output stream (std::ofstream, std::cout, etc.).

//! \param os The output stream.
void Genome::print(std::ostream& os) const
{
    for(const auto& gene : genome)
    {
        gene->print(os);
    };
}

//! Determine how much time should be used to choose a move, that is, for the entire search.

//! \param board The current board position.
//! \param clock The game clock.
double Genome::time_to_examine(const Board& board, const Clock& clock) const
{
    return static_cast<const Look_Ahead_Gene*>(genome[look_ahead_gene_index].get())->time_to_examine(board, clock);
}

//! Returns a factor that is multiplied by the time allocated to examine a branch of the game tree.

//! With alpha-beta pruning, the search for the best move often cuts off before the entire branch
//! of the game tree is explored, using less time than expected. The value returned by this function
//! is multiplied by the time allocated for a branch to make sure the time allocated is actually used,
//! allowing deeper searches of the game tree. More or less time may be allocated based on the
//! board position being examined.
//! \param board The board position being examined.
//! \returns The multiplicative factor.
double Genome::speculation_time_factor(const Board& board) const
{
    return static_cast<const Look_Ahead_Gene*>(genome[look_ahead_gene_index].get())->speculation_time_factor(board);
}

//! Consults the Mutation_Rate_Gene to determine how many point mutations to apply to the genome.

//! \returns A number of mutations to apply.
double Genome::components_to_mutate() const
{
    return static_cast<const Mutation_Rate_Gene*>(genome[mutation_rate_gene_index].get())->mutation_count();
}
