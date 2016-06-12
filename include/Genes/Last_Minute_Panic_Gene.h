#ifndef LAST_MINUTE_PANIC_GENE_H
#define LAST_MINUTE_PANIC_GENE_H

#include <string>

#include "Genes/Gene.h"


class Last_Minute_Panic_Gene : public Gene
{
    public:
        Last_Minute_Panic_Gene();
        virtual ~Last_Minute_Panic_Gene() override;

        void mutate() override;
        Last_Minute_Panic_Gene* duplicate() const override;

        double score_board(const Board& board, Color perspective) const override;

        std::string name() const override;

        double time_required() const;

    private:
        void reset_properties() const override;
        void load_properties() override;

        double minimum_time;
};

#endif // LAST_MINUTE_PANIC_GENE_H
