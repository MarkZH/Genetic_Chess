#include "Exceptions/Bad_Still_Alive_Line.h"

#include <string>

//! Exception constructor

//! \param line_number The number of the line in the gene pool file.
//! \param line The offending line.
Bad_Still_Alive_Line::Bad_Still_Alive_Line(int line_number, std::string& line) :
    std::runtime_error("Invalid \"Still Alive\" line (line# " + std::to_string(line_number) + "): " + line)
{
}
