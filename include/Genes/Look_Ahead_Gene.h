#ifndef LOOK_AHEAD_GENE_H
#define LOOK_AHEAD_GENE_H

#include <iosfwd>

#include "Genes/Gene.h"


class Look_Ahead_Gene : public Gene
{
    public:
        Look_Ahead_Gene();
        ~Look_Ahead_Gene();

        Look_Ahead_Gene* duplicate() const override;
        void mutate() override;

        double score_board(const Board& board, Color color) const override;

        std::string name() const override;

        size_t look_ahead(double time, size_t choices_per_move) const;

    private:
        void reset_properties() const override;
        void load_properties() override;

        double look_ahead_constant;
};

#endif // LOOK_AHEAD_GENE_H
