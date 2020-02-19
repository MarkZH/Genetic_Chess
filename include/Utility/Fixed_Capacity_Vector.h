#ifndef FIXED_CAPACITY_VECTOR_H
#define FIXED_CAPACITY_VECTOR_H

#include <array>
#include <vector>
#include <algorithm>
#include <cassert>

template<typename T, size_t capacity>
class Scoped_Push_Guard;

//! A std::vector - like container with variable size but no heap allocation.
//
//! \tparam T The type of data to be stored.
//! \tparam capacity The maximum size of the container.
//!
//! Under the hood, the container's data is stored as a fixed-size std::array<T, capacity>
//! with an index to where the next piece of data should be inserted. As such, the
//! data type T must have a default constructor.
template<typename T, size_t capacity>
class Fixed_Capacity_Vector
{
    public:
        //! Add a new item to the end of the Fixed_Capacity_Vector.
        //
        //! \param new_item The item to be added.
        //! \throws assertion_failure if the Fixed_Capacity_Vector is fulland in DEBUG mode.
        void push_back(const T& new_item) noexcept
        {
            assert( ! full());
            data[insertion_point++] = new_item;
        }

        //! Add a new item to the end of the Fixed_Capacity_Vector and remove it when leaving the current scope.
        //
        //! \param new_item The data to be added.
        //! \returns A Scoped_Push_Guard object that will remove the item in its destructor.
        //! \throws assertion_failure if the Fixed_Capacity_Vector is full and in DEBUG mode.
        auto scoped_push_back(const T& new_item) noexcept
        {
            assert( ! full());
            return Scoped_Push_Guard(*this, new_item);
        }

        //! Remove the last item.
        //
        //! \throws assertion_failure if the Fixed_Capacity_Vector is emptyand in DEBUG mode.
        //!
        //! No destructors are called until the entire Fixed_Capacity_Vector is destructed.
        void pop_back() noexcept
        {
            assert( ! empty());
            --insertion_point;
        }

        //! Remove all data from the store.
        //
        //! Similarly to pop_back(), no destructors are called until the entire Fixed_Capacity_Vector is destructed.
        void clear() noexcept
        {
            insertion_point = 0;
        }

        //! Count the number of copies of a given item the Fixed_Capacity_Vector contains.
        //
        //! \param item The item to count.
        constexpr size_t count(const T& item) const noexcept
        {
            return std::count(cbegin(), cend(), item);
        }

        //! Return if Fixed_Capacity_Vector is empty.
        constexpr bool empty() const noexcept
        {
            return size() == 0;
        }

        //! Return the current size of the valid data of the Fixed_Capacity_Vector.
        constexpr size_t size() const noexcept
        {
            return insertion_point;
        }

        //! Return if the Fixed_Capacity_Vector is full and can take no more push_back()s.
        constexpr bool full() const noexcept
        {
            return size() == data.size();
        }

        //! Create a std::vector<T> containing just the valid data.
        constexpr std::vector<T> to_vector() const noexcept
        {
            return {cbegin(), cend()};
        }

        //! Returns the first item in the Fixed_Capacity_Vector.
        //
        //! \throws assertion_failure If there is no data.
        constexpr T front() const noexcept
        {
            assert( ! empty());
            return data.front();
        }

    private:
        using data_store = std::array<T, capacity>;
        data_store data;
        size_t insertion_point = 0;

        constexpr typename data_store::const_iterator cbegin() const noexcept
        {
            return data.cbegin();
        }

        constexpr typename data_store::const_iterator cend() const noexcept
        {
            return data.cbegin() + size();
        }
};


//! A guard class for automatic management of the contents of a Fixed_Capacty_Vector based on scope.
//
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
        //! Create an instance that adds the new item to the end of the given Fixed_Capacity_Vector.
        //
        //! \param vec The vector to modify.
        //! \param new_value The item to add to the end of the vector.
        [[nodiscard]] Scoped_Push_Guard(Fixed_Capacity_Vector<T, capacity>& vec, const T& new_value) noexcept : data_store(vec)
        {
            data_store.push_back(new_value);
        }

        //! Removes the last item (presumably the one added by the constructor) from the vector.
        ~Scoped_Push_Guard() noexcept
        {
            data_store.pop_back();
        }

    private:
        Fixed_Capacity_Vector<T, capacity>& data_store;
};

#endif // FIXED_CAPACITY_VECTOR_H
