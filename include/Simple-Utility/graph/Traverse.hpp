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
#include "Simple-Utility/graph/Graph.hpp"
#include "Simple-Utility/graph/Node.hpp"
#include "Simple-Utility/graph/Queue.hpp"
#include "Simple-Utility/graph/Tracker.hpp"

#include <array>
#include <cassert>
#include <concepts>
#include <optional>
#include <ranges>
#include <type_traits>
#include <vector>

namespace sl::graph::concepts
{
	template <typename T, typename Node, typename Graph, typename Tracker>
	concept explorer = basic_node<Node>
						&& basic_graph<Graph>
						&& tracker_for<Tracker, node::vertex_t<Node>>
						&& sl::concepts::unqualified<T>
						&& std::destructible<T>
						&& requires(const T& explorer, const Node& node, const Graph& view, Tracker& tracker)
						{
							{ std::invoke(explorer, node::vertex(node), tracker) } -> std::convertible_to<Node>;
							{ std::invoke(explorer, view, node, tracker) } -> std::ranges::input_range;
							requires std::convertible_to<
								std::ranges::range_reference_t<std::invoke_result_t<T&, const Graph&, const Node&, Tracker&>>,
								Node>;
						};

	template <typename T, typename Graph, typename Explorer, typename Queue, typename Tracker>
	concept traverser_kernel = basic_graph<Graph>
								&& sl::concepts::unqualified<T>
								&& std::destructible<T>
								&& requires(
								T& kernel,
								const Graph& view,
								const Explorer& explorer,
								Queue& queue,
								Tracker& tracker
							)
								{
									{ !std::invoke(kernel, view, explorer, queue, tracker) } -> std::convertible_to<bool>;
									{
										*std::invoke(kernel, view, explorer, queue, tracker)
									} -> std::convertible_to<std::remove_cvref_t<decltype(queue::next(queue))>>;
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
					graph::edge_t<View>>,
				Node>
		[[nodiscard]]
		constexpr auto operator ()(const View& graph, const Node& current, Tracker& tracker) const
		{
			return std::invoke(
				CollectorStrategy{},
				graph::out_edges(graph, node::vertex(current)),
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

	struct PreOrderKernel
	{
		template <typename Graph, typename Explorer, typename Queue, typename Tracker>
		[[nodiscard]]
		constexpr auto operator()(const Graph& graph, const Explorer& explorer, Queue& queue, Tracker& tracker) const
		{
			using node_type = std::remove_cvref_t<decltype(queue::next(queue))>;

			const auto queueNext = [&]() -> std::optional<node_type>
			{
				if (!queue::empty(queue))
				{
					return {queue::next(queue)};
				}

				return std::nullopt;
			};

			std::optional result = queueNext();
			for (;
				result && !tracker::set_visited(tracker, node::vertex(*result));
				result = queueNext())
			{
			}

			if (result)
			{
				queue::insert(
					queue,
					std::invoke(explorer, graph, *result, tracker));
			}

			return result;
		}
	};

	template <
		concepts::basic_node Node,
		concepts::basic_graph Graph,
		concepts::queue_for<Node> QueueStrategy,
		concepts::tracker_for<node::vertex_t<Node>> TrackingStrategy,
		concepts::explorer<Node, Graph, TrackingStrategy> ExplorationStrategy = default_explorer_t<Node, NodeFactory<Node>>,
		concepts::traverser_kernel<Graph, ExplorationStrategy, QueueStrategy, TrackingStrategy> KernelStrategy = PreOrderKernel>
		requires concepts::edge_for<graph::edge_t<Graph>, Node>
	class BasicTraverser
	{
	public:
		using node_type = Node;
		using edge_type = graph::edge_t<Graph>;
		using vertex_type = node::vertex_t<Node>;
		using graph_type = Graph;
		using queue_type = QueueStrategy;
		using tracker_type = TrackingStrategy;

		~BasicTraverser() = default;

		BasicTraverser(const BasicTraverser&) = delete;
		BasicTraverser& operator =(const BasicTraverser&) = delete;
		BasicTraverser(BasicTraverser&&) = default;
		BasicTraverser& operator =(BasicTraverser&&) = default;

		template <
			typename... GraphArgs,
			typename... QueueArgs,
			typename... TrackerArgs,
			typename... ExplorerArgs>
			requires std::constructible_from<graph_type, GraphArgs...>
					&& std::constructible_from<queue_type, QueueArgs...>
					&& std::constructible_from<tracker_type, TrackerArgs...>
					&& std::constructible_from<ExplorationStrategy, ExplorerArgs...>
		[[nodiscard]]
		explicit constexpr BasicTraverser(
			const vertex_type& origin,
			std::tuple<GraphArgs...> graphArgs,
			std::tuple<QueueArgs...> queueArgs,
			std::tuple<TrackerArgs...> trackerArgs,
			std::tuple<ExplorerArgs...> explorerArgs
		)
			: m_Explorer{std::make_from_tuple<ExplorationStrategy>(std::move(explorerArgs))},
			m_Queue{std::make_from_tuple<queue_type>(std::move(queueArgs))},
			m_Tracker{std::make_from_tuple<tracker_type>(std::move(trackerArgs))},
			m_Graph{std::make_from_tuple<graph_type>(std::move(graphArgs))}
		{
			assert(queue::empty(m_Queue) && "Queue already contains elements.");

			queue::insert(m_Queue, std::array{std::invoke(m_Explorer, origin, m_Tracker)});
		}

		[[nodiscard]]
		constexpr std::optional<node_type> next()
		{
			return std::invoke(m_Kernel, m_Graph, m_Explorer, m_Queue, m_Tracker);
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
		constexpr const graph_type& view() const noexcept
		{
			return m_Graph;
		}

	private:
		ExplorationStrategy m_Explorer{};
		KernelStrategy m_Kernel{};
		queue_type m_Queue;
		tracker_type m_Tracker;
		graph_type m_Graph;
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
