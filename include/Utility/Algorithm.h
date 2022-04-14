#ifndef ALGORITHM_H
#define ALGORITHM_H

#include <algorithm>
#include <cstddef>

//! \brief A collection of algorithms that operate on ranges.
namespace Algorithm
{
    //! Check if range has exactly n elements that satisfy a predicate
    //!
    //! \tparam Iter An iterator type from the container range
    //! \tparam Pred A callable type that returns a bool or equivalent
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

    //! \brief A class for temporarily modifying a container within a scope by pushing elements to the back.
    //!
    //! \tparam Container A container-like object that has push_back methods or can be used with std::insert and std::erase.
    //!
    //! The constucted object is a guard whose destructor
    //! returns the container to its original state. It is
    //! advised not to modify the container during the guard's
    //! lifetime except by other scoped_push_back calls.
    //!
    //! Example of usage:
    //! \code{cpp}
    //! auto data = std::vector<int>{1, 2, 3}; // data == {1, 2, 3}
    //!
    //! {
    //!     const auto push_guard = Algorithm::scoped_push_back(4); // data == {1, 2, 3, 4}
    //! }
    //!
    //! // data == {1, 2, 3}
    //! \endcode
    template<typename Container>
    class [[nodiscard]] scoped_push_back
    {
        public:
            //! \brief Temporarily add a single item.
            //!
            //! \param container The container to be modified.
            //! \param item The item to add to the back of the container.
            scoped_push_back(Container& container, typename Container::const_reference item) : scoped_push_back(container)
            {
                the_container.push_back(item);
            }

            //! \brief Temporarily add a range of items.
            //!
            //! \tparam Iterator An iterator type.
            //! \param container The container to be modified.
            //! \param begin An iterator to the first item to be added.
            //! \param end An iterator just passed the last item to be added.
            template<typename Iterator>
            scoped_push_back(Container& container, Iterator begin, Iterator end) : scoped_push_back(container)
            {
                the_container.insert(the_container.end(), begin, end);
            }

            ~scoped_push_back()
            {
                the_container.erase(the_container.begin() + original_size, the_container.end());
            }

        private:
            Container& the_container;
            size_t original_size;

            scoped_push_back(Container& container) : the_container(container), original_size(container.size())
            {
            }
    };
}

#endif // ALGORITHM_H
