//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include "Simple-Utility/graph/Traverse.hpp"

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>

#include "Defines.hpp"

#include "Simple-Utility/graph/mixins/tracker/Null.hpp"
#include "Simple-Utility/test_util/Catch2Ext.hpp"
#include "Simple-Utility/test_util/TrompeloeilExt.hpp"

// ReSharper disable CppDeclaratorNeverUsed
// ReSharper disable CppTypeAliasNeverUsed
// ReSharper disable CppClangTidyClangDiagnosticUnneededMemberFunction

using DefaultNode = GenericBasicNode<int>;
using DefaultEdge = GenericBasicEdge<int>;
using DefaultQueue = QueueMock<DefaultNode>;
using DefaultTracker = TrackerMock<sg::node::vertex_t<DefaultNode>>;
using DefaultView = BasicViewMock<sg::node::vertex_t<DefaultNode>>;
using DefaultTraverser = sg::detail::BasicTraverser<
	DefaultNode,
	DefaultView,
	DefaultQueue,
	DefaultTracker>;

using MovableTraverser = sg::detail::BasicTraverser<
	DefaultNode,
	EmptyViewStub<int>,
	EmptyQueueStub<DefaultNode>,
	sg::tracker::Null>;

namespace
{
	template <sg::concepts::basic_node Node>
	struct TraverserMock
	{
		inline static constexpr bool trompeloeil_movable_mock = true;

		using node_type = Node;

		MAKE_MOCK0(next, std::optional<node_type>());
	};
}

using TestExplorers = std::tuple<
#ifdef SL_UTILITY_HAS_RANGES_VIEWS
	sg::detail::LazyExplorer,
#endif
	sg::detail::BufferedExplorer>;

TEMPLATE_LIST_TEST_CASE(
	"Explorer implementations behave as expected.",
	"[graph][graph::detail]",
	TestExplorers
)
{
	using namespace Catch::Matchers;

	constexpr DefaultNode current{.vertex = 42};
	const BasicViewMock<int> view{};
	REQUIRE_CALL(view, edges(current))
		.RETURN(std::vector<DefaultEdge>{{41}, {43}, {44}, {45}});

	TrackerMock<int> tracker{};
	REQUIRE_CALL(tracker, set_discovered(41))
		.RETURN(false);
	REQUIRE_CALL(tracker, set_discovered(43))
		.RETURN(false);
	REQUIRE_CALL(tracker, set_discovered(44))
		.RETURN(true);
	REQUIRE_CALL(tracker, set_discovered(45))
		.RETURN(false);

	TestType explorer{};
	REQUIRE_THAT(std::invoke(explorer, current, view, tracker), RangeEquals(std::array{DefaultEdge{44}}));
}

namespace
{
	template <typename Node, typename Edge>
	struct NodeFactoryMock
	{
		MAKE_CONST_MOCK1(MakeOrigin, Node(const sg::node::vertex_t<Node>& vertex));
		MAKE_CONST_MOCK2(MakeSuccessor, Node(const Node& current, const Edge& edge));

		[[nodiscard]]
		constexpr Node operator ()(const sg::node::vertex_t<Node>& vertex) const
		{
			return MakeOrigin(vertex);
		}

		[[nodiscard]]
		constexpr Node operator ()(const Node& current, const Edge& edge) const
		{
			return MakeSuccessor(current, edge);
		}
	};
}

using TestKernels = std::tuple<
#ifdef SL_UTILITY_HAS_RANGES_VIEWS
	sg::detail::LazyKernel<DefaultNode, NodeFactoryMock<DefaultNode, DefaultEdge>>,
#endif
	sg::detail::BufferedKernel<DefaultNode, NodeFactoryMock<DefaultNode, DefaultEdge>>>;

TEMPLATE_LIST_TEST_CASE(
	"Kernel implementations behave as expected.",
	"[graph][graph::detail]",
	TestKernels
)
{
	using namespace Catch::Matchers;

	TestType kernel{};

	SECTION("When creating origin.")
	{
		REQUIRE_CALL(kernel.node_factory(), MakeOrigin(42))
			.RETURN(DefaultNode{.vertex = 42});

		REQUIRE(DefaultNode{.vertex = 42} == std::invoke(kernel, 42));
	}

	SECTION("When creating successor(s).")
	{
		constexpr DefaultNode current{.vertex = 42};
		const auto& [expected, edges] = GENERATE(
			(table<std::vector<DefaultNode>, std::vector<DefaultEdge>>)({
				{{}, {}},
				{{{.vertex = 43}}, {{.destination = 43}}},
				{{{.vertex = 43}, {.vertex = 41}}, {{.destination = 43}, {.destination = 41}}}
				}));

		std::vector<std::unique_ptr<trompeloeil::expectation>> expectations{};
		for (const auto& edge : edges)
		{
			expectations.emplace_back(
				NAMED_REQUIRE_CALL(kernel.node_factory(), MakeSuccessor(current, edge))
					.RETURN(DefaultNode{.vertex = sg::edge::destination(edge)}));
		}

		REQUIRE_THAT(std::invoke(kernel, current, edges), RangeEquals(expected));
	}
}

TEST_CASE(
	"detail::BasicTraverser is not copyable but movable, when strategies support it.",
	"[graph][graph::traverser][graph::detail]"
)
{
	STATIC_REQUIRE(!std::is_copy_constructible_v<MovableTraverser>);
	STATIC_REQUIRE(!std::is_copy_assignable_v<MovableTraverser>);
	STATIC_REQUIRE(std::is_move_constructible_v<MovableTraverser>);
	STATIC_REQUIRE(std::is_move_assignable_v<MovableTraverser>);
}

TEMPLATE_TEST_CASE(
	"detail::BasicTraverser satisfies graph::concepts::traverser.",
	"[graph][graph::traverser][graph::concepts]",
	MovableTraverser,
	DefaultTraverser,
	TraverserMock<DefaultNode>
)
{
	STATIC_REQUIRE(sg::concepts::traverser<TestType>);
}

TEST_CASE("detail::BasicTraverser can be constructed with an origin.", "[graph][graph::traverser][graph::detail]")
{
	using namespace trompeloeil_ext;
	using namespace Catch::Matchers;

	const auto origin = GENERATE(take(5, random(std::numeric_limits<int>::min(), std::numeric_limits<int>::max())));

	DefaultQueue queue{};
	ALLOW_CALL(queue, empty()) // internal assertion
		.RETURN(true);
	REQUIRE_CALL(queue, do_insert(matches(RangeEquals(std::array{DefaultNode{origin}}))));

	DefaultTracker tracker{};
	REQUIRE_CALL(tracker, set_discovered(origin))
		.RETURN(true);

	const DefaultTraverser traverser{
		origin,
		std::forward_as_tuple(DefaultView{}),
		std::forward_as_tuple(std::move(queue)),
		std::forward_as_tuple(std::move(tracker)),
		std::tuple{},
		std::tuple{}
	};
}

TEST_CASE("detail::BasicTraverser::next returns the current node, or std::nullopt.", "[graph][graph::traverser][graph::detail]")
{
	using trompeloeil::_;
	using namespace trompeloeil_ext;
	using namespace catch_ext;
	using namespace Catch::Matchers;

	constexpr DefaultNode originNode{42};
	auto traverser = [&]
	{
		DefaultTracker trackerMock{};
		REQUIRE_CALL(trackerMock, set_discovered(42))
			.RETURN(true);

		DefaultQueue queue{};
		ALLOW_CALL(queue, empty()) // internal assertion
			.RETURN(true);
		REQUIRE_CALL(queue, do_insert(matches(RangeEquals(std::array{originNode}))));

		return DefaultTraverser{
			originNode.vertex,
			std::forward_as_tuple(DefaultView{}),
			std::forward_as_tuple(std::move(queue)),
			std::forward_as_tuple(std::move(trackerMock)),
			std::tuple{},
			std::tuple{}
		};
	}();

	using VertexInfo = DefaultView::edge_type;
	auto& view = const_cast<DefaultView&>(traverser.view());
	auto& queue = const_cast<DefaultQueue&>(traverser.queue());
	auto& tracker = const_cast<DefaultTracker&>(traverser.tracker());

	SECTION("Next returns a node, when queue contains elements.")
	{
		// vertex 43 will be skipped on purpose
		REQUIRE_CALL(view, edges(originNode))
			.RETURN(std::vector<VertexInfo>{{41}, {43}, {44}});

		REQUIRE_CALL(queue, do_insert(matches(RangeEquals(std::vector<DefaultNode>{{41}, {44}}))));
		REQUIRE_CALL(queue, empty())
			.RETURN(false);
		REQUIRE_CALL(queue, next())
			.RETURN(DefaultNode{.vertex = 42});

		REQUIRE_CALL(tracker, set_discovered(41))
			.RETURN(true);
		REQUIRE_CALL(tracker, set_discovered(43))
			.RETURN(false);
		REQUIRE_CALL(tracker, set_discovered(44))
			.RETURN(true);
		REQUIRE_CALL(tracker, set_visited(42))
			.RETURN(true);

		REQUIRE(DefaultNode{.vertex = 42} == traverser.next());
	}

	SECTION("Next returns std::nullopt, when queue is empty.")
	{
		REQUIRE_CALL(queue, empty())
			.RETURN(true);

		REQUIRE(std::nullopt == traverser.next());
	}
}

TEST_CASE("graph::IterableTraverser can be used as a range.", "[graph][graph::traverser]")
{
	SECTION("Is empty, when traverser returned std::nullopt.")
	{
		TraverserMock<DefaultNode> traverser{};
		REQUIRE_CALL(traverser, next())
			.RETURN(std::nullopt);

		sg::IterableTraverser range{std::move(traverser)};
		STATIC_REQUIRE(std::ranges::input_range<decltype(range)>);

		REQUIRE(0 == std::ranges::distance(range));
	}

	SECTION("Returns nodes as given from traverser.")
	{
		TraverserMock<DefaultNode> traverser{};
		trompeloeil::sequence seq{};
		REQUIRE_CALL(traverser, next())
			.RETURN(DefaultNode{.vertex = 42})
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(traverser, next())
			.RETURN(DefaultNode{.vertex = 41})
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(traverser, next())
			.RETURN(DefaultNode{.vertex = -42})
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(traverser, next())
			.RETURN(std::nullopt)
			.IN_SEQUENCE(seq);

		sg::IterableTraverser range{std::move(traverser)};
		STATIC_REQUIRE(std::ranges::input_range<decltype(range)>);

		REQUIRE_THAT(range, Catch::Matchers::RangeEquals(std::to_array<DefaultNode>({{42}, {41}, {-42}})));
	}
}

// Test case needs to be a template, because the negative requires statements will fail.
TEMPLATE_TEST_CASE(
	"graph::IterableTraverser::begin can only be called from an lvalue-ref.",
	"[graph][graph::traverser]",
	sg::IterableTraverser<TraverserMock<DefaultNode>>
)
{
	STATIC_REQUIRE(requires{ {std::declval<TestType&>().begin()} -> std::input_iterator; });
	STATIC_REQUIRE(!requires{std::declval<const TestType&>().begin(); });
	STATIC_REQUIRE(!requires{std::declval<TestType&&>().begin(); });
	STATIC_REQUIRE(!requires{std::declval<const TestType&&>().begin(); });
}
