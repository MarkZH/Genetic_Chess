#include "Genes/Genome.h"

#include "Game/Board.h"
#include "Game/Color.h"
#include "Utility.h"

#include "Genes/Gene.h"

#include "Genes/Total_Force_Gene.h"
#include "Genes/Freedom_To_Move_Gene.h"
#include "Genes/Pawn_Advancement_Gene.h"
#include "Genes/Opponent_Pieces_Targeted_Gene.h"
#include "Genes/Sphere_of_Influence_Gene.h"
#include "Genes/Look_Ahead_Gene.h"
#include "Genes/King_Confinement_Gene.h"
#include "Genes/King_Protection_Gene.h"
#include "Genes/Branch_Pruning_Gene.h"


// Creation ex nihilo
Genome::Genome() :
    piece_strength_gene_index(-1),
    look_ahead_gene_index(-1),
    branch_pruning_gene_index(-1)
{
    // Regulator genes
    genome.emplace_back(new Piece_Strength_Gene);
    piece_strength_gene_index = genome.size() - 1;

    genome.emplace_back(new Look_Ahead_Gene);
    look_ahead_gene_index = genome.size() - 1;

    genome.emplace_back(new Branch_Pruning_Gene);
    branch_pruning_gene_index = genome.size() - 1;

    // Normal genes
    if(piece_strength_gene_index < genome.size())
    {
        genome.emplace_back(new Total_Force_Gene(std::static_pointer_cast<Piece_Strength_Gene>(genome[piece_strength_gene_index])));
    }
    genome.emplace_back(new Freedom_To_Move_Gene);
    genome.emplace_back(new Pawn_Advancement_Gene);
    if(piece_strength_gene_index < genome.size())
    {
        genome.emplace_back(new Opponent_Pieces_Targeted_Gene(std::static_pointer_cast<Piece_Strength_Gene>(genome[piece_strength_gene_index])));
    }
    genome.emplace_back(new Sphere_of_Influence_Gene);
    genome.emplace_back(new King_Confinement_Gene);
    genome.emplace_back(new King_Protection_Gene);
}

// Cloning
Genome::Genome(const Genome& other) :
    genome(),
    piece_strength_gene_index(other.piece_strength_gene_index),
    look_ahead_gene_index(other.look_ahead_gene_index),
    branch_pruning_gene_index(other.branch_pruning_gene_index)
{
    // Copy all other genes
    for(const auto& gene : other.genome)
    {
        genome.emplace_back(gene->duplicate());
    }

    reseat_piece_strength_gene();
}

void Genome::reseat_piece_strength_gene()
{
    auto piece_strength_gene = std::static_pointer_cast<Piece_Strength_Gene>
        (genome[piece_strength_gene_index]);
    for(auto& gene : genome)
    {
        gene->reset_piece_strength_gene(piece_strength_gene);
    }
}

// Injection
Genome& Genome::operator=(const Genome& other)
{
    if(this == &other)
    {
        return *this;
    }

    piece_strength_gene_index = other.piece_strength_gene_index;
    look_ahead_gene_index = other.look_ahead_gene_index;
    branch_pruning_gene_index = other.branch_pruning_gene_index;

    genome.clear();
    for(const auto& gene : other.genome)
    {
        genome.emplace_back(gene->duplicate());
    }

    reseat_piece_strength_gene();

    return *this;
}

// Sexual reproduction
Genome::Genome(const Genome& A, const Genome& B) :
    genome(),
    piece_strength_gene_index(A.piece_strength_gene_index),
    look_ahead_gene_index(A.look_ahead_gene_index),
    branch_pruning_gene_index(A.branch_pruning_gene_index)
{
    // Copy all other genes
    for(size_t i = 0; i < A.genome.size(); ++i)
    {
        auto& donor = (Random::coin_flip() ? A : B);
        genome.emplace_back(donor.genome[i]->duplicate());
    }

    reseat_piece_strength_gene();
}

void Genome::read_from(std::istream& is)
{
    std::string line;
    while(getline(is, line))
    {
        if(line.empty())
        {
            continue;
        }

        if(line == "END")
        {
            return;
        }

        if(String::starts_with(line, "Name:"))
        {
            auto gene_name = String::split(line, ": ")[1];
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
    }
}

double Genome::score_board(const Board& board, Color perspective) const
{
    double score = 0;
    for(const auto& gene : genome)
    {
        score += gene->evaluate(board, perspective);
    }

    return score;
}

double Genome::evaluate(const Board& board, Color perspective) const
{
    if(board.game_has_ended())
    {
        if(board.get_winner() == NONE) // stalemate
        {
            return 0;
        }
        else if(board.get_winner() == perspective) // checkmate win
        {
            return Math::infinity;
        }
        else // checkmate loss
        {
            return -Math::infinity;
        }
    }

    return score_board(board, perspective) - score_board(board, opposite(perspective));
}

void Genome::mutate()
{
    for(auto& gene : genome)
    {
        const int mean_number_of_mutations = 2;
        if(Random::random_integer(1, genome.size()) <= mean_number_of_mutations)
        {
            gene->mutate();
        }
    }
}

void Genome::print(std::ostream& os) const
{
    for(const auto& gene : genome)
    {
        gene->print(os);
    }
    os << "\n";
}

int Genome::positions_to_examine(const Board& board, const Clock& clock) const
{
    return std::static_pointer_cast<Look_Ahead_Gene>
        (genome[look_ahead_gene_index])->positions_to_examine(board, clock);
}

bool Genome::good_enough_to_examine(const Board& before, const Board& after, Color perspective) const
{
    auto score_difference = evaluate(after, perspective) - evaluate(before, perspective);
    return std::static_pointer_cast<Branch_Pruning_Gene>
        (genome[branch_pruning_gene_index])->good_enough_to_examine(score_difference);
}
