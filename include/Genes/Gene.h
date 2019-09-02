#ifndef GENE_H
#define GENE_H

#include <map>
#include <string>
#include <iosfwd>
#include <memory>

#include "Game/Color.h"

class Board;
class Piece_Strength_Gene;

//! The base class of all genes that control the behavior of Genetic AIs.
class Gene
{
    public:
        //! Instantiate the basic parts of the abstract class Gene.
        //
        //! \param non_negative_priority Specifies whether the scoring priority should be restricted
        //!        to values greater than or equal to zero.
        //!
        //! For some genes, having the priority vary across positive and negative numbers
        //! slows down evolution because it has some other multiplicative factor that can
        //! also take on negative or positive values. For example, the Total_Force_Gene priority
        //! is effectively multliplied by the scores from the Piece_Strength_Gene. Since switching
        //! the sign of both leads to the same behavior, making the priority non-negative cuts
        //! off the redundant half of the search space.
        explicit Gene(bool non_negative_priority = false);

        virtual ~Gene() = default;

        //! Reads an input stream for gene data.
        //
        //! Every line should be one of the following:
        //! - \<property\> = \<value\> with optional comments at the end preceded by '#'.
        //! - blank
        //! - A commented line starting with a '#'.
        //! \param is An input stream (std::ifstream, std::iostream, or similar).
        //! \throws Genetic_AI_Creation_Error If there is an invalid line or an unexpected property
        void read_from(std::istream& is);

        //! Read gene data from a text file.
        //
        //! The method creates an input stream from the text file and passes it to the
        //! other Gene::read_from().
        void read_from(const std::string& file_name);

        //! Applies a random mutation to the priority or other aspect of a gene.
        void mutate();

        //! Gives a numerical score to the board in the arguments.
        //
        //! \param board The state of the board to be evaluated--found at the leaves of the game search tree.
        //! \param perspective For which player the board is being scored.
        //! \param prior_real_moves How many of the first moves on the board represent real moves from the current game.
        //! \returns A numerical score indicating the likelihood that the board in the first argument is winning for board.whose_turn().
        double evaluate(const Board& board, Color perspective, size_t prior_real_moves) const;

        //! Copies the gene data and returns a pointer to the new data
        //
        //! \returns A unique pointer to the copied data.
        virtual std::unique_ptr<Gene> duplicate() const = 0;

        //! Tells how many mutatable components are present in a gene.
        //
        //! The more components a gene has, the more likely it will be mutated by a call
        //! to Genome::mutate().
        //! \returns How many individually mutatable components exist in a gene.
        size_t mutatable_components() const;

        //! Returns the name of the gene.
        //
        //! \returns A text string with the name of the gene.
        virtual std::string name() const = 0;

        //! Outputs gene data to a std::ostream in text form.
        //
        //! \param os An output stream (std::ofstream, std::cout, etc.)
        void print(std::ostream& os) const;

        //! Gives the gene the new location of the Piece Strength Gene.
        //
        //! When a Genome is copied or assigned to, the Genes that reference
        //! the Piece_Strength_Gene need to be told the location of the new
        //! copy. For all other genes, this does nothing.
        //! \param psg A pointer to the correct Piece Strength Gene.
        virtual void reset_piece_strength_gene(const Piece_Strength_Gene* psg);

        //! Tests the board-scoring method of the Gene.
        //
        //! \param[out] test_variable If the test fails, this parameter is set to false. Otherwise, it keeps
        //!        its original value.
        //! \param board The board upon which the test takes place.
        //! \param perspective The player for whom the score is being calculated.
        //! \param expected_score The expected score returned by Gene::score_board().
        void test(bool& test_variable, const Board& board, Color perspective, double expected_score) const;

    protected:
        //! Returns a structure relating a gene property to a numerical value.
        //
        //! By default, the only property a gene has is "Priority," a multiplicative factor
        //! that controls how influential a gene's score is to the overall evaulation of a
        //! board position. This method is overridden by derived Gene classes to either augment
        //! or replace this data with more specialized properties.
        //! \returns A collection of gene properties with their numerical values.
        virtual std::map<std::string, double> list_properties() const;

        //! Reads a properties data structure and loads the data into itself.
        //
        //! This method is the counterpart to Gene::list_properties() in that it
        //! reads the same data structure as the one produced by Gene::list_properties().
        //! This means that this method is also overridden by derived Genes that have
        //! different properties.
        //! \param properties A data structure with all the data needed for this gene.
        //! \throws std::out_of_range When an expected property is not present in the input.
        virtual void load_properties(const std::map<std::string, double>& properties);

    private:
        double scoring_priority;
        bool priority_is_non_negative;

        virtual double score_board(const Board& board, Color perspective, size_t prior_real_moves) const = 0;
        [[noreturn]] void throw_on_invalid_line(const std::string& line, const std::string& reason) const;

        //! A method overridden by derived genes to mutate more specific gene components.
        //
        //! By default, this method does nothing.
        virtual void gene_specific_mutation();
};

#endif // GENE_H
