//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/generators/catch_generators_adapters.hpp>
#include <catch2/generators/catch_generators_random.hpp>

#include "Defines.hpp"

#include "Simple-Utility/graph/Traverse.hpp"

#include <iterator>
#include <numeric>

// ReSharper disable CppDeclaratorNeverUsed
// ReSharper disable CppTypeAliasNeverUsed
// ReSharper disable CppClangTidyClangDiagnosticUnneededMemberFunction

namespace
{
	struct VertexMemberNode
	{
		using vertex_type = int;

		int vertex{};

		friend bool operator==(const VertexMemberNode&, const VertexMemberNode&) = default;
	};

	struct DefaultConstructibleQueue
	{
		std::vector<VertexMemberNode> queue{};

		[[nodiscard]]
		bool empty() const noexcept
		{
			return queue.empty();
		}

		void insert(const auto& elements)
		{
			queue.insert(std::end(queue), std::ranges::begin(elements), std::ranges::end(elements));
		}

		[[nodiscard]]
		VertexMemberNode next()
		{
			const VertexMemberNode front = queue.front();
			queue.erase(std::begin(queue));
			return front;
		}
	};
}

using DefaultNode = VertexMemberNode;
using DefaultQueue = QueueMock<DefaultNode>;
using DefaultState = sg::detail::BasicState<DefaultNode, DefaultQueue>;

TEST_CASE("BasicState is constructible from origin node and queue argument.", "[graph][graph::traverse][graph::detail]")
{
	using trompeloeil::_;

	const int originVertex = GENERATE(take(5, random(std::numeric_limits<int>::min(), std::numeric_limits<int>::max())));

	DefaultQueue queueMock{};
	REQUIRE_CALL(queueMock, do_insert(_))
		.WITH(std::ssize(_1) == 1)
		.WITH(sg::node::vertex(_1[0]) == originVertex);

	const DefaultState state{{originVertex}, std::move(queueMock)};
}

TEST_CASE("BasicState is constructible from origin node.", "[graph][graph::traverse][graph::detail]")
{
	const int originVertex = GENERATE(take(5, random(std::numeric_limits<int>::min(), std::numeric_limits<int>::max())));

	const sg::detail::BasicState<VertexMemberNode, DefaultConstructibleQueue> state{{originVertex}};

	REQUIRE(originVertex == sg::node::vertex(state.queue().queue.front()));
}

TEST_CASE("BasicState::next accepts current neighbors and returns the next node, if present.", "[graph][graph::traverse][graph::detail]")
{
	using trompeloeil::_;

	DefaultQueue queueMock{};
	REQUIRE_CALL(queueMock, do_insert(_))
		.WITH(std::ranges::equal(std::array{DefaultNode{42}}, _1));
	DefaultState state{{42}, std::move(queueMock)};

	SECTION("Providing neighbor infos on next call.")
	{
		std::vector<DefaultNode> neighbors{{44}, {45}, {46}};
		auto& queueMember = const_cast<DefaultQueue&>(state.queue());

		trompeloeil::sequence seq{};
		REQUIRE_CALL(queueMember, do_insert(_))
			.WITH(std::ranges::is_permutation(neighbors, _1))
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(queueMember, empty())
			.RETURN(false)
			.IN_SEQUENCE(seq);

		REQUIRE_CALL(queueMember, next())
			.RETURN(DefaultNode{42})
			.IN_SEQUENCE(seq);

		REQUIRE(DefaultNode{42} == state.next(neighbors));

		SECTION("And when the internal queue depletes.")
		{
			REQUIRE_CALL(queueMember, do_insert(_))
				.WITH(std::ranges::empty(_1))
				.IN_SEQUENCE(seq);

			REQUIRE_CALL(queueMember, empty())
				.RETURN(true)
				.IN_SEQUENCE(seq);

			REQUIRE(std::nullopt == state.next(std::array<DefaultNode, 0>{}));
		}
	}
}
