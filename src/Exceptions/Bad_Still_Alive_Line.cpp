#include "Exceptions/Bad_Still_Alive_Line.h"

#include <string>

Bad_Still_Alive_Line::Bad_Still_Alive_Line(size_t line_number, std::string& line) noexcept :
    std::runtime_error("Invalid \"Still Alive\" line (line# " + std::to_string(line_number) + "): " + line)
{
}
