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

#include "Simple-Utility/test_util/Catch2Ext.hpp"
#include "Simple-Utility/test_util/TrompeloeilExt.hpp"

// ReSharper disable CppDeclaratorNeverUsed
// ReSharper disable CppTypeAliasNeverUsed
// ReSharper disable CppClangTidyClangDiagnosticUnneededMemberFunction

using DefaultNode = GenericBasicNode<int>;
using DefaultQueue = QueueMock<DefaultNode>;
using DefaultState = sg::detail::BasicState<DefaultNode, DefaultQueue>;
using DefaultTracker = TrackerMock<sg::node::vertex_t<DefaultNode>>;
using DefaultView = BasicViewMock<sg::node::vertex_t<DefaultNode>>;
using DefaultNodeFactory = GenericBasicNodeFactoryMock<DefaultNode>;
using DefaultDriver = sg::detail::BasicTraverseDriver<
	DefaultNode,
	DefaultState,
	DefaultTracker,
	DefaultNodeFactory>;

TEST_CASE("BasicState is constructible.", "[graph][graph::traverse][graph::detail]")
{
	using State = sg::detail::BasicState<DefaultNode, EmptyQueueStub<DefaultNode>>;

	SECTION("Default constructible.")
	{
		constexpr State state{};
	}

	SECTION("Constructible with queue object as argument.")
	{
		constexpr State state{EmptyQueueStub<DefaultNode>{}};
	}
}

TEST_CASE(
	"BasicState::next accepts current neighbors and returns the next node, if present.",
	"[graph][graph::traverse][graph::detail]")
{
	using trompeloeil::_;
	using namespace trompeloeil_ext;
	using namespace Catch::Matchers;
	using namespace catch_ext;

	DefaultQueue queue{};
	ALLOW_CALL(queue, empty()) // allow debug assertion
		.RETURN(true);
	DefaultState state{std::move(queue)};

	SECTION("Providing neighbor infos on next call.")
	{
		std::vector<DefaultNode> neighbors{{44}, {45}, {46}};
		auto& queueMember = const_cast<DefaultQueue&>(state.queue());

		trompeloeil::sequence seq{};
		REQUIRE_CALL(queueMember, do_insert(matches(UnorderedRangeEquals(neighbors))))
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(queueMember, empty())
			.RETURN(false)
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(queueMember, next())
			.RETURN(DefaultNode{44})
			.IN_SEQUENCE(seq);

		REQUIRE(DefaultNode{44} == state.next(neighbors));

		SECTION("And when the internal queue depletes.")
		{
			REQUIRE_CALL(queueMember, do_insert(matches(RangesEmpty{})))
				.IN_SEQUENCE(seq);

			REQUIRE_CALL(queueMember, empty())
				.RETURN(true)
				.IN_SEQUENCE(seq);

			REQUIRE(std::nullopt == state.next(std::array<DefaultNode, 0>{}));
		}
	}
}

TEST_CASE("BasicTraverseDriver can be constructed with an origin.", "[graph][graph::traverse][graph::detail]")
{
	using namespace trompeloeil_ext;
	using namespace Catch::Matchers;

	const auto origin = GENERATE(take(5, random(std::numeric_limits<int>::min(), std::numeric_limits<int>::max())));

	DefaultNodeFactory nodeFactoryMock{};
	REQUIRE_CALL(nodeFactoryMock, make_init_node(origin))
		.RETURN(DefaultNode{.vertex = origin});

	DefaultTracker trackerMock{};
	REQUIRE_CALL(trackerMock, set_discovered(origin))
		.RETURN(true);
	REQUIRE_CALL(trackerMock, set_visited(origin))
		.RETURN(true);

	const sg::detail::BasicTraverseDriver<
		DefaultNode,
		sg::detail::BasicState<DefaultNode, EmptyQueueStub<DefaultNode>>,
		DefaultTracker,
		DefaultNodeFactory> driver{
		origin,
		std::forward_as_tuple(std::move(trackerMock)),
		std::forward_as_tuple(std::move(nodeFactoryMock)),
		std::tuple{}
	};

	REQUIRE(DefaultNode{.vertex = origin} == driver.current_node());
}

TEST_CASE("BasicTraverseDriver::next returns the current node, or std::nullopt.", "[graph][graph::traverse][graph::detail]")
{
	using trompeloeil::_;
	using namespace trompeloeil_ext;
	using namespace catch_ext;
	using namespace Catch::Matchers;

	constexpr DefaultNode originNode{.vertex = 42};
	auto driver = [&]
	{
		DefaultNodeFactory nodeFactoryMock{};
		REQUIRE_CALL(nodeFactoryMock, make_init_node(42))
			.RETURN(originNode);

		DefaultTracker trackerMock{};
		REQUIRE_CALL(trackerMock, set_discovered(42))
			.RETURN(true);
		REQUIRE_CALL(trackerMock, set_visited(42))
			.RETURN(true);

		DefaultQueue queue{};
		ALLOW_CALL(queue, empty())
			.RETURN(true);

		return DefaultDriver{
			42,
			std::forward_as_tuple(std::move(trackerMock)),
			std::forward_as_tuple(std::move(nodeFactoryMock)),
			std::forward_as_tuple(std::move(queue))
		};
	}();

	using VertexInfo = DefaultView::edge_type;
	DefaultView view{};
	auto& nodeFactoryMock = const_cast<DefaultNodeFactory&>(driver.node_factory());
	auto& queueMock = const_cast<DefaultQueue&>(driver.state().queue());
	auto& trackerMock = const_cast<DefaultTracker&>(driver.tracker());

	SECTION("Next returns a node.")
	{
		// vertex 43 will be skipped on purpose
		REQUIRE_CALL(view, edges(originNode))
			.RETURN(std::vector<VertexInfo>{{41}, {43}, {44}});

		REQUIRE_CALL(nodeFactoryMock, make_successor_node(originNode, VertexInfo{41}))
			.RETURN(DefaultNode{.vertex = 41});
		REQUIRE_CALL(nodeFactoryMock, make_successor_node(originNode, VertexInfo{44}))
			.RETURN(DefaultNode{.vertex = 44});

		REQUIRE_CALL(queueMock, do_insert(matches(RangeEquals(std::vector<DefaultNode>{{41}, {44}}))));
		REQUIRE_CALL(queueMock, empty())
			.RETURN(false);
		REQUIRE_CALL(queueMock, next())
			.RETURN(DefaultNode{.vertex = 41});

		REQUIRE_CALL(trackerMock, set_discovered(41))
			.RETURN(true);
		REQUIRE_CALL(trackerMock, set_discovered(43))
			.RETURN(false);
		REQUIRE_CALL(trackerMock, set_discovered(44))
			.RETURN(true);
		REQUIRE_CALL(trackerMock, set_visited(41))
			.RETURN(true);

		//REQUIRE(DefaultNode{.vertex = 41} == driver.next(std::as_const(view)));
	}

	SECTION("Next returns std::nullopt.")
	{
		REQUIRE_CALL(view, edges(originNode))
			.RETURN(std::vector<sg::view::edge_t<DefaultView>>{});

		REQUIRE_CALL(queueMock, do_insert(matches(RangesEmpty{})));
		REQUIRE_CALL(queueMock, empty())
			.RETURN(true);

		REQUIRE(std::nullopt == driver.next(std::as_const(view)));
	}
}
