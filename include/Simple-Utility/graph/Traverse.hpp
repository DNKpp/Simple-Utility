//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_TRAVERSE_HPP
#define SIMPLE_UTILITY_GRAPH_TRAVERSE_HPP

#pragma once

#include "Simple-Utility/Config.hpp"
#include "Simple-Utility/functional/Tuple.hpp"
#include "Simple-Utility/graph/Edge.hpp"
#include "Simple-Utility/graph/Node.hpp"
#include "Simple-Utility/graph/Queue.hpp"
#include "Simple-Utility/graph/Tracker.hpp"
#include "Simple-Utility/graph/View.hpp"

#include <array>
#include <cassert>
#include <concepts>
#include <optional>
#include <ranges>
#include <type_traits>
#include <vector>

namespace sl::graph::concepts
{
	template <typename T, typename Node, typename View, typename Tracker>
	concept explorer = basic_node<Node>
						&& view_for<View, Node>
						&& tracker_for<Tracker, node::vertex_t<Node>>
						&& sl::concepts::unqualified<T>
						&& std::destructible<T>
						&& requires(const T& explorer, const Node& node, const View& view, Tracker& tracker)
						{
							{ std::invoke(explorer, node::vertex(node), tracker) } -> std::convertible_to<Node>;
							{ std::invoke(explorer, view, node, tracker) } -> std::ranges::input_range;
							requires std::convertible_to<
								std::ranges::range_reference_t<std::invoke_result_t<T&, const View&, const Node&, Tracker&>>,
								Node>;
						};

	template <typename T>
	concept traverser = std::destructible<T>
						&& requires(T& traverser)
						{
							typename T::node_type;
							{ !traverser.next() } -> std::convertible_to<bool>;
							{ *traverser.next() } -> std::convertible_to<typename T::node_type>;
						};
}

namespace sl::graph::detail
{
	template <typename Node>
	struct NodeFactory;

	template <concepts::basic_node Node>
	struct NodeFactory<Node>
	{
		[[nodiscard]]
		constexpr Node operator ()(const node::vertex_t<Node>& vertex) const
		{
			return Node{vertex};
		}

		template <concepts::edge_for<Node> Edge>
		[[nodiscard]]
		constexpr Node operator ()([[maybe_unused]] const Node& predecessor, const Edge& edge) const
		{
			if constexpr (concepts::ranked_node<Node>)
			{
				return Node{
					edge::destination(edge),
					node::rank(predecessor) + edge::weight(edge)
				};
			}
			else
			{
				return Node{edge::destination(edge)};
			}
		}
	};

	template <concepts::basic_node Node>
		requires requires { typename decorator::NodeFactory<Node, NodeFactory>::node_type; }
	struct NodeFactory<Node>
		: public decorator::NodeFactory<Node, NodeFactory>
	{
	};

	template <concepts::basic_node Node, typename NodeFactory, typename CollectorStrategy>
	class BasicExplorer
	{
	public:
		[[nodiscard]]
		explicit BasicExplorer() = default;

		[[nodiscard]]
		explicit constexpr BasicExplorer(
			NodeFactory nodeFactory
		) noexcept(std::is_nothrow_move_constructible_v<NodeFactory>)
			: m_NodeFactory{std::move(nodeFactory)}
		{
		}

		template <typename Tracker>
		[[nodiscard]]
		constexpr Node operator ()(const node::vertex_t<Node>& vertex, Tracker& tracker) const
		{
			[[maybe_unused]] const bool result = tracker::set_discovered(tracker, vertex);
			assert(result && "Tracker returned false (already visited) for the origin node.");

			return std::invoke(m_NodeFactory, vertex);
		}

		template <typename View, typename Tracker>
			requires std::convertible_to<
				std::invoke_result_t<
					NodeFactory,
					const Node&,
					view::edge_t<View>>,
				Node>
		[[nodiscard]]
		constexpr auto operator ()(const View& graph, const Node& current, Tracker& tracker) const
		{
			return std::invoke(
				CollectorStrategy{},
				view::edges(graph, current),
				current,
				m_NodeFactory,
				tracker);
		}

	private:
		SL_UTILITY_NO_UNIQUE_ADDRESS NodeFactory m_NodeFactory{};
	};

	struct BufferedCollector
	{
		template <typename Edges, typename Node, typename NodeFactory, typename Tracker>
		constexpr auto operator ()(Edges&& edges, const Node& current, const NodeFactory& nodeFactory, Tracker& tracker)
		{
			std::vector<Node> results{};
			if constexpr (std::ranges::sized_range<decltype(edges)>)
			{
				results.reserve(std::ranges::size(edges));
			}

			for (const auto& edge : edges)
			{
				if (tracker::set_discovered(tracker, edge::destination(edge)))
				{
					results.emplace_back(std::invoke(nodeFactory, current, edge));
				}
			}

			return results;
		}
	};

	template <concepts::basic_node Node, typename NodeFactory>
	using BufferedExplorer = BasicExplorer<Node, NodeFactory, BufferedCollector>;

#ifdef SL_UTILITY_HAS_RANGES_VIEWS

	struct LazyCollector
	{
		template <typename Edges, typename Node, typename NodeFactory, typename Tracker>
		constexpr auto operator ()(Edges&& edges, const Node& current, const NodeFactory& nodeFactory, Tracker& tracker)
		{
			return std::views::all(std::forward<Edges>(edges))
					| std::views::filter([&](const auto& edge) { return tracker::set_discovered(tracker, edge::destination(edge)); })
					| std::views::transform([&](const auto& edge) { return std::invoke(nodeFactory, current, edge); });
		}
	};

	template <concepts::basic_node Node, typename NodeFactory>
	using LazyExplorer = BasicExplorer<Node, NodeFactory, LazyCollector>;

	template <typename Node, typename NodeFactory>
	using default_explorer_t = BufferedExplorer<Node, NodeFactory>;
#else
	template <typename Node, typename NodeFactory>
	using default_explorer_t = BufferedExplorer<Node, NodeFactory>;
#endif

	template <
		concepts::basic_node Node,
		concepts::view_for<Node> View,
		concepts::queue_for<Node> QueueStrategy,
		concepts::tracker_for<node::vertex_t<Node>> TrackingStrategy,
		concepts::explorer<Node, View, TrackingStrategy> ExplorationStrategy = default_explorer_t<Node, NodeFactory<Node>>>
		requires concepts::edge_for<view::edge_t<View>, Node>
	class BasicTraverser
	{
	public:
		using node_type = Node;
		using edge_type = view::edge_t<View>;
		using vertex_type = node::vertex_t<Node>;
		using view_type = View;
		using queue_type = QueueStrategy;
		using tracker_type = TrackingStrategy;

		~BasicTraverser() = default;

		BasicTraverser(const BasicTraverser&) = delete;
		BasicTraverser& operator =(const BasicTraverser&) = delete;
		BasicTraverser(BasicTraverser&&) = default;
		BasicTraverser& operator =(BasicTraverser&&) = default;

		template <
			typename... ViewArgs,
			typename... QueueArgs,
			typename... TrackerArgs,
			typename... ExplorerArgs>
			requires std::constructible_from<view_type, ViewArgs...>
					&& std::constructible_from<queue_type, QueueArgs...>
					&& std::constructible_from<tracker_type, TrackerArgs...>
					&& std::constructible_from<ExplorationStrategy, ExplorerArgs...>
		[[nodiscard]]
		explicit constexpr BasicTraverser(
			const vertex_type& origin,
			std::tuple<ViewArgs...> viewArgs,
			std::tuple<QueueArgs...> queueArgs,
			std::tuple<TrackerArgs...> trackerArgs,
			std::tuple<ExplorerArgs...> explorerArgs
		)
			: m_Explorer{std::make_from_tuple<ExplorationStrategy>(std::move(explorerArgs))},
			m_Queue{std::make_from_tuple<queue_type>(std::move(queueArgs))},
			m_Tracker{std::make_from_tuple<tracker_type>(std::move(trackerArgs))},
			m_View{std::make_from_tuple<view_type>(std::move(viewArgs))}
		{
			assert(queue::empty(m_Queue) && "Queue already contains elements.");

			queue::insert(m_Queue, std::array{std::invoke(m_Explorer, origin, m_Tracker)});
		}

		[[nodiscard]]
		constexpr std::optional<node_type> next()
		{
			const auto queueNext = [&]() -> std::optional<node_type>
			{
				if (!queue::empty(m_Queue))
				{
					return {queue::next(m_Queue)};
				}

				return std::nullopt;
			};

			std::optional result = queueNext();
			for (;
				result && !tracker::set_visited(m_Tracker, node::vertex(*result));
				result = queueNext())
			{
			}

			if (result)
			{
				queue::insert(
					m_Queue,
					std::invoke(m_Explorer, m_View, *result, m_Tracker));
			}

			return result;
		}

		[[nodiscard]]
		constexpr const queue_type& queue() const noexcept
		{
			return m_Queue;
		}

		[[nodiscard]]
		constexpr const tracker_type& tracker() const noexcept
		{
			return m_Tracker;
		}

		[[nodiscard]]
		constexpr const view_type& view() const noexcept
		{
			return m_View;
		}

	private:
		SL_UTILITY_NO_UNIQUE_ADDRESS ExplorationStrategy m_Explorer{};
		queue_type m_Queue;
		tracker_type m_Tracker;
		view_type m_View;
	};
}

namespace sl::graph
{
	template <concepts::traverser Traverser>
	class IterableTraverser
	{
	public:
		using node_type = typename Traverser::node_type;
		using vertex_type = node::vertex_t<node_type>;

		~IterableTraverser() = default;

		IterableTraverser(const IterableTraverser&) = delete;
		IterableTraverser& operator =(const IterableTraverser&) = delete;
		IterableTraverser(IterableTraverser&&) = default;
		IterableTraverser& operator =(IterableTraverser&&) = default;

		template <typename... TraverserArgs>
			requires std::constructible_from<Traverser, TraverserArgs...>
		[[nodiscard]]
		explicit constexpr IterableTraverser(
			TraverserArgs&&... traverserArgs
		) noexcept(std::is_nothrow_constructible_v<Traverser, TraverserArgs...>)
			: m_Traverser{std::forward<TraverserArgs>(traverserArgs)...}
		{
		}

		struct Sentinel final
		{
		};

		struct Iterator final
		{
			friend IterableTraverser;

		public:
			using iterator_concept = std::input_iterator_tag;
			using element_type = node_type;
			using difference_type = std::ptrdiff_t;

			[[nodiscard]]
			constexpr const node_type& operator *() const noexcept
			{
				return *m_Value;
			}

			constexpr Iterator& operator ++()
			{
				m_Value = m_Source->next();
				return *this;
			}

			constexpr void operator ++(int)
			{
				operator++();
			}

			[[nodiscard]]
			constexpr bool operator==([[maybe_unused]] const Sentinel) const noexcept
			{
				return !m_Value;
			}

		private:
			Traverser* m_Source{};
			std::optional<node_type> m_Value{};

			[[nodiscard]]
			constexpr explicit Iterator(Traverser& source)
				: m_Source{std::addressof(source)},
				m_Value{source.next()}
			{
			}
		};

		[[nodiscard]]
		constexpr Iterator begin() &
		{
			return Iterator{m_Traverser};
		}

		void begin() const & = delete;

		[[nodiscard]]
		constexpr Sentinel end() const noexcept
		{
			return Sentinel{};
		}

	private:
		Traverser m_Traverser;
	};

	template <typename Traverser>
		requires concepts::traverser<std::remove_cvref_t<Traverser>>
	IterableTraverser(Traverser&&) -> IterableTraverser<Traverser>;
}

#endif
