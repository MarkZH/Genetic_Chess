#ifndef FIXED_CAPACITY_VECTOR_H
#define FIXED_CAPACITY_VECTOR_H

#include <array>
#include <algorithm>
#include <iterator>
#include <cassert>

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
        //! \brief A typedef for use with scoped_push_back
        using const_reference = const T&;

        //! Create an empty Fixed_Capacity_Vector.
        constexpr Fixed_Capacity_Vector() = default;

        //! \brief Add a new item to the end of the Fixed_Capacity_Vector.
        //!
        //! \param new_item The item to be added.
        //! \exception assertion_failure if the Fixed_Capacity_Vector is fulland in DEBUG mode.
        constexpr void push_back(const T& new_item) noexcept
        {
            assert( ! full());
            data[insertion_point++] = new_item;
        }

        //! \brief Inserts a range of values into the vector at the given position.
        //!
        //! \tparam Iterator An iterator type
        //! \param position An iterator to where the first element should be inserted (can be end()).
        //! \param range_begin An iterator to the first element that will be inserted.
        //! \param range_end An iterator to one passed the last element that will be inserted.
        template<typename Iterator>
        constexpr void insert(typename data_store::iterator position,
                              Iterator range_begin,
                              Iterator range_end) noexcept
        {
            const auto move_distance = std::distance(range_begin, range_end);
            assert(size() + move_distance < maximum_size());
            std::move(position, end(), position + move_distance);
            std::copy(range_begin, range_end, position);
            insertion_point += move_distance;
        }

        //! \brief Erase a range of elements and move the elements to the right of the range to join with the remaining elements.
        //!
        //! \param range_begin An iterator to the first element to remove
        //! \param range_end An iterator to one-passed the last element to remove.
        constexpr void erase(typename data_store::iterator range_begin,
                             typename data_store::iterator range_end) noexcept
        {
            assert(size_t(std::distance(range_begin, range_end)) <= size());
            std::copy(range_end, end(), range_begin);
            insertion_point -= std::distance(range_begin, range_end);
        }

        //! \brief Shift all items one place to the left, discarding the first element.
        //! \exception assertion_failure If the vector is empty.
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

        //! \brief Return the maximum size of the Fixed_Capacity_Vector.
        constexpr size_t maximum_size() const noexcept
        {
            return data.size();
        }

        //! \brief Return if the Fixed_Capacity_Vector is full and can take no more push_back()s.
        constexpr bool full() const noexcept
        {
            return size() == maximum_size();
        }

        //! \brief Returns the first item in the Fixed_Capacity_Vector.
        //!
        //! \exception assertion_failure If there is no data.
        constexpr const T& front() const noexcept
        {
            assert( ! empty());
            return data.front();
        }

        //! \brief Returns the first item in the Fixed_Capacity_Vector.
        //!
        //! \exception assertion_failure If there is no data.
        constexpr T& front() noexcept
        {
            assert( ! empty());
            return data.front();
        }

        //! \brief Returns the last item in the Fixed_Capacity_Vector.
        //!
        //! \exception assertion_failure If there is no data.
        constexpr const T& back() const noexcept
        {
            assert( ! empty());
            return data[insertion_point - 1];
        }

        //! \brief Returns the last item in the Fixed_Capacity_Vector.
        //!
        //! \exception assertion_failure If there is no data.
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

        //! \brief An iterator to the beginning of the vector.
        constexpr typename data_store::iterator begin() noexcept
        {
            return data.begin();
        }

        //! \brief An iterator to the end of the vector.
        constexpr typename data_store::iterator end() noexcept
        {
            return data.begin() + size();
        }

    private:
        data_store data;
        size_t insertion_point = 0;
};

#endif // FIXED_CAPACITY_VECTOR_H
