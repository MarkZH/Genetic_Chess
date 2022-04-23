#ifndef GENE_H
#define GENE_H

#include <map>
#include <string>
#include <iosfwd>
#include <memory>

#include "Game/Color.h"

#include "Genes/Interpolated_Gene_Value.h"

class Board;
class Piece_Strength_Gene;
class Genome_Creation_Error;

//! \brief The base class of all genes that control the behavior of Genetic Chess players.
class Gene
{
    public:
        virtual ~Gene() = default;

        //! \brief Reads an input stream for gene data.
        //!
        //! Every line should be of the form: \<property\> = \<value\> with optional comments at the end preceded by '#'.
        //! A blank line marks the end of the gene data.
        //! \param is An input stream (std::ifstream, std::iostream, or similar).
        //! \exception Genome_Creation_Error or derivative if there is an invalid line or an unexpected property
        void read_from(std::istream& is);

        //! \brief Read gene data from a text file.
        //!
        //! The method creates an input stream from the text file, finds the first line with
        //! the correct Name: line, and passes it to the other Gene::read_from().
        void read_from(const std::string& file_name);

        //! \brief Applies a random mutation to the priority or other aspect of a gene.
        void mutate() noexcept;

        //! \brief Gives a numerical score to the board in the arguments.
        //!
        //! \param board The state of the board to be evaluated--found at the leaves of the game search tree.
        //! \param perspective For which player the board is being scored.
        //! \param depth The current game tree search depth.
        //! \param game_progress An estimate of the fraction of the game that has been played.
        //! \returns A numerical score indicating the likelihood that the board in the first argument is winning for board.whose_turn().
        double evaluate(const Board& board, Piece_Color perspective, size_t depth, double game_progress) const noexcept;

        //! \brief Copies the gene data and returns a pointer to the new data
        //!
        //! \returns A unique pointer to the copied data.
        virtual std::unique_ptr<Gene> duplicate() const noexcept = 0;

        //! \brief Tells how many mutatable components are present in a gene.
        //!
        //! The more components a gene has, the more likely it will be mutated by a call
        //! to Genome::mutate().
        //! \returns How many individually mutatable components exist in a gene.
        size_t mutatable_components() const noexcept;

        //! \brief Returns the name of the gene.
        //!
        //! \returns A text string with the name of the gene.
        virtual std::string name() const noexcept = 0;

        //! \brief Outputs gene data to a std::ostream in text form.
        //!
        //! \param os An output stream (std::ofstream, std::cout, etc.)
        void print(std::ostream& os) const noexcept;

        //! \brief Gives the gene the new location of the Piece Strength Gene.
        //!
        //! When a Genome is copied or assigned to, the Genes that reference
        //! the Piece_Strength_Gene need to be told the location of the new
        //! copy. For all other genes, this does nothing.
        //! \param psg A pointer to the correct Piece Strength Gene.
        virtual void reset_piece_strength_gene(const Piece_Strength_Gene* psg) noexcept;

        //! \brief Returns the priority of the gene.
        //!
        //! \param stage Which stage of the game to query for the priority.
        double priority(Game_Stage stage) const noexcept;

        //! \brief Returns whether the gene has a Priority component
        bool has_priority() const noexcept;

        //! \brief Scales the priority by multiplying by the parameter.
        //!
        //! \param stage Which stage of the game to query for the priority.
        //! \param k Number to multiply priority by.
        void scale_priority(Game_Stage stage, double k) noexcept;

        //! Tests the board-scoring method of the Gene.
        //
        //! \brief Tests the board-scoring method of the Gene.
        //!
        //! \param[out] test_variable If the test fails, this parameter is set to false. Otherwise, it keeps
        //!        its original value.
        //! \param board The board upon which the test takes place.
        //! \param perspective The player for whom the score is being calculated.
        //! \param expected_score The expected score returned by Gene::score_board().
        void test(bool& test_variable, const Board& board, Piece_Color perspective, double expected_score) const noexcept;

    protected:
        //! \brief When preparing to write gene data to a file, regulatory genes can use this to delete unused Priority data.
        void delete_priorities(std::map<std::string, std::string>& properties) const noexcept;

    private:
        Interpolated_Gene_Value priorities = {"Priority", 1.0, 1.0, 0.005};

        virtual double score_board(const Board& board, Piece_Color perspective, size_t depth) const noexcept = 0;

        template<typename Error = Genome_Creation_Error>
        [[noreturn]] void throw_on_invalid_line(const std::string& line, const std::string& reason) const
        {
            throw Error("Invalid line in while reading for " + name() + ": " + line + "\n" + reason);
        }

        //! \brief A method overridden by derived genes to mutate more specific gene components.
        //!
        //! By default, this method does nothing.
        virtual void gene_specific_mutation() noexcept;

        //! \brief Returns a structure relating a gene property to a numerical value.
        //!
        //! By default, the only property a gene has is "Priority," a multiplicative factor
        //! that controls how influential a gene's score is to the overall evaulation of a
        //! board position. This method is overridden by derived Gene classes to either augment
        //! or replace this data with more specialized properties.
        //! \returns A collection of gene properties with their numerical values.
        std::map<std::string, std::string> list_properties() const noexcept;

        //! \brief Allow Gene subtypes to make changes to the gene properties to be recorded.
        virtual void adjust_properties(std::map<std::string, std::string>& properties) const noexcept;

        //! \brief Reads a properties data structure and loads the data into itself.
        //!
        //! This method is the counterpart to Gene::list_properties() in that it
        //! reads the same data structure as the one produced by Gene::list_properties().
        //! This means that this method is also overridden by derived Genes that have
        //! different properties.
        //! \param properties A data structure with all the data needed for this gene.
        //! \exception std::out_of_range When an expected property is not present in the input.
        void load_properties(const std::map<std::string, std::string>& properties);

        //! \brief Load the properties specific to the Gene subtype.
        virtual void load_gene_properties(const std::map<std::string, std::string>& properties);
};

//! \brief A template class to create the duplicate method for all Gene subtypes.
//!
//! \tparam Gene_Type The real Gene subclass that implements the Gene.
//!
//! Every concrete Gene subclass should be declared as
//! \code{cpp}class Chess_Gene : public Clonable_Gene<Chess_Gene> { ... }; \endcode
template<typename Gene_Type>
class Clonable_Gene : public Gene
{
    public:
        //! \brief The concrete (though templated) implementation of the duplicate() method.
        std::unique_ptr<Gene> duplicate() const noexcept override
        {
            return std::make_unique<Gene_Type>(static_cast<const Gene_Type&>(*this));
        }
};

#endif // GENE_H
