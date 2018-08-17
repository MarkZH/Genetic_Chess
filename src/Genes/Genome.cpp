#include <string>
#include <vector>

#include "Genes/Genome.h"

#include "Game/Board.h"
#include "Game/Color.h"
#include "Game/Game_Result.h"
#include "Moves/Move.h"
#include "Utility.h"

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
#include "Genes/Priority_Threshold_Gene.h"
#include "Genes/Stacked_Pawns_Gene.h"
#include "Genes/Pawn_Islands_Gene.h"


// Creation ex nihilo
Genome::Genome()
{
    // Regulator genes
    genome.emplace_back(std::make_unique<Piece_Strength_Gene>());
    piece_strength_gene_index = genome.size() - 1;

    genome.emplace_back(std::make_unique<Look_Ahead_Gene>());
    look_ahead_gene_index = genome.size() - 1;

    genome.emplace_back(std::make_unique<Priority_Threshold_Gene>());
    priority_threshold_gene_index = genome.size() - 1;

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
}

// Cloning
Genome::Genome(const Genome& other) :
    genome(),
    piece_strength_gene_index(other.piece_strength_gene_index),
    look_ahead_gene_index(other.look_ahead_gene_index),
    priority_threshold_gene_index(other.priority_threshold_gene_index)
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

// Injection
Genome& Genome::operator=(const Genome& other)
{
    if(this == &other)
    {
        return *this;
    }

    piece_strength_gene_index = other.piece_strength_gene_index;
    look_ahead_gene_index = other.look_ahead_gene_index;
    priority_threshold_gene_index = other.priority_threshold_gene_index;

    genome.clear();
    for(const auto& gene : other.genome)
    {
        genome.emplace_back(gene->duplicate());
    }

    reset_piece_strength_gene();

    return *this;
}

// Sexual reproduction
Genome::Genome(const Genome& A, const Genome& B) :
    piece_strength_gene_index(A.piece_strength_gene_index),
    look_ahead_gene_index(A.look_ahead_gene_index),
    priority_threshold_gene_index(A.priority_threshold_gene_index)
{
    for(size_t i = 0; i < A.genome.size(); ++i)
    {
        auto& donor = (Random::coin_flip() ? A : B);
        genome.emplace_back(donor.genome[i]->duplicate());
    }

    reset_piece_strength_gene();
}

void Genome::read_from(std::istream& is)
{
    std::string line;
    while(std::getline(is, line))
    {
        line = String::strip_comments(line, '#');

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

double Genome::score_board(const Board& board, const Board& opposite_board, size_t depth, double minimum_priority) const
{
    double score = 0.0;
    for(const auto& gene : genome)
    {
        if(std::abs(gene->priority()) > minimum_priority)
        {
            score += gene->evaluate(board, opposite_board, depth);
        }
    }

    return score;
}

double Genome::evaluate(const Board& board, Color perspective, size_t depth) const
{
    auto other_board = board;
    other_board.set_turn(opposite(board.whose_turn()));
    const auto& my_board        = (board.whose_turn() == perspective ? board : other_board);
    const auto& opponents_board = (board.whose_turn() == perspective ? other_board : board);

    auto current_minimum_priority = Random::random_real(0.0, minimum_priority());
    return score_board(my_board, opponents_board, depth, current_minimum_priority) -
           score_board(opponents_board, my_board, depth, current_minimum_priority);
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
    std::for_each(genome.begin(), genome.end(), [&](const auto& gene){ gene->print(os); });
}

double Genome::time_to_examine(const Board& board, const Clock& clock) const
{
    return static_cast<const Look_Ahead_Gene*>(genome[look_ahead_gene_index].get())->time_to_examine(board, clock);
}

double Genome::speculation_time_factor(const Board& board, size_t depth) const
{
    return static_cast<const Look_Ahead_Gene*>(genome[look_ahead_gene_index].get())->speculation_time_factor(board, depth);
}

double Genome::minimum_priority() const
{
    return static_cast<const Priority_Threshold_Gene*>(genome[priority_threshold_gene_index].get())->threshold();
}
