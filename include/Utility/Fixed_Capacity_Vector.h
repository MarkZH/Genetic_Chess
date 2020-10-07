#ifndef FIXED_CAPACITY_VECTOR_H
#define FIXED_CAPACITY_VECTOR_H

#include <array>
#include <algorithm>
#include <iterator>
#include <cassert>

template<typename T, size_t capacity>
class Scoped_Push_Guard;

//! \brief A std::vector - like container with variable size but no heap allocation.
//!
//! \tparam T The type of data to be stored.
//! \tparam capacity The maximum size of the container.
//!
//! Under the hood, the container's data is stored as a fixed-size std::array<T, capacity>.
//! As such, the data type T must have a default constructor to populate the unused portions
//! of the array.
template<typename T, size_t capacity>
class Fixed_Capacity_Vector
{
    private:
        using data_store = std::array<T, capacity>;

    public:
        //! Create an empty Fixed_Capacity_Vector.
        constexpr Fixed_Capacity_Vector() = default;

        //! \brief Add a new item to the end of the Fixed_Capacity_Vector.
        //!
        //! \param new_item The item to be added.
        //! \throws assertion_failure if the Fixed_Capacity_Vector is fulland in DEBUG mode.
        constexpr void push_back(const T& new_item) noexcept
        {
            assert( ! full());
            data[insertion_point++] = new_item;
        }

        //! \brief Add a new item to the end of the Fixed_Capacity_Vector and remove it when leaving the current scope.
        //!
        //! \param new_item The data to be added.
        //! \returns A Scoped_Push_Guard object that will remove the item in its destructor.
        //! \throws assertion_failure if the Fixed_Capacity_Vector is full and in DEBUG mode.
        constexpr auto scoped_push_back(const T& new_item) noexcept
        {
            assert( ! full());
            return Scoped_Push_Guard(*this, new_item);
        }

        //! \brief Push back a range of values that will be removed when the current scope ends.
        //! \tparam Iterator An iterator type ranging over values of type T.
        //! \param begin The first value to add.
        //! \param end One past the last value to add.
        //! \returns A Scoped_Push_Guard object that will remove the items upon exiting the current scope.
        //! \throws assertion_failure If there is not enough room for the items to be added.
        template<typename Iterator>
        constexpr auto scoped_push_back(Iterator begin, Iterator end) noexcept
        {
            assert(size() + std::distance(begin, end) <= data.size());
            return Scoped_Push_Guard(*this, begin, end);
        }

        //! \brief Remove the last n items.
        //!
        //! \param n The number of items to remove from the end of the vector.
        //! \throws assertion_failure If n is greater than the current size of the vector.
        constexpr void pop_back(size_t n = 1) noexcept
        {
            assert(size() >= n);
            insertion_point -= n;
        }

        //! \brief Shift all items one place to the left, discarding the first element.
        //! \throws assertion_failure If the vector is empty.
        constexpr void shift_left() noexcept
        {
            assert( ! empty());
            std::move(std::next(data.begin()), data.end(), data.begin());
            --insertion_point;
        }

        //! \brief Remove all data from the store.
        //!
        //! Similarly to pop_back(), no destructors are called until the entire Fixed_Capacity_Vector is destructed.
        constexpr void clear() noexcept
        {
            insertion_point = 0;
        }

        //! \brief Return if Fixed_Capacity_Vector is empty.
        constexpr bool empty() const noexcept
        {
            return size() == 0;
        }

        //! \brief Return the current size of the valid data of the Fixed_Capacity_Vector.
        constexpr size_t size() const noexcept
        {
            return insertion_point;
        }

        //! \brief Return if the Fixed_Capacity_Vector is full and can take no more push_back()s.
        constexpr bool full() const noexcept
        {
            return size() == data.size();
        }

        //! \brief Returns the first item in the Fixed_Capacity_Vector.
        //!
        //! \throws assertion_failure If there is no data.
        constexpr const T& front() const noexcept
        {
            assert( ! empty());
            return data.front();
        }

        //! \brief Returns the first item in the Fixed_Capacity_Vector.
        //!
        //! \throws assertion_failure If there is no data.
        constexpr T& front() noexcept
        {
            assert( ! empty());
            return data.front();
        }

        //! \brief Returns the last item in the Fixed_Capacity_Vector.
        //!
        //! \throws assertion_failure If there is no data.
        constexpr const T& back() const noexcept
        {
            assert( ! empty());
            return data[insertion_point - 1];
        }

        //! \brief Returns the last item in the Fixed_Capacity_Vector.
        //!
        //! \throws assertion_failure If there is no data.
        constexpr T& back() noexcept
        {
            assert( ! empty());
            return data[insertion_point - 1];
        }

        //! \brief An iterator to the beginning of the vector.
        constexpr typename data_store::const_iterator begin() const noexcept
        {
            return data.begin();
        }

        //! \brief An iterator to the end of the vector.
        constexpr typename data_store::const_iterator end() const noexcept
        {
            return data.begin() + size();
        }

    private:
        data_store data;
        size_t insertion_point = 0;
};


//! \brief A guard class for automatic management of the contents of a Fixed_Capacty_Vector based on scope.
//!
//! \tparam T The type of data used by the originating Fixed_Capacity_Vector.
//! \tparam capacity The capacity of the originating FIxed_Capacity_Vector.
//!
//! Upon construction, a new value is added to the end of the Fixed_Capacity_Vector.
//! Upon destruction, the value at the end is remvoed.
//!
//! No checks are performed to make sure that the item removed by the destructor is the same item that
//! was added in the destructor. The user is encouraged to only use Fixed_Capacity_Vector::scoped_push_back()
//! inside a scope where that method has been used once (include function calls in the same scope that take
//! the Fixed_Capacity_Vector by reference). As an example of faulty usage, consider the following:
//! \code{cpp}
//! auto data = Fixed_Capacity_Vector<int, 100>{}; // insertion index == 0
//! {
//!     auto push_guard = data.scoped_push_back(1); // insertion index == 1
//!     data.clear(); // insertion index == 0
//! } // push_guard destructor calls data.pop_back(); insertion index == -1 or assert() failure in debug mode.
//! data.push_back(2); // The value is written to index size_t{-1}; probably results in a crash with a segfault.
//! \endcode
template<typename T, size_t capacity>
class [[nodiscard]] Scoped_Push_Guard
{
    public:
        //! \brief Removes the last item (presumably the one added by the constructor) from the vector.
        ~Scoped_Push_Guard() noexcept
        {
            data_store.pop_back(items_added);
        }

        Scoped_Push_Guard(const Scoped_Push_Guard&) = delete;
        Scoped_Push_Guard& operator=(const Scoped_Push_Guard&) = delete;

    private:
        Fixed_Capacity_Vector<T, capacity>& data_store;
        size_t items_added;

        //! \brief Create an instance that adds the new item to the end of the given Fixed_Capacity_Vector.
        //!
        //! \param vec The vector to modify.
        //! \param new_value The item to add to the end of the vector.
        constexpr Scoped_Push_Guard(Fixed_Capacity_Vector<T, capacity>& vec, const T& new_value) noexcept : data_store(vec), items_added(1)
        {
            data_store.push_back(new_value);
        }

        //! \brief Create an instance that adds a number of items to the given Fixed_Capacity_Vector through container iterators.
        //!
        //! \tparam Iterator An iterator type that dereferences to a type T.
        //! \param vec The vector to modify.
        //! \param begin The iterator to the first value to add.
        //! \param end The iterator marking the end of the range (one past the last item).
        template<typename Iterator>
        constexpr Scoped_Push_Guard(Fixed_Capacity_Vector<T, capacity>& vec, Iterator begin, Iterator end) noexcept : data_store(vec), items_added(0)
        {
            for(auto i = begin; i != end; ++i)
            {
                data_store.push_back(*i);
                ++items_added;
            }
        }

        friend class Fixed_Capacity_Vector<T, capacity>;
};

#endif // FIXED_CAPACITY_VECTOR_H
