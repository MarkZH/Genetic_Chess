#ifndef SEARCH_STRATEGY_GENE_H
#define SEARCH_STRATEGY_GENE_H

#include "Gene.h"

#include <string>
#include <map>

#include "Search_Method.h"
#include "Gene_Value.h"

#include "Game/Color.h"

class Board;

//! \brief Gene determines whether standard minimax search is used or iterative deepening
class Search_Strategy_Gene : public Clonable_Gene<Search_Strategy_Gene>
{
    public:
        //! \brief Index for locating the gene in the genome
        static constexpr size_t genome_index = 2;

        std::string name() const noexcept override;

        //! \brief Returns the current search method
        Search_Method searching_method() const noexcept;

    private:
        Gene_Value<Search_Method> search_method = {"Search Method", Search_Method::MINIMAX};

        double score_board(const Board& board, Piece_Color perspective, size_t depth, double game_progress) const noexcept override;
        void gene_specific_mutation() noexcept override;

        void adjust_properties(std::map<std::string, std::string>& properties) const noexcept override;
        void load_gene_properties(const std::map<std::string, std::string>& properties) override;
};

#endif // SEARCH_STRATEGY_GENE_H
