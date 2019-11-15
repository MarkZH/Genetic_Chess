#ifndef BAD_STILL_ALIVE_LINE_H
#define BAD_STILL_ALIVE_LINE_H

#include <stdexcept>
#include <string>

//! An exception that is thrown when a malformed "Still Alive" line is encountered in a gene pool file.
class Bad_Still_Alive_Line : public std::runtime_error
{
    public:
        //! Exception constructor
        //
        //! \param line_number The number of the line in the gene pool file.
        //! \param line The offending line.
        Bad_Still_Alive_Line(size_t line_number, std::string& line) noexcept;
};

#endif // BAD_STILL_ALIVE_LINE_H
