#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <algorithm>

namespace Algorithm
{
    //! Check if range has exactly n elements that satisfy a predicate
    //! 
    //! \param begin Beginning of the range
    //! \param end End of the range
    //! \param predicate The predicate to satisfy
    //! \param n The number of expected satisfactory elements
    template<typename Iter, typename Pred>
    bool has_exactly_n(Iter begin, Iter end, Pred predicate, size_t n) noexcept
    {
        if(n == 0)
        {
            return std::none_of(begin, end, predicate);
        }
        else
        {
            const auto i = std::find_if(begin, end, predicate);
            return i != end && has_exactly_n(std::next(i), end, predicate, n - 1);
        }
    }
}

#endif // ALGORITHM_H
