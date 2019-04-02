#ifndef GENE_H
#define GENE_H

#include <map>
#include <string>
#include <iosfwd>
#include <memory>

class Board;
class Piece_Strength_Gene;

//! The base class of all genes that control the behavior of Genetic AIs.
class Gene
{
    public:
        Gene();
        virtual ~Gene() = default;

        void read_from(std::istream& is);
        void read_from(const std::string& file_name);

        void mutate();
        double evaluate(const Board& board, const Board& opposite_board, size_t depth) const;

        //! Copies the gene data and returns a pointer to the new data

        //! \returns A unique pointer to the copied data.
        virtual std::unique_ptr<Gene> duplicate() const = 0;

        size_t mutatable_components() const;

        //! Returns the name of the gene.

        //! \returns A text string with the name of the gene.
        virtual std::string name() const = 0;

        void print(std::ostream& os) const;

        virtual void reset_piece_strength_gene(const Piece_Strength_Gene* psg);

        bool test(const Board& board, double expected_score) const;

    protected:
        virtual std::map<std::string, double> list_properties() const;
        virtual void load_properties(const std::map<std::string, double>& properties);

        void make_priority_minimum_zero();

    private:
        double scoring_priority;

        virtual double score_board(const Board& board, const Board& opposite_board, size_t depth) const = 0;
        [[noreturn]] void throw_on_invalid_line(const std::string& line, const std::string& reason) const;
        virtual void gene_specific_mutation();
};

#endif // GENE_H
