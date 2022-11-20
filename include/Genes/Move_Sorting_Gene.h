#ifndef MOVE_SORTING_GENE_H
#define MOVE_SORTING_GENE_H

#include "Genes/Gene.h"

#include <vector>
#include <functional>
#include <string>

class Board;
class Move;

//! \brief A named function for sorting moves based on the sorter predicate.
//!
//! The sorter function is fed into std::partition to put moves in a better search order.
struct Move_Sorter
{
    //! The name of the sorting function for reading/writing to a genome file.
    std::string name;

    //! The sorting predicate. Moves for which this function returns true will
    //! be moved earlier than moves which return false.
    std::function<bool(const Move*, const Board&)> sorter;
};

//! \brief A gene for sorting moves before searching deeper in the game tree.
//!
//! Since moves that make larger changes to the game state are likely to induce
//! alpha-beta pruning earlier, sorting moves can lead to a large saving in
//! search time.
class Move_Sorting_Gene : public Clonable_Gene<Move_Sorting_Gene>
{
    public:
        //! \brief Index for locating the gene in the genome
        static const size_t genome_index = 2;

        Move_Sorting_Gene() noexcept;

        //! \brief Sort moves before searching further in the game tree.
        //! \tparam Iter An iterator type that points to a const Move*.
        //! \param begin An iterator to the beginning of the move list to be sorted.
        //! \param end An iterator to the end of the move list to be sorted.
        //! \param board The board from which the move list is derived.
        template<typename Iter>
        void sort_moves(Iter begin, Iter end, const Board& board) const noexcept
        {
            for(size_t i = 0; i < sorter_count; ++i)
            {
                begin = std::partition(begin, end, [this, &board, i](const auto move) { return move_sorters[i].sorter(move, board); });
            }
        }

    private:
        std::vector<Move_Sorter> move_sorters;
        size_t sorter_count = 0;

        double score_board(const Board& board, Piece_Color perspective, size_t depth) const noexcept override;

        void gene_specific_mutation() noexcept override;

        void adjust_properties(std::map<std::string, std::string>& properties) const noexcept override;
        void load_gene_properties(const std::map<std::string, std::string>& properties) override;
};

#endif // MOVE_SORTING_GENE_H
