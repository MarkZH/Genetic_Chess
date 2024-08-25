#ifndef RESIGNATION_GENE_h

#include "Gene.h"
#include "Gene_Value.h"
#include "Game/Color.h"

#include <vector>

class Game_Tree_Node_Result;

class Resignation_Gene : public Clonable_Gene<Resignation_Gene>
{
    public:
        //! \brief Index for locating the gene in the genome
        static const size_t genome_index = 3;

        Resignation_Gene() noexcept;

        bool should_resign(const std::vector<Game_Tree_Node_Result>& commentary, Piece_Color perspective) const noexcept;
        void set_centipawn_value(double centipawn_value) const noexcept;

    private:
        Gene_Value board_score_floor{"Score Floor", -10.0, 1.0};
        Gene_Value max_under_floor_streak{"Max Under Floor Streak", 20.0, 1.0};
        mutable double internal_score_floor = board_score_floor.value();
        
        void gene_specific_mutation() noexcept override;
        void adjust_properties(std::map<std::string, std::string>& properties) const noexcept override;
        void load_gene_properties(const std::map<std::string, std::string>& properties) override;

        double score_board(const Board& board, Piece_Color perspective, size_t depth) const noexcept override;
};

#endif // !RESIGNATION_GENE_h
