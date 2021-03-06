//		  Copyright Dominic Koepke 2019 - 2020.
// Distributed under the Boost Software License, Version 1.0.
//	(See accompanying file LICENSE_1_0.txt or copy at
//		  https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_CONTAINER_VECTOR2D_HPP
#define SL_CONTAINER_VECTOR2D_HPP

#pragma once

#include <concepts>
#include <type_traits>
#include <vector>

#include "Simple-Utility/TypeTraits.hpp"

namespace sl::container::detail
{
	template <class T>
	concept Element = std::semiregular<T>;
}

namespace sl::container
{
	template <detail::Element T, class TContainer = std::vector<T>>
	class Vector2d
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

		constexpr Vector2d() noexcept(noexcept(TContainer())) = default;

		/*ToDo: c++20
		constexpr*/
		~Vector2d() noexcept = default;

		constexpr explicit Vector2d(size_type width, size_type height, const T& value = T{}) :
			m_Width{ width },
			m_Height{ height },
			m_Data(width * height)
		{
		}

		constexpr Vector2d(const Vector2d&) = default;
		constexpr Vector2d& operator =(const Vector2d&) = default;

		constexpr Vector2d(Vector2d&& other) noexcept(std::is_nothrow_move_constructible_v<TContainer>)
		{
			*this = std::move(other);
		}

		constexpr Vector2d& operator =(Vector2d&& other) noexcept(std::is_nothrow_move_assignable_v<TContainer>)
		{
			using std::swap;

			swap(m_Data, other.m_Data);
			swap(m_Width, other.m_Width);
			swap(m_Height, other.m_Height);
			return *this;
		}

		template <class T2 = T>
		requires std::same_as<T, T2> && std::equality_comparable<T2>
		// ToDo: c++20
		[[nodiscard]] /*constexpr*/ bool operator ==(const Vector2d& other) const noexcept(IsNothrowComparable_v<TContainer, TContainer>)
		{
			return m_Width == other.m_Width && m_Height == other.m_Height && m_Data == other.m_Data;
		}

		constexpr void setWidth(size_type width, const T& value = T{})
		{
			m_Data.reserve(width * m_Height);
			setWidthImpl(width, value);
		}

		constexpr void setHeight(size_type height, const T& value = T{})
		{
			m_Data.reserve(m_Width * height);
			setHeightImpl(height, value);
		}

		constexpr void resize(size_type width, size_type height, const T& value = T{})
		{
			m_Data.reserve(width * height);
			resizeImpl(width, height, value);
		}

		[[nodiscard]] constexpr size_type cellCount() const noexcept
		{
			return std::size(m_Data);
		}

		[[nodiscard]] constexpr size_type width() const noexcept
		{
			return m_Width;
		}

		[[nodiscard]] constexpr size_type height() const noexcept
		{
			return m_Height;
		}

		[[nodiscard]] constexpr bool empty() const noexcept
		{
			return m_Width == 0 || m_Height == 0;
		}

		[[nodiscard]] constexpr bool isInRange(size_type x, size_type y) const noexcept
		{
			return x < width() && y < height();
		}

		[[nodiscard]] constexpr const_iterator operator[](size_type column) const noexcept
		{
			auto itr = std::cbegin(m_Data);
			std::advance(itr, cellIndex(column, 0));
			return itr;
		}

		[[nodiscard]] constexpr iterator operator[](size_type column) noexcept
		{
			auto itr = std::begin(m_Data);
			std::advance(itr, cellIndex(column, 0));
			return itr;
		}

		[[nodiscard]] constexpr const_reference at(size_type x, size_type y) const
		{
			return m_Data.at(cellIndex(x, y));
		}

		[[nodiscard]] constexpr reference at(size_type x, size_type y)
		{
			return m_Data.at(cellIndex(x, y));
		}

		constexpr void clear() noexcept
		{
			m_Data.clear();
			m_Width = 0;
			m_Height = 0;
		}

		[[nodiscard]] constexpr T* rawData() noexcept
		{
			return m_Data.data();
		}

		[[nodiscard]] const T* rawData() const noexcept
		{
			return m_Data.data();
		}

		[[nodiscard]] constexpr iterator begin() noexcept
		{
			return std::begin(m_Data);
		}

		[[nodiscard]] constexpr const_iterator begin() const noexcept
		{
			return std::begin(m_Data);
		}

		[[nodiscard]] constexpr const_iterator cbegin() const noexcept
		{
			return std::cbegin(m_Data);
		}

		[[nodiscard]] constexpr reverse_iterator rbegin() noexcept
		{
			return std::rbegin(m_Data);
		}

		[[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept
		{
			return std::rbegin(m_Data);
		}

		[[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept
		{
			return std::crbegin(m_Data);
		}

		[[nodiscard]] constexpr iterator end() noexcept
		{
			return std::end(m_Data);
		}

		[[nodiscard]] constexpr const_iterator end() const noexcept
		{
			return std::end(m_Data);
		}

		[[nodiscard]] constexpr const_iterator cend() const noexcept
		{
			return std::cend(m_Data);
		}

		[[nodiscard]] constexpr reverse_iterator rend() noexcept
		{
			return std::rend(m_Data);
		}

		[[nodiscard]] constexpr const_reverse_iterator rend() const noexcept
		{
			return std::rend(m_Data);
		}

		[[nodiscard]] constexpr const_reverse_iterator crend() const noexcept
		{
			return std::crend(m_Data);
		}

	private:
		size_type m_Width = 0;
		size_type m_Height = 0;
		TContainer m_Data;

		[[nodiscard]] constexpr size_type cellIndex(size_type x, size_type y) const noexcept
		{
			return x * height() + y;
		}

		constexpr void resizeImpl(size_type width, size_type height, const T& value)
		{
			setHeightImpl(height, value);
			setWidthImpl(width, value);
		}

		constexpr void setWidthImpl(size_type width, const T& value)
		{
			m_Data.resize(width * m_Height, value);

			m_Width = width;
		}

		constexpr void setHeightImpl(size_type height, const T& value)
		{
			if (std::empty(m_Data))
			{
				m_Data.resize(m_Width * height, value);
				m_Height = height;
			}
			else if (m_Height < height)
			{
				m_Data.resize(height * m_Width, value);
				const auto diff = height - m_Height;

				auto destItr = std::rbegin(m_Data) + diff;
				auto srcItr = std::rbegin(m_Data) + diff * m_Width;
				auto srcEnd = srcItr + m_Height;

				while (srcEnd != std::rend(m_Data))
				{
					destItr = std::rotate(destItr, srcItr, srcEnd) + diff;
					srcItr += m_Height;
					srcEnd += m_Height;
				}
			}
			else if (height < m_Height)
			{
				const auto diff = m_Height - height;
				auto itr = std::begin(m_Data);
				do
				{
					itr += height;
					itr = m_Data.erase(itr, itr + diff);
				}
				while (itr != std::end(m_Data));
			}

			m_Height = height;
		}
	};
}

#endif
