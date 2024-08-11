#ifndef PGN_H

#include <string>
#include <print>

namespace PGN
{
    //! \brief Confirm that all moves in a PGN game record are legal moves.
    //!
    //! \param file_name The name of the file with the PGN game records. All games will be examined.
    bool confirm_game_record(const std::string& file_name);

    //! \brief Format and print a header line for a PGN game.
    //! 
    //! \tparam Output_Stream A type of std::ostream or std::ofstream.
    //! \tparam Data_Type The type of data be written as the value of the header line.
    //! \param output The stream being written to.
    //! \param heading The name of the header parameter.
    //! \param data The value of the header parameter.
    template<typename Output_Stream, typename Data_Type>
    void print_game_header_line(Output_Stream& output, const std::string& heading, const Data_Type& data)
    {
        const auto header_format = "[{} \"{}\"]";
        if constexpr(std::is_same_v<Data_Type, std::string>)
        {
            std::println(output, header_format, heading, data.empty() ? "?" : data);
        }
        else
        {
            std::println(output, header_format, heading, data);
        }
    }
}

#endif // !PGN_H
