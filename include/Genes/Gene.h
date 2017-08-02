#ifndef GENE_H
#define GENE_H

#include <map>
#include <string>
#include <iosfwd>
#include <memory>

class Board;
class Piece_Strength_Gene;

class Gene
{
    public:
        Gene();
        virtual ~Gene() = default;

        void read_from(std::istream& is);

        void mutate();
        double evaluate(const Board& board) const;
        virtual std::unique_ptr<Gene> duplicate() const = 0;

        virtual std::string name() const = 0;
        void print(std::ostream& os) const;

        virtual void reset_piece_strength_gene(const Piece_Strength_Gene* psg);

    protected:
        mutable std::map<std::string, double> properties; // used to simplify reading/writing from/to files
        virtual void reset_properties() const;
        virtual void load_properties();
        void make_priority_non_negative();

    private:
        virtual double score_board(const Board& board) const = 0;
        [[noreturn]] void throw_on_invalid_line(const std::string& line, const std::string& reason) const;
        virtual void gene_specific_mutation();

        double priority;
        bool priority_non_negative;
};

#endif // GENE_H
