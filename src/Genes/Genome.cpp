#include <string>
#include <vector>

#include "Genes/Genome.h"

#include "Game/Board.h"
#include "Game/Color.h"
#include "Game/Game_Result.h"
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
#include "Genes/Castling_Possible_Gene.h"
#include "Genes/Piece_Strength_Gene.h"


// Creation ex nihilo
Genome::Genome()
{
    // Regulator genes
    genome.emplace_back(std::make_unique<Piece_Strength_Gene>());
    piece_strength_gene_index = genome.size() - 1;

    genome.emplace_back(std::make_unique<Look_Ahead_Gene>());
    look_ahead_gene_index = genome.size() - 1;

    // Normal genes
    auto psg = static_cast<Piece_Strength_Gene*>(genome[piece_strength_gene_index].get());

    genome.emplace_back(std::make_unique<Total_Force_Gene>(psg));
    genome.emplace_back(std::make_unique<Freedom_To_Move_Gene>());
    genome.emplace_back(std::make_unique<Pawn_Advancement_Gene>());
    genome.emplace_back(std::make_unique<Opponent_Pieces_Targeted_Gene>(psg));
    genome.emplace_back(std::make_unique<Sphere_of_Influence_Gene>());
    genome.emplace_back(std::make_unique<King_Confinement_Gene>());
    genome.emplace_back(std::make_unique<King_Protection_Gene>());
    genome.emplace_back(std::make_unique<Castling_Possible_Gene>());
}

// Cloning
Genome::Genome(const Genome& other) :
    genome(),
    piece_strength_gene_index(other.piece_strength_gene_index),
    look_ahead_gene_index(other.look_ahead_gene_index)
{
    for(const auto& gene : other.genome)
    {
        genome.emplace_back(gene->duplicate());
    }

    reseat_piece_strength_gene();
}

void Genome::reseat_piece_strength_gene()
{
    auto piece_strength_gene = static_cast<Piece_Strength_Gene*>(genome[piece_strength_gene_index].get());
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
    look_ahead_gene_index(A.look_ahead_gene_index)
{
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
    while(std::getline(is, line))
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

double Genome::score_board(const Board& board) const
{
    double score = 0;
    for(const auto& gene : genome)
    {
        score += gene->evaluate(board);
    }

    return score;
}

double Genome::evaluate(const Board& board, const Game_Result& result, Color perspective) const
{
    if(result.game_has_ended())
    {
        if(result.get_winner() == NONE) // stalemate
        {
            return 0;
        }
        else if(result.get_winner() == perspective) // checkmate win
        {
            return Math::win_score;
        }
        else // checkmate loss
        {
            return Math::lose_score;
        }
    }

    auto other_board = board;
    other_board.set_turn(opposite(board.whose_turn()));
    const auto& my_board =        (board.whose_turn() == perspective ? board : other_board);
    const auto& opponents_board = (board.whose_turn() == perspective ? other_board : board);

    return score_board(my_board) - score_board(opponents_board);
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

double Genome::time_to_examine(const Board& board, const Clock& clock) const
{
    return static_cast<Look_Ahead_Gene*>(genome[look_ahead_gene_index].get())->time_to_examine(board, clock);
}

double Genome::speculation_time_factor(const Board & board) const
{
    return static_cast<const Look_Ahead_Gene*>(genome[look_ahead_gene_index].get())->speculation_time_factor(board);
}
