//The MIT License(MIT)
//
//Copyright(c) 2019 Dominic Koepke
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files(the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions :
//
//The above copyright notice and this permission notice shall be included in
//all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//THE SOFTWARE.

#ifndef SL_CONTAINER_VECTOR2D_HPP
#define SL_CONTAINER_VECTOR2D_HPP

#pragma once

#include <vector>
#include <cassert>
#include "Simple-Utility/operators/compare.hpp"

namespace sl::container
{
    /*!@defgroup container container
    \brief Useful container classes.
    \details Here are some useful container classes. They are inspired by the STL container, thus you can use them with all STL algorithms.
    @{*/

    /*!\brief 2 dimensional vector.
    *\details This class is a 2 dimensional vector. Internally, it is stored as an std::vector, thus all data is stored in a continuous range and allocation is much faster than
    in a nested construct like std::vector<std::vector<T>>.
    It provides all iterators a vector offers, too, so you can use it with all STL algorithms.
	*/
    template <class T, class TContainer = std::vector<T>>
    class Vector2d :
		operators::Equal<Vector2D<T>>
    {
    public:
    	using value_type = typename TContainer::value_type;
    	using allocator_type = typename TContainer::allocator_type;
		using size_type = typename TContainer::size_type;
    	using difference_type = typename TContainer::difference_type;
        using reference = typename TContainer::reference;
    	using const_reference = typename TContainer::const_reference;
    	using pointer = typename TContainer::pointer;
    	using const_pointer = typename TContainer::const_pointer;
    	using iterator = typename TContainer::iterator;
    	using const_iterator = typename TContainer::const_iterator;
    	using reverse_iterator = typename TContainer::reverse_iterator;
    	using const_reverse_iterator = typename TContainer::const_reverse_iterator;

    private:
        TContainer m_Data;
        size_type m_Width = 0;
    	size_type m_Height = 0;

    public:
        /*!\brief Constructing an empty Vector2D*/
        Vector2d() = default;

		/*!\brief Constructing a Vector2D with specific size
		\param size The initializing size*/
		explicit Vector2d(size_type _width, size_type _height) :
            m_Width{ _width },
    		m_Height{ _height },
			m_Data(_width * _height)
		{
		}
    	

        /*!\brief Access specific element
        \param pos The position which will be returned.
		\attention Does not throw if index is out of range.
        \return Returns a const reference to the value.*/
        const_iterator operator[](size_type _row) const
        {
            return std::begin(m_Data) + _row * width();
        }

        /*!\brief Access specific element
        \param x The x position which will be returned
        \param y The y position which will be returned
		\exception std::out_of_range parameter is out of range
        \return Returns a const reference to the value*/
        const_reference at(size_type _x, size_type _y) const
        {
            return m_Data.at(_y * getWidth() + _x);
        }

        /*!\brief Access specific element
        \param x The x position which will be returned
        \param y The y position which will be returned
		\exception std::out_of_range parameter is out of range
        \return Returns a reference to the value*/
        reference at(size_type _x, size_type _y)
        {
            return m_Data.at(_y * getWidth() + _x);
        }

        /*!\brief modify width
		\details Resize all rows to the given width. If the new width is smaller than the current, all data in row
		 behind the last element will be lost.
        \param width The new width
		\exception Lookup std::vector::resize reference*/
        void set_width(size_type _width)
        {
			assert(_width >= 0);
            if (_width < getWidth() && getHeight() > 0)
            {
                for (std::size_t i = 1; i < getHeight(); ++i)
                {
                    auto itrBegin = m_Data.begin() + getWidth() * i;
                    std::move(itrBegin, itrBegin + _width, m_Data.begin() + _width * i);
                }
                m_Data.resize(_width * getHeight());
            }
            else if (_width > getWidth() && getHeight() > 0)
            {
                auto widthDiff = _width - getWidth();
                auto oldSize = m_Data.size();
                m_Data.resize(_width * getHeight());
                auto sizeDiff = m_Data.size() - oldSize;
                for (std::size_t i = 0; i < getHeight() - 1; ++i)
                {
                    auto itrBegin = m_Data.rbegin() + sizeDiff + getWidth() * i;
                    auto itrEnd = itrBegin + getWidth();
                    auto destItrBegin = m_Data.rbegin() + widthDiff * (i + 1) + getWidth() * i;
                    std::move(itrBegin, itrEnd, destItrBegin);
                    std::fill(std::max(destItrBegin + _width, itrBegin), itrEnd, T());
                }
            }
            m_Width = _width;
        }

        /*!\brief modify height
		\details Resize all columns to the given height. If the new height is smaller than the current, all data in column
		 behind the last element will be lost.
        \param height The new height
		\exception Lookup std::vector::resize reference*/
        void set_height(size_type _height)
        {
			assert(_height >= 0);
            m_Data.resize(getWidth() * _height);
            m_Height = _height;
        }

        /*!\brief modify size
        \param width The new width
        \param height The new height
		\exception Lookup std::vector::resize reference*/
        void resize(size_type _width, size_type _height)
        {
            setWidth(_width);
            setHeight(_height);
        }

		/*!\brief Returns total cell count*/
		size_type cell_count() const
		{
			return m_Data.size();
		}

        /*!\brief Returns the current width*/
        size_type width() const
        {
            return m_Width;
        }

        /*!\brief Returns the current height*/
        size_type height() const
        {
			return m_Height;
        }

        /*!\brief Clears all internal data.*/
        void clear()
        {
            m_Data.clear();
			m_Width = 0;
			m_Height = 0;
        }

		bool isNull() const
		{
			return m_Width == 0 || m_Height == 0;
		}

        /*!\brief Inserts a new row at before the given iterator
		\details The new line must at least equal to the Vector2D width.
        \param insertItr The position at which the new line will be inserted
        \param itrBegin The begin iterator of the new line
		\exception Lookup std::vector::resize reference*/
        template <class TForeignIterator>
        void insert_row(size_type _row, TForeignIterator _itrBegin)
        {
            auto itrEnd = _itrBegin;
            std::advance(itrEnd, width());
            m_Data.insert((*this)[_row], _itrBegin, itrEnd);
            ++m_Height;
        }

        /*!\brief Erase the row at the given index.
        \param row The row index.
		\exception Lookup std::vector::erase reference*/
        void erase_row(size_type _row)
        {
            m_Data.erase((*this)[_row], itr + width());
            --m_Height;
        }
		
		/*!\brief pointer to raw data
		\attention This pointer is only valid as long as you do not resize the Vector2D.
		\return pointer to first element*/
		T* raw_data()
		{
			return m_Data.data();
		}
		
		/*!\brief const pointer to raw data
		*\attention This pointer is only valid as long as you do not resize the Vector2D.
		\return const pointer to first element*/
		const T* raw_data() const
		{
			return m_Data.data();
		}

        /*!\brief Checks if both Vector2D objects are equal
		\details Two Vector2D objects are equal, if all their elements matches.
        \param other The other Vector2D object.
        \return Returns true if both Vector2D objects are equal*/
        friend bool operator ==(const Vector2d& _lhs, const Vector2d& _rhs)
        {
            return _lhs.m_Size == _rhs.m_Size && _lhs.m_Data == _rhs.m_Data;
        }

		auto begin()
		{
			return std::begin(m_Data);
		}

		auto begin() const
		{
			return std::begin(m_Data);
		}

		auto cbegin() const
		{
			return std::cbegin(m_Data);
		}

		auto rbegin()
		{
			return std::rbegin(m_Data);
		}

		auto rbegin() const
		{
			return std::rbegin(m_Data);
		}

		auto crbegin() const
		{
			return std::crbegin(m_Data);
		}

		auto end()
		{
			return std::end(m_Data);
		}

		auto end() const
		{
			return std::end(m_Data);
		}

		auto cend() const
		{
			return std::cend(m_Data);
		}

		auto rend()
		{
			return std::rend(m_Data);
		}

		auto rend() const
		{
			return std::rend(m_Data);
		}

		auto crend() const
		{
			return std::crend(m_Data);
		}
    };
    /*! @}*/
}

#endif // !SL_CONTAINER_VECTOR2D_HPP
