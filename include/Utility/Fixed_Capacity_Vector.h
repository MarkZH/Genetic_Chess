#ifndef FIXED_CAPACITY_VECTOR_H
#define FIXED_CAPACITY_VECTOR_H

#include <array>
#include <vector>
#include <algorithm>
#include <cassert>

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

#endif // FIXED_CAPACITY_VECTOR_H