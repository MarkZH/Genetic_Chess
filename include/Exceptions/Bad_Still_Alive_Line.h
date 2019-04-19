#ifndef BAD_STILL_ALIVE_LINE_H
#define BAD_STILL_ALIVE_LINE_H

#include <stdexcept>
#include <string>

class Bad_Still_Alive_Line : public std::runtime_error
{
    public:
        Bad_Still_Alive_Line(int line_number, std::string& line);
};

#endif // BAD_STILL_ALIVE_LINE_H
