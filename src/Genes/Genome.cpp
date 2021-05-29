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

Genome::Genome() noexcept :
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
        std::make_unique<Checkmate_Material_Gene>()
    }
{
    renormalize_priorities();
    reset_piece_strength_gene();

    assert(gene_reference<Piece_Strength_Gene>().name() == "Piece Strength Gene");
    assert(gene_reference<Look_Ahead_Gene>().name() == "Look Ahead Gene");
}

Genome::Genome(const Genome& other) noexcept
{
    std::transform(other.genome.begin(), other.genome.end(),
                   genome.begin(),
                   [](const auto& gene)
                   {
                       return gene->duplicate();
                   });
    reset_piece_strength_gene();
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
                   genome.begin(),
                   [](const auto& gene_a, const auto& gene_b)
                   {
                       return (Random::coin_flip() ? gene_a : gene_b)->duplicate();
                   });

    reset_piece_strength_gene();
    renormalize_priorities();
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
            throw Genetic_AI_Creation_Error("No colon in parameter line: " + line);
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
    Random::random_element(genes)->mutate();
    renormalize_priorities();
}

void Genome::print(std::ostream& os) const noexcept
{
    for(const auto& gene : genome)
    {
        gene->print(os);
    }
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

const std::array<double, 6>& Genome::piece_values() const noexcept
{
    return gene_reference<Piece_Strength_Gene>().piece_values();
}
