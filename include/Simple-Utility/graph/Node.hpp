//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#ifndef SIMPLE_UTILITY_GRAPH_NODE_HPP
#define SIMPLE_UTILITY_GRAPH_NODE_HPP

#pragma once

#include "Simple-Utility/Config.hpp"
#include "Simple-Utility/Utility.hpp"
#include "Simple-Utility/concepts/stl_extensions.hpp"
#include "Simple-Utility/graph/Common.hpp"
#include "Simple-Utility/graph/Edge.hpp"

#include <functional>
#include <optional>

namespace sl::graph::customize
{
	template <class>
	struct vertex_fn;

	template <class>
	struct rank_fn;
}

namespace sl::graph::detail
{
	template <class Node>
		requires requires(const Node& node, customize::vertex_fn<Node> fn)
		{
			requires concepts::vertex<std::remove_cvref_t<decltype(fn(node))>>;
		}
	constexpr decltype(auto) vertex(const Node& node, const priority_tag<3>) noexcept(noexcept(customize::vertex_fn<Node>{}(node)))
	{
		return customize::vertex_fn<Node>{}(node);
	}

	template <class Node>
		requires requires
		{
			requires concepts::vertex<std::remove_cvref_t<decltype(std::declval<const Node&>().vertex)>>;
		}
	constexpr auto& vertex(const Node& node, const priority_tag<2>) noexcept
	{
		return node.vertex;
	}

	template <class Node>
		requires requires(const Node& node)
		{
			requires concepts::vertex<std::remove_cvref_t<decltype(node.vertex())>>;
		}
	constexpr decltype(auto) vertex(const Node& node, const priority_tag<1>) noexcept(noexcept(node.vertex()))
	{
		return node.vertex();
	}

	template <class Node>
		requires requires(const Node& node)
		{
			requires concepts::vertex<std::remove_cvref_t<decltype(vertex(node))>>;
		}
	constexpr decltype(auto) vertex(const Node& node, const priority_tag<0>) noexcept(noexcept(vertex(node)))
	{
		return vertex(node);
	}

	struct vertex_fn
	{
		template <class Node>
			requires requires(const Node& node, const priority_tag<3> tag)
			{
				requires concepts::vertex<std::remove_cvref_t<decltype(detail::vertex(node, tag))>>;
			}
		constexpr decltype(auto) operator ()(const Node& node) const noexcept(noexcept(detail::vertex(node, priority_tag<3>{})))
		{
			return detail::vertex(node, priority_tag<3>{});
		}
	};

	template <class Node>
		requires requires(const Node& node, customize::rank_fn<Node> fn)
		{
			requires concepts::rank<std::remove_cvref_t<decltype(fn(node))>>;
		}
	constexpr decltype(auto) rank(const Node& node, const priority_tag<3>) noexcept(noexcept(customize::rank_fn<Node>{}(node)))
	{
		return customize::rank_fn<Node>{}(node);
	}

	template <class Node>
		requires requires(const Node& node)
		{
			requires concepts::rank<std::remove_cvref_t<decltype(node.rank)>>;
		}
	constexpr auto& rank(const Node& node, const priority_tag<2>) noexcept
	{
		return node.rank;
	}

	template <class Node>
		requires requires(const Node& node)
		{
			requires concepts::rank<std::remove_cvref_t<decltype(node.rank())>>;
		}
	constexpr decltype(auto) rank(const Node& node, const priority_tag<1>) noexcept(noexcept(node.rank()))
	{
		return node.rank();
	}

	template <class Node>
		requires requires(const Node& node)
		{
			requires concepts::rank<std::remove_cvref_t<decltype(rank(node))>>;
		}
	constexpr decltype(auto) rank(const Node& node, const priority_tag<0>) noexcept(noexcept(rank(node)))
	{
		return rank(node);
	}

	struct rank_fn
	{
		template <class Node>
			requires requires(const Node& node, const priority_tag<3> tag)
			{
				requires concepts::rank<std::remove_cvref_t<decltype(detail::rank(node, tag))>>;
			}
		constexpr decltype(auto) operator ()(const Node& node) const noexcept(noexcept(detail::rank(node, priority_tag<3>{})))
		{
			return detail::rank(node, priority_tag<3>{});
		}
	};
}

namespace sl::graph::node
{
	inline constexpr detail::vertex_fn vertex{};
	inline constexpr detail::rank_fn rank{};

	template <class>
	struct traits;

	template <typename Node>
	using vertex_t = typename traits<Node>::vertex_type;

	template <typename Node>
	using rank_t = typename traits<Node>::rank_type;

	template <class T>
		requires concepts::readable_vertex_type<T>
	struct traits<T>
	{
		using vertex_type = typename T::vertex_type;
	};

	template <class T>
		requires concepts::readable_vertex_type<T>
				&& concepts::readable_rank_type<T>
	struct traits<T>
	{
		using vertex_type = typename T::vertex_type;
		using rank_type = typename T::rank_type;
	};
}

namespace sl::graph::concepts
{
	template <class T>
	concept basic_node = sl::concepts::unqualified<T>
						&& std::copyable<T>
						&& std::destructible<T>
						&& requires { typename node::traits<T>::vertex_type; }
						&& vertex<node::vertex_t<T>>
						&& requires(const T& node)
						{
							// ReSharper disable once CppRedundantTemplateKeyword
							// ReSharper disable once CppRedundantTypenameKeyword
							{ node::vertex(node) } -> std::convertible_to<typename node::template vertex_t<T>>; // pleases msvc v142
						};

	template <class Node>
	concept ranked_node = basic_node<Node>
						&& requires { typename node::traits<Node>::rank_type; }
						&& rank<node::rank_t<Node>>
						&& requires(const Node& node)
						{
							// ReSharper disable once CppRedundantTemplateKeyword
							// ReSharper disable once CppRedundantTypenameKeyword
							{ node::rank(node) } -> std::convertible_to<typename node::template rank_t<Node>>; // pleases msvc v142
						};

	template <class Edge, class Node>
	concept edge_for = basic_node<Node>
						&& edge<Edge>
						&& std::same_as<edge::vertex_t<Edge>, node::vertex_t<Node>>
						&& (!ranked_node<Node>
							|| requires(const Edge& edge)
							{
								requires weighted_edge<Edge>;
								{ edge::weight(edge) } -> std::convertible_to<node::rank_t<Node>>;
							});
}

namespace sl::graph
{
	template <concepts::vertex Vertex>
	struct CommonBasicNode
	{
		using vertex_type = Vertex;

		vertex_type vertex;

		[[nodiscard]]
		friend bool operator==(const CommonBasicNode&, const CommonBasicNode&) = default;
	};

	template <concepts::vertex Vertex, concepts::rank Rank>
	struct CommonRankedNode
	{
		using vertex_type = Vertex;
		using rank_type = Rank;

		vertex_type vertex;
		rank_type rank;

		[[nodiscard]]
		friend bool operator ==(const CommonRankedNode&, const CommonRankedNode&) = default;
	};
}

namespace sl::graph::decorator
{
	template <concepts::basic_node Node>
	struct PredecessorNode
		: public Node
	{
		using vertex_type = node::vertex_t<Node>;

		std::optional<vertex_type> predecessor{};

		[[nodiscard]]
		friend bool operator ==(const PredecessorNode&, const PredecessorNode&) = default;
	};

	template <concepts::basic_node Node, template<class> typename BaseNodeFactory>
	class NodeFactory;

	template <concepts::basic_node Node, template<class> typename BaseNodeFactory>
	class NodeFactory<PredecessorNode<Node>, BaseNodeFactory>
		: private BaseNodeFactory<Node>
	{
	private:
		using Super = BaseNodeFactory<Node>;

	public:
		using node_type = PredecessorNode<Node>;
		using vertex_type = node::vertex_t<node_type>;

		template <typename... Args>
		[[nodiscard]]
		constexpr node_type operator ()(vertex_type origin, Args&&... args) const
		{
			// leave code as-is, because directly returning the temporary results in an ICE on gcc10
			node_type node{
				{std::invoke(static_cast<const Super&>(*this), std::move(origin), std::forward<Args>(args)...)},
				std::nullopt
			};
			return node;
		}

		template <concepts::edge_for<node_type> Edge, typename... Args>
		[[nodiscard]]
		constexpr node_type operator ()(const node_type& current, const Edge& edge, Args&&... args) const
		{
			// leave code as-is, because directly returning the temporary results in an ICE on gcc10
			node_type node{
				{std::invoke(static_cast<const Super&>(*this), current, edge, std::forward<Args>(args)...)},
				node::vertex(current)
			};
			return node;
		}
	};

	template <concepts::basic_node Node>
	struct DepthNode
		: public Node
	{
		using vertex_type = node::vertex_t<Node>;

		int depth{};

		[[nodiscard]]
		friend bool operator ==(const DepthNode&, const DepthNode&) = default;
	};

	template <concepts::basic_node Node, template<class> typename BaseNodeFactory>
	class NodeFactory<DepthNode<Node>, BaseNodeFactory>
		: private BaseNodeFactory<Node>
	{
	private:
		using Super = BaseNodeFactory<Node>;

	public:
		using node_type = DepthNode<Node>;
		using vertex_type = node::vertex_t<node_type>;

		template <typename... Args>
		[[nodiscard]]
		constexpr node_type operator ()(vertex_type origin, Args&&... args) const
		{
			// leave code as-is, because directly returning the temporary results in an ICE on gcc10
			node_type node{
				{std::invoke(static_cast<const Super&>(*this), std::move(origin), std::forward<Args>(args)...)},
				{0}
			};
			return node;
		}

		template <concepts::edge_for<node_type> Edge, typename... Args>
		[[nodiscard]]
		constexpr node_type operator ()(const node_type& current, const Edge& edge, Args&&... args) const
		{
			// leave code as-is, because directly returning the temporary results in an ICE on gcc10
			node_type node{
				{std::invoke(static_cast<const Super&>(*this), current, edge, std::forward<Args>(args)...)},
				{current.depth + 1}
			};
			return node;
		}
	};
}

#endif
