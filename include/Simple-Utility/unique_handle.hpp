//          Copyright Dominic Koepke 2019 - 2022.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SL_UNIQUE_HANDLE_HPP
#define SL_UNIQUE_HANDLE_HPP

#pragma once

#include <compare>
#include <concepts>
#include <functional>
#include <optional>

#include "Simple-Utility/concepts/stl_extensions.hpp"
#include "Simple-Utility/nullables/base.hpp"

// some of the std::optional interface hasn't been declared constexpr before
#if __cpp_lib_optional >= 202106L
#define SL_UNIQUE_HANDLE_FULL_CONSTEXPR constexpr
#else
#define SL_UNIQUE_HANDLE_FULL_CONSTEXPR
#endif

namespace sl
{
	/*! \defgroup GROUP_UNIQUE_HANDLE unique_handle
	*
	* \brief A helper type, which acts as a nullable resource handle with self-cleanup support.
	*
	* \details The class \ref unique_handle is in fact a wrapper around a ``std::optional``, thus has at least the overhead of that. Additionally it adds two
	* important aspects:
	*		-# it resets its internal value after it got moved.
	*		-# it invokes its delete action every time when the internal value switches its state from initialized to uninitialized.
	*
	* The latter happens when the value is in an initialized state and the \ref unique_handle gets
	*		- moved,
	*		- destructed or
	*		- assigned
	*
	* This is very similar to the behaviour of a ``std::unique_ptr``, hence the name.
	* This behaviour is useful in cases when one has an identifier to a resource, which isn't stored on the heap (thus a ``std::unique_ptr`` is not
	* a good option), and this identifier should have the responsibility as an owner over that resource, but the resource itself is not bound to the
	* lifetime of that identifier. This might sound quite abstract, thus let us visit a simple example.
	*
	* Here some entities are stored in a simple ``std::list``. Imagine this entities are accessible from many places in your program, thus something
	* like this can easily happen.
	*
	* \code{.cpp}
	*	std::list<Entity> entities{};
	*
	*	{
	*		entities.emplace_front();
	*		auto entity_id = entities.begin();
	*
	*		// do some actions with entity and other stuff
	*
	*		// entity should now be erased. Not actually c++-ig, is it?
	*		entities.erase(entity_id);
	*	}
	* \endcode
	*
	* This is clearly no ``memory leak`` but if one forgets to erase the entity, it exists until the list is cleared.
	*	With \ref unique_handle one can do this.
	*
	* \code{.cpp}
	*	struct list_delete_action
	*	{
	*		std::list<Entity>* list{};  // pointer here, because a delete action must be move and copyable
	*
	*		void operator ()(const std::list<Entity>::iterator& itr) { list->erase(itr); }
	*	};
	*	std::list<Entity> entities{};
	*
	*	{
	*		entities.emplace_front();
	*		sl::unique_handle entity_id{ entities.begin(), list_delete_action{ &entities } };
	*
	*		// do some actions with entity and other stuff
	*
	*		// no cleanup necessary
	*	}
	* \endcode
	*
	* Of course, at a first glance this seems quite more verbose, but in the long term nobody has to care about that entity anymore. This is what ``RAII`` is about.
	* Note that ``unique_handles`` also can be stored as a member, then they really begin to shine, because if one would like to bind that entity to the lifetime of
	* an other object that would of course lead to custom move constructor, assignment operator and destructor and explicitly deleted copy. With a
	* \ref unique_handle none of this is necessary (and this is in fact the main reason why I decided to implement this).
	*
	* \note As using lambdas as delete action is usually fine, using capturing lambdas will fail to compile because they are non-copy-assignable. Use a old-school
	* invokable struct as in the example instead.
	*
	* \see https://en.cppreference.com/w/cpp/utility/optional
	* \see https://en.cppreference.com/w/cpp/memory/unique_ptr
	* \see https://en.cppreference.com/w/cpp/language/lambda
	*
	* @{
	*/

	/**
	 * \brief helper type for indicating unique_handles with uninitialized state
	 */
	// ReSharper disable once IdentifierTypo
	struct nullhandle_t
	{ };

	/**
	 * \brief an object of type nullhandle_t
	 */
	// ReSharper disable once IdentifierTypo
	constexpr nullhandle_t nullhandle{};

	/**
	 * \brief exception type which indicates checked access to an uninitialized value
	 */
	using bad_handle_access = std::bad_optional_access;

	/**
	 * \brief default delete action for unique_handle with an empty operator ()
	 */
	struct default_delete_action
	{
		/**
		 * \brief Empty invoke operator
		 */
		constexpr void operator ()(auto&) const noexcept
		{ }
	};

	namespace detail
	{
		template <class T>
		struct value_validator
		{
			static_assert(std::movable<T>, "The value type must be movable.");
			static_assert(concepts::not_same_as<std::remove_cvref_t<T>, nullhandle_t>, "The value type must not be sl::nullhandle_t.");

			using type = T;
		};

		template <class T, class TDeleteAction>
		struct delete_action_validator
		{
			static_assert(std::copyable<TDeleteAction>, "The delete action object must be copyable (capturing lambdas are not).");
			static_assert(std::invocable<TDeleteAction, T&>, "The delete action object must be invokable by T&.");

			using type = TDeleteAction;
		};

		template <class T>
		using type_t = typename T::type;
	}

	/**
	 * \brief Checks whether the given template type is usable as value type for ``unique_handle`` types.
	 */
	template <class T>
	concept value = std::movable<T> && concepts::not_same_as<T, nullhandle_t>;

	/**
	 * \brief Checks whether the given template type is usable as delete action type for ``unique_handle`` types.
	 */
	template <class T, class TValue>
	concept delete_action_for = value<TValue>
							&& std::invocable<T, TValue&>
							&& std::copyable<T>;

	/**
	 * \brief This type models some kind of ``std::optional`` behaviour but resets itself on move operations.
	 * \details For more details and information about related components go to \ref GROUP_UNIQUE_HANDLE "unique_handle group page".
	 * \tparam T The type of the stored value
	 * \tparam TDeleteAction Type of the used delete action
	 */
	template <value T, delete_action_for<T> TDeleteAction = default_delete_action>
	class unique_handle
	{
	private:
		template <value T2, delete_action_for<T2> TOtherDeleteAction>
		friend class unique_handle;

	public:
		/**
		 * \brief Type of the stored value
		 */
		using value_type = T;
		/**
		 * \brief Type of the used delete action
		 */
		using delete_action_type = TDeleteAction;

		/**
		 * \brief Default constructor. The value will be in an uninitialized stated and the delete action gets default constructed.
		 */
		constexpr unique_handle() noexcept = default;

		/**
		 * \brief Destruct. Does invoke the delete action if the value is in an initialized state.
		 */
		constexpr ~unique_handle() noexcept
		{
			invoke_delete_action_if_necessary();
		}

		/**
		 * \brief Move constructor, which relocates the ownership of the value to the target and resets the source. Delete actions will be copied.
		 * \param other The source object which will lose ownership, if it has any.
		 */
		SL_UNIQUE_HANDLE_FULL_CONSTEXPR unique_handle
		(
			unique_handle&& other
		) noexcept(std::is_nothrow_move_constructible_v<T>
					&& std::is_nothrow_copy_constructible_v<TDeleteAction>)
			: m_Value{ std::exchange(other.m_Value, std::nullopt) },
			m_DeleteAction{ other.m_DeleteAction }
		{ }

		/**
		 * \brief Move assignment, which relocates the ownership of the value to the target and resets the source. Delete actions will be copied.
		 * \param other The source object which will lose ownership, if it has any.
		 * \return A reference to the target object
		 */
		SL_UNIQUE_HANDLE_FULL_CONSTEXPR unique_handle& operator =
		(
			unique_handle&& other
		) noexcept(std::is_nothrow_move_constructible_v<T>
					&& std::is_nothrow_move_assignable_v<T>
					&& std::is_nothrow_copy_constructible_v<TDeleteAction>
					&& std::is_nothrow_copy_assignable_v<TDeleteAction>)
		{
			if (this != &other)
			{
				invoke_delete_action_if_necessary();

				m_Value = std::exchange(other.m_Value, std::nullopt);
				m_DeleteAction = other.m_DeleteAction;
			}
			return *this;
		}

		/**
		 * \brief Swaps the target and the source in a more efficient way.
		 * \param other The source object
		 */
		constexpr void swap
		(
			unique_handle& other
		) noexcept(std::is_nothrow_move_constructible_v<T>
					&& std::is_nothrow_swappable_v<T>
					&& std::is_nothrow_move_constructible_v<TDeleteAction>
					&& std::is_nothrow_swappable_v<TDeleteAction>)
		{
			using std::swap;

			swap(m_Value, other.m_Value);
			swap(m_DeleteAction, other.m_DeleteAction);
		}

		/**
		 * \brief Explicitly deleted copy constructor.
		 */
		unique_handle(const unique_handle&) = delete;
		/**
		 * \brief Explicitly deleted copy assignment.
		 * \return nothing
		 */
		unique_handle& operator =(const unique_handle&) = delete;

		/**
		 * \brief Explicitly does not initialize the value. This overload is mainly used for convenience when returning
		 * a \ref nullhandle.
		 * \param deleteAction The provided delete action object
		 */
		constexpr unique_handle(nullhandle_t, const delete_action_type& deleteAction = delete_action_type()) noexcept
			: m_Value{ std::nullopt },
			m_DeleteAction{ deleteAction }
		{ }

		/**
		 * \brief Explicitly resets the value and invokes the delete action if value was initialized.
		 */
		SL_UNIQUE_HANDLE_FULL_CONSTEXPR unique_handle& operator =(nullhandle_t) noexcept
		{
			invoke_delete_action_if_necessary();

			m_Value = std::nullopt;
			return *this;
		}

		/**
		 * \brief Constructor overload for explicitly providing a delete action.
		 * \param deleteAction The provided delete action object
		 */
		constexpr unique_handle(const delete_action_type& deleteAction) noexcept
			: m_Value{ std::nullopt },
			m_DeleteAction{ deleteAction }
		{ }

		/**
		 * \brief Constructor overload for initializing the value.
		 * \tparam T2 Type of the provided value. Must be convertible to ``T``.
		 * \param value Used object to initialize the value
		 * \param deleteAction The provided delete action object
		 */
		template <concepts::initializes<T> T2>
			requires concepts::not_same_as<std::remove_cvref_t<T2>, unique_handle>
					&& concepts::not_same_as<std::remove_cvref_t<T2>, nullhandle_t>
		explicit (!std::convertible_to<T2&&, T>)
		constexpr unique_handle(T2&& value, const delete_action_type& deleteAction = delete_action_type{})
			: m_Value{ std::forward<T2>(value) },
			m_DeleteAction{ deleteAction }
		{ }

		/**
		 * \brief Assignment operator overload for assigning the value.
		 * \tparam T2 Type of the provided value. Must be convertible to ``T``.
		 * \param value Used object to assign the value
		 * \return A reference to this
		 */
		template <concepts::assignable_to<T&> T2>
			requires concepts::not_same_as<std::remove_cvref_t<T2>, unique_handle>
					&& concepts::not_same_as<std::remove_cvref_t<T2>, nullhandle_t>
					&& concepts::initializes<std::remove_cvref_t<T2>, T>
		constexpr unique_handle& operator =(T2&& value)
		{
			invoke_delete_action_if_necessary();

			m_Value = std::forward<T2>(value);
			return *this;
		}

		/**
		 * \brief Constructor overload for directly initializing the value with a set of arguments.
		 * \tparam TArgs Type of the provided arguments.
		 * \param args Used arguments to initialize the value
		 */
		template <class... TArgs>
			requires std::constructible_from<T, TArgs...>
		constexpr explicit unique_handle(std::in_place_t, TArgs&&... args)
			: m_Value{ std::in_place, std::forward<TArgs>(args)... }
		{ }

		/**
		 * \brief Constructor overload for directly initializing the value with a set of arguments and also initializing
		 * the delete action.
		 * \tparam TArgs Type of the provided arguments.
		 * \param deleteAction The provided delete action object
		 * \param args Used arguments to initialize the value
		 */
		template <class... TArgs>
			requires std::constructible_from<T, TArgs...>
		constexpr explicit unique_handle(std::in_place_t, const delete_action_type& deleteAction, TArgs&&... args)
			: m_Value{ std::in_place, std::forward<TArgs>(args)... },
			m_DeleteAction{ deleteAction }
		{ }

		/**
		 * \brief Constructor overload for directly initializing the value with a set of arguments.
		 * \tparam TArgs Type of the provided arguments.
		 * \param args Used arguments to initialize the value
		 */
		template <class... TArgs>
			requires std::constructible_from<T, TArgs...>
		SL_UNIQUE_HANDLE_FULL_CONSTEXPR void emplace(TArgs&&... args)
		{
			m_Value.emplace(std::forward<TArgs>(args)...);
		}

		/**
		 * \brief Resets the value and invokes the delete action if value was initialized.
		 */
		SL_UNIQUE_HANDLE_FULL_CONSTEXPR void reset() noexcept
		{
			invoke_delete_action_if_necessary();

			m_Value.reset();
		}

		/**
		 * \brief Immutable access to the value. No checks will be performed.
		 * \exception Throws bad_handle_access if value is uninitialized.
		 * \return A const reference to the value
		 */
		[[nodiscard]]
		constexpr const T& raw() const { return m_Value.value(); }

		/**
		 * \brief Immutable access to the value. No checks will be performed.
		 * \return A const reference to the value
		 */
		[[nodiscard]]
		constexpr const T& operator *() const noexcept { return *m_Value; }

		/**
		 * \brief Immutable access to the value. No checks will be performed.
		 * \return A const pointer to the value
		 */
		[[nodiscard]]
		constexpr const T* operator ->() const noexcept { return &*m_Value; }

		/**
		 * \brief Checks whether the value is initialized.
		 * \return True if value is initialized
		 */
		[[nodiscard]]
		constexpr explicit operator bool() const noexcept { return m_Value.has_value(); }

		/**
		 * \brief Checks whether the value is initialized.
		 * \return True if value is initialized
		 */
		[[nodiscard]]
		constexpr bool is_valid() const noexcept { return m_Value.has_value(); }

		/**
		 * \brief Immutable access to the delete action.
		 * \return A const reference to the delete action
		 */
		[[nodiscard]]
		constexpr const delete_action_type& delete_action() const noexcept { return m_DeleteAction; }

		/**
		 * \brief Equality-comparison operator overload between two \ref unique_handle "unique_handles". operator != is implicitly defined.
		 * \tparam T2 Other value type.
		 * \tparam TOtherDeleteAction Other delete action type.
		 * \param other The ``unique_handle`` to compare with
		 * \return Both sides compare equal if both handles contain uninitialized values or both value are initialized and compare equal.
		 * Otherwise ``false`` is returned.
		 */
		template <std::equality_comparable_with<T> T2, class TOtherDeleteAction>
		[[nodiscard]]
		constexpr bool operator ==(const unique_handle<T2, TOtherDeleteAction>& other) const
		{
			return m_Value == other.m_Value;
		}

		/**
		 * \brief Three-way-comparison operator overload between two \ref unique_handle "unique_handles".
		 * \tparam T2 Other value type.
		 * \tparam TOtherDeleteAction Other delete action type.
		 * \param other The object to compare with
		 * \return If both handles have initialized values, both values will be compared. Otherwise they will be compared in accordance
		 * to the result of ``is_valid()``.
		 */
		template <std::three_way_comparable_with<T> T2, class TOtherDeleteAction>
		[[nodiscard]]
		constexpr std::compare_three_way_result_t<T, T2> operator <=>(const unique_handle<T2, TOtherDeleteAction>& other) const
		{
			return m_Value <=> other.m_Value;
		}

		/**
		 * \brief Three-way-comparison operator overload for comparison between a \ref unique_handle and a value.
		 * \tparam T2 Type of right-hand-side. Must be three-way-comparable to ``T``
		 * \param other The object to compare with
		 * \return If the handle's value is initialized, both values will be compared. Otherwise handle is less.
		 */
		template <class T2>
			requires concepts::not_same_as<std::remove_cvref_t<T2>, unique_handle>
					// do not move into template arg list, as this will result in a template cycle
					&& std::three_way_comparable_with<T, T2>
		[[nodiscard]]
		constexpr std::compare_three_way_result_t<T, T2> operator <=>(const T2& other) const
		{
			return m_Value <=> other;
		}

		/**
		 * \brief Equality-comparison operator overload for comparison between a \ref unique_handle and a value. operator != is implicitly defined.
		 * \tparam T2 Type of right-hand-side. Must be equality-comparable to ``T``
		 * \param other The object to compare with
		 * \return Returns ``true`` if handle is valid and value compares equal equal to rhs. Otherwise ``false``.
		 */
		template <class T2>
			requires concepts::not_same_as<std::remove_cvref_t<T2>, unique_handle>
					// do not move into template arg list, as this will result in a template cycle
					&& std::equality_comparable_with<T, T2>
		[[nodiscard]]
		constexpr bool operator ==(const T2& other) const
		{
			return m_Value == other;
		}

		/**
		 * \brief Equality-comparison operator overload for comparison of \ref unique_handle and \ref nullhandle_t.
		 * \return Returns true if handle's value is uninitialized.
		 */
		[[nodiscard]]
		constexpr bool operator ==(nullhandle_t) const noexcept
		{
			return !is_valid();
		}

		/**
		 * \brief Three-way-comparison operator overload for comparison of \ref unique_handle and \ref nullhandle_t.
		 * \return Returns ``std::strong_ordering::equal`` if handle's value is uninitialized, otherwise ``std::strong_ordering::greater``.
		 */
		[[nodiscard]]
		constexpr std::strong_ordering operator <=>(nullhandle_t) const noexcept
		{
			return is_valid() <=> false;
		}

	private:
		std::optional<T> m_Value{};

		[[no_unique_address]]
		TDeleteAction m_DeleteAction{};

		constexpr void invoke_delete_action_if_necessary() noexcept
		{
			if (m_Value)
				std::invoke(m_DeleteAction, *m_Value);
		}
	};

	/**
	 * \brief Deduction guide for \ref unique_handle class
	 * \relatesalso unique_handle
	 * \tparam T Value type
	 * \tparam TDeleteAction Delete action type
	 */
	template <class T, class TDeleteAction>
	unique_handle
	(
		T,
		TDeleteAction
	) -> unique_handle<detail::type_t<detail::value_validator<T>>, detail::type_t<detail::delete_action_validator<T, TDeleteAction>>>;

	/**
	 * \brief Deduction guide for \ref unique_handle class
	 * \relatesalso unique_handle
	 * \tparam T Value type
	 */
	template <class T>
	unique_handle(T) -> unique_handle<T>;

	/**
	 * \brief Specialization for \ref sl::unique_handle "unique_handle" types.
	 * \ingroup GROUP_NULLABLES_TRAITS
	 * \tparam T Value type.
	 * \tparam TDeleteAction Delete action type.
	 */
	template <class T, class TDeleteAction>
	struct nullables::traits<unique_handle<T, TDeleteAction>>
	{
		using value_type = typename unique_handle<T, TDeleteAction>::value_type;
		static constexpr nullhandle_t null{ nullhandle };
	};

	/** @} */
}

#endif
