#pragma once

#include <vector>
#include <algorithm>
#include <iterator>

namespace sl::container
{
namespace detail
{
	template <class T, bool UniqueElements, class Compare = std::less<>, class Container = std::vector<T>>
	class BaseSortedVector
	{
	public:
		using container_type = Container;
		using value_type = typename Container::value_type;
		using size_type = typename Container::size_type;
		using iterator = typename Container::iterator;
		using reverse_iterator = typename Container::reverse_iterator;
		using const_iterator = typename Container::const_iterator;
		using const_reverse_iterator = typename Container::const_reverse_iterator;

		BaseSortedVector() = default;

		explicit BaseSortedVector(Compare _compare) :
			m_Compare(std::move(_compare))
		{
		}

		explicit BaseSortedVector(Container _container, Compare _compare = Compare()) :
			m_Container(std::move(_container)),
			m_Compare(std::move(_compare))
		{
			std::sort(std::begin(m_Container), std::end(m_Container), m_Compare);
		}

		~BaseSortedVector() = default;

		BaseSortedVector(const BaseSortedVector&) = default;
		BaseSortedVector(BaseSortedVector&&) noexcept = default;

		BaseSortedVector& operator =(const BaseSortedVector&) = default;
		BaseSortedVector& operator =(BaseSortedVector&&) noexcept = default;

		bool empty() const
		{
			return std::empty(m_Container);
		}

		size_type size() const
		{
			return std::size(m_Container);
		}

		void clear()
		{
			m_Container.clear();
		}

		void reserve(size_type _new_cap)
		{
			m_Container.reserve(_new_cap);
		}

		size_type capacity() const noexcept
		{
			return m_Container.capacity();
		}

		void shrink_to_fit()
		{
			return m_Container.shrink_to_fit();
		}

		template <class TValueType>
		std::pair<iterator, bool> insert(TValueType&& _value)
		{
			return _insert(std::forward<TValueType>(_value));
		}

		template <class TIterator>
		void insert(TIterator _itr, const TIterator& _end)
		{
			for (; _itr != _end; ++_itr)
			{
				_insert(*_itr);
			}
		}

		void insert(std::initializer_list<value_type> _ilist)
		{
			insert(std::begin(_ilist), std::end(_ilist));
		}

		template <class TValueType>
		std::pair<iterator, bool> insert_or_assign(TValueType&& _value)
		{
			return _insert_or_assign(std::forward<TValueType>(_value));
		}

		iterator erase(const_iterator _itr)
		{
			return m_Container.erase(_itr);
		}

		iterator erase(const_iterator _first, const_iterator _last)
		{
			return m_Container.erase(_first, _last);
		}

		template <class TComparable, typename = std::enable_if_t<!std::is_convertible_v<TComparable, const_iterator>>>
		iterator erase(const TComparable& _value)
		{
			auto itr = std::lower_bound(std::begin(m_Container), std::end(m_Container), _value, m_Compare);
			if (itr != end())
			{
				return m_Container.erase(itr);
			}
			return end();
		}

		template <class TComparable>
		iterator find(const TComparable& _value)
		{
			return _find(m_Container, m_Compare, _value);
		}

		template <class TComparable>
		const_iterator find(const TComparable& _value) const
		{
			return _find(m_Container, m_Compare, _value);
		}

		template <class TComparable>
		iterator lower_bound(const TComparable& _value)
		{
			return _lower_bound(m_Container, m_Compare, _value);
		}

		template <class TComparable>
		const_iterator lower_bound(const TComparable& _value) const
		{
			return _lower_bound(m_Container, m_Compare, _value);
		}

		template <class TComparable>
		iterator upper_bound(const TComparable& _value)
		{
			return _upper_bound(m_Container, m_Compare, _value);
		}

		template <class TComparable>
		const_iterator upper_bound(const TComparable& _value) const
		{
			return _upper_bound(m_Container, m_Compare, _value);
		}

		template <class TComparable>
		bool contains(const TComparable& _value) const
		{
			return find(_value) != end();
		}

		/*#####
		# multi element stuff
		#####*/
		template <class TComparable, bool Unique = UniqueElements, typename = std::enable_if_t<!Unique>>
		std::pair<iterator, iterator> equal_range(const TComparable& _value)
		{
			return _equal_range(m_Container, m_Compare, _value);
		}

		template <class TComparable, bool Unique = UniqueElements, typename = std::enable_if_t<!Unique>>
		std::pair<const_iterator, const_iterator> equal_range(const TComparable& _value) const
		{
			return _equal_range(m_Container, m_Compare, _value);
		}

		template <class TComparable, bool Unique = UniqueElements, typename = std::enable_if_t<!Unique>>
		iterator find_last(const TComparable& _value)
		{
			return _find_last(m_Container, m_Compare, _value);
		}

		template <class TComparable, bool Unique = UniqueElements, typename = std::enable_if_t<!Unique>>
		const_iterator find_last(const TComparable& _value) const
		{
			return _find_last(m_Container, m_Compare, _value);
		}

		template <class TComparable, bool Unique = UniqueElements, typename = std::enable_if_t<!Unique>>
		iterator erase_all_of(const TComparable& _value)
		{
			auto range = _equal_range(m_Container, m_Compare, _value);
			return m_Container.erase(range.first, range.second);
		}

		template <class TComparable, bool Unique = UniqueElements, typename = std::enable_if_t<!Unique>>
		size_type count(const TComparable& _value) const
		{
			auto range = _equal_range(m_Container, m_Compare, _value);
			return std::distance(range.first, range.second);
		}

		template <bool Unique = UniqueElements, typename = std::enable_if_t<!Unique>>
		void unique()
		{
			m_Container.erase(std::unique(std::begin(m_Container), std::end(m_Container),
										  [&compare = m_Compare](const auto& _lhs, const auto& _rhs)
										  {
											  return !compare(_lhs, _rhs) && !compare(_rhs, _lhs);
										  }), end());
		}

		/*#####
		# comparison stuff
		#####*/
		bool operator ==(const BaseSortedVector& _other) const
		{
			return std::equal(begin(), end(), std::begin(_other), std::end(_other),
							  [&compare = m_Compare](const auto& _elLhs, const auto& _elRhs)
							  {
								  return !compare(_elLhs, _elRhs) && !compare(_elRhs, _elLhs);
							  });
		}

		friend bool operator !=(const BaseSortedVector& _lhs, const BaseSortedVector& _rhs)
		{
			return !(_lhs == _rhs);
		}

		bool operator <(const BaseSortedVector& _other) const
		{
			return std::lexicographical_compare(begin(), end(), std::begin(_other), std::end(_other), m_Compare);
		}

		friend bool operator >=(const BaseSortedVector& _lhs, const BaseSortedVector& _rhs)
		{
			return !(_lhs < _rhs);
		}

		friend bool operator >(const BaseSortedVector& _lhs, const BaseSortedVector& _rhs)
		{
			return _rhs < _lhs;
		}

		friend bool operator <=(const BaseSortedVector& _lhs, const BaseSortedVector& _rhs)
		{
			return !(_lhs > _rhs);
		}

		/*#####
		# Iterator stuff
		#####*/
		iterator begin() noexcept
		{
			return std::begin(m_Container);
		}

		const_iterator begin() const noexcept
		{
			return std::begin(m_Container);
		}

		const_iterator cbegin() const noexcept
		{
			return std::cbegin(m_Container);
		}

		iterator end() noexcept
		{
			return std::end(m_Container);
		}

		const_iterator end() const noexcept
		{
			return std::end(m_Container);
		}

		const_iterator cend() const noexcept
		{
			return std::cend(m_Container);
		}

		iterator rbegin() noexcept
		{
			return std::rbegin(m_Container);
		}

		const_reverse_iterator rbegin() const noexcept
		{
			return std::rbegin(m_Container);
		}

		const_reverse_iterator crbegin() const noexcept
		{
			return std::crbegin(m_Container);
		}

		iterator rend() noexcept
		{
			return std::rend(m_Container);
		}

		const_reverse_iterator rend() const noexcept
		{
			return std::rend(m_Container);
		}

		const_reverse_iterator crend() const noexcept
		{
			return std::crend(m_Container);
		}

	private:
		Container m_Container;

		Compare m_Compare;

		template <class TValueType>
		std::pair<iterator, bool> _insert(TValueType&& _value)
		{
			auto itr = _lower_bound(m_Container, m_Compare, _value);
			if constexpr (UniqueElements)
			{
				if (itr == end() || m_Compare(_value, *itr))
				{
					m_Container.insert(itr, std::forward<TValueType>(_value));
					return {itr, true};
				}
			}
			else
			{
				m_Container.insert(itr, std::forward<TValueType>(_value));
				return {itr, true};
			}
			return {itr, false};
		}

		template <class TValueType>
		std::pair<iterator, bool> _insert_or_assign(TValueType&& _value)
		{
			auto itr = _lower_bound(m_Container, m_Compare, _value);
			if (itr == end() || m_Compare(_value, *itr))
			{
				m_Container.insert(itr, std::forward<TValueType>(_value));
				return {itr, true};
			}
			*itr = std::forward<TValueType>(_value);
			return {itr, false};
		}

		template <class TContainer, class TCompare, class TComparable>
		static auto _find(TContainer&& _container, TCompare&& _compare, const TComparable& _value)
		{
			auto itr = _lower_bound(_container, _compare, _value);
			if (itr != std::end(_container) && !_compare(_value, *itr))
			{
				return itr;
			}
			return std::end(_container);
		}

		template <class TContainer, class TCompare, class TComparable>
		static auto _find_last(TContainer&& _container, TCompare&& _compare, const TComparable& _value)
		{
			auto [begin, end] = _equal_range(_container, _compare, _value);
			auto dist         = std::distance(begin, end);
			if (0 < dist)
			{
				std::advance(begin, dist - 1);
				return begin;
			}
			return std::end(_container);
		}

		template <class TContainer, class TCompare, class TComparable>
		static auto _lower_bound(TContainer&& _container, TCompare&& _compare, const TComparable& _value)
		{
			return std::lower_bound(std::begin(_container), std::end(_container), _value, _compare);
		}

		template <class TContainer, class TCompare, class TComparable>
		static auto _upper_bound(TContainer&& _container, TCompare&& _compare, const TComparable& _value)
		{
			return std::upper_bound(std::begin(_container), std::end(_container), _value, _compare);
		}

		template <class TContainer, class TCompare, class TComparable>
		static auto _equal_range(TContainer&& _container, TCompare&& _compare, const TComparable& _value)
		{
			return std::equal_range(std::begin(_container), std::end(_container), _value, _compare);
		}
	};
}

	template <class T, class Compare = std::less<>, class Container = std::vector<T>>
	using SortedVector = detail::BaseSortedVector<T, true, Compare, Container>;
	template <class T, class Compare = std::less<>, class Container = std::vector<T>>
	using MultiSortedVector = detail::BaseSortedVector<T, false, Compare, Container>;
}
