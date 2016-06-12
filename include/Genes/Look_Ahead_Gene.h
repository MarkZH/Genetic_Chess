#ifndef LOOK_AHEAD_GENE_H
#define LOOK_AHEAD_GENE_H

#include <iosfwd>

#include "Genes/Gene.h"


class Look_Ahead_Gene : public Gene
{
    public:
        Look_Ahead_Gene();
        virtual ~Look_Ahead_Gene() override;

        Look_Ahead_Gene* duplicate() const override;
        void mutate() override;

        double score_board(const Board& board, Color perspective) const override;

        std::string name() const override;

        size_t positions_to_examine(double time_left) const;

    private:
        void reset_properties() const override;
        void load_properties() override;

        double look_ahead_constant;
};

#endif // LOOK_AHEAD_GENE_H
