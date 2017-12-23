#include "Game/Square.h"

#include <cmath>
#include <algorithm>

Square::operator bool() const
{
    return file != '\0' && rank != 0;
}

int king_distance(const Square& a, const Square& b)
{
    return std::max(std::abs(a.file - b.file),
                    std::abs(a.rank - b.rank));
}

bool operator==(const Square& a, const Square& b)
{
    return (a.file == b.file) && (a.rank == b.rank);
}
