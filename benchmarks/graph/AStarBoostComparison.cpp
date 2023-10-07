//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <filesystem>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_get_random_seed.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/generators/catch_generators_random.hpp>

#include <boost/graph/astar_search.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/grid_graph.hpp>

#include <fstream>
#include <random>
#include <unordered_map>
#include <unordered_set>

#include "../Defines.hpp"

#include "Simple-Utility/graph/AStarSearch.hpp"

/* Most of this code is directly taken from https://github.com/boostorg/graph/blob/develop/example/astar_maze.cpp
 * and slightly modernized.
 * The example types are used for benchmarking both, boost::graph and sl::graph.
 */

// Distance traveled in the maze
using distance = double;

using grid = boost::grid_graph<2>;
using vertex_descriptor = boost::graph_traits<grid>::vertex_descriptor;
using edge_descriptor = boost::graph_traits<grid>::edge_descriptor;
using vertices_size_type = boost::graph_traits<grid>::vertices_size_type;

struct vertex_hash
{
	using argument_type = vertex_descriptor;
	using result_type = std::size_t;

	std::size_t operator()(const vertex_descriptor& u) const
	{
		std::size_t seed = 0;
		boost::hash_combine(seed, u[0]);
		boost::hash_combine(seed, u[1]);
		return seed;
	}
};

using vertex_set = boost::unordered_set<vertex_descriptor, vertex_hash>;
using filtered_grid = boost::vertex_subset_complement_filter<grid, vertex_set>::type;

// A searchable maze
//
// The maze is grid of locations which can either be empty or contain a
// barrier.  You can move to an adjacent location in the grid by going up,
// down, left and right.  Moving onto a barrier is not allowed.  The maze can
// be solved by finding a path from the lower-left-hand corner to the
// upper-right-hand corner.  If no open path exists between these two
// locations, the maze is unsolvable.
//
// The maze is implemented as a filtered grid graph where locations are
// vertices.  Barrier vertices are filtered out of the graph.
//
// A-star search is used to find a path through the maze. Each edge has a
// weight of one, so the total path length is equal to the number of edges
// traversed.
class maze
{
public:
	friend std::ostream& operator<<(std::ostream&, const maze&);
	friend void random_maze(maze&, std::uint32_t);

	explicit maze(const std::size_t x, const std::size_t y)
		: m_Grid{boost::array{{x, y}}},
		m_BarrierGrid(make_vertex_subset_complement_filter(m_Grid, m_Barriers))
	{
	}

	// The length of the maze along the specified dimension.
	vertices_size_type length(std::size_t d) const { return m_Grid.length(d); }

	bool has_barrier(vertex_descriptor u) const
	{
		return m_Barriers.find(u) != m_Barriers.end();
	}

	// Try to find a path from the lower-left-hand corner source (0,0) to the
	// upper-right-hand corner goal (x-1, y-1).
	vertex_descriptor source() const { return vertex(0, m_Grid); }

	vertex_descriptor goal() const
	{
		return vertex(num_vertices(m_Grid) - 1, m_Grid);
	}

	std::optional<std::tuple<vertex_set, distance>> solve();

	bool solved() const { return !m_Solution.empty(); }

	bool solution_contains(vertex_descriptor u) const
	{
		return m_Solution.find(u) != m_Solution.end();
	}

	const filtered_grid& get_grid() const noexcept
	{
		return m_BarrierGrid;
	}

private:
	// The grid underlying the maze
	grid m_Grid;
	// The barriers in the maze
	vertex_set m_Barriers{};
	// The underlying maze grid with barrier vertices filtered out
	filtered_grid m_BarrierGrid;
	// The vertices on a solution path through the maze
	vertex_set m_Solution{};
	// The length of the solution path
	distance m_SolutionLength{};
};

// Euclidean heuristic for a grid
//
// This calculates the Euclidean distance between a vertex and a goal
// vertex.
class euclidean_heuristic
	: public boost::astar_heuristic<filtered_grid, double>
{
public:
	explicit euclidean_heuristic(const vertex_descriptor& goal)
		: m_Goal{goal}
	{
	}

	double operator()(vertex_descriptor v) const
	{
		return std::sqrt(
			std::pow(static_cast<double>(m_Goal[0] - v[0]), 2)
			+ std::pow(static_cast<double>(m_Goal[1] - v[1]), 2));
	}

private:
	vertex_descriptor m_Goal;
};

// Exception thrown when the goal vertex is found
struct found_goal
{
};

// Visitor that terminates when we find the goal vertex
struct astar_goal_visitor : public boost::default_astar_visitor
{
	explicit astar_goal_visitor(const vertex_descriptor& goal)
		: m_Goal{goal}
	{
	}

	void examine_vertex(const vertex_descriptor& u, const filtered_grid&) const
	{
		if (u == m_Goal)
		{
			throw found_goal{};
		}
	}

private:
	vertex_descriptor m_Goal;
};

// Solve the maze using A-star search.  Return true if a solution was found.
std::optional<std::tuple<vertex_set, distance>> maze::solve()
{
	// The predecessor map is a vertex-to-vertex mapping.
	std::unordered_map<vertex_descriptor, vertex_descriptor, vertex_hash> predecessors{};
	// The distance map is a vertex-to-distance mapping.
	std::unordered_map<vertex_descriptor, distance, vertex_hash> distances{};

	const vertex_descriptor g = goal();
	const vertex_descriptor s = source();

	try
	{
		astar_search(
			m_BarrierGrid,
			source(),
			euclidean_heuristic{g},
			weight_map(boost::static_property_map{distance{1}})
			.predecessor_map(boost::associative_property_map{predecessors})
			.distance_map(boost::associative_property_map{distances})
			.visitor(astar_goal_visitor{g}));
	}
	catch (const found_goal&)
	{
		// Walk backwards from the goal through the predecessor chain adding
		// vertices to the solution path.
		for (vertex_descriptor u = g; u != s; u = predecessors[u])
		{
			m_Solution.insert(u);
		}
		m_Solution.insert(s);
		m_SolutionLength = distances[g];
		return std::tuple{m_Solution, m_SolutionLength};
	}

	return std::nullopt;
}

// Generate a maze with a random assignment of barriers.
void random_maze(maze& m, const std::uint32_t seed)
{
	const vertices_size_type n = num_vertices(m.m_Grid);
	const vertex_descriptor s = m.source();
	const vertex_descriptor g = m.goal();
	// One quarter of the cells in the maze should be barriers.
	vertices_size_type barriers{n / 4u};

	std::mt19937 rng{seed};
	while (barriers > 0)
	{
		// Choose horizontal or vertical direction.
		const std::size_t direction = std::uniform_int_distribution<std::size_t>{0, 1}(rng);
		// Walls range up to one quarter the dimension length in this direction.
		vertices_size_type wall = std::uniform_int_distribution<std::size_t>{1, m.length(direction) / 4}(rng);
		// Create the wall while decrementing the total barrier count.
		vertex_descriptor u = vertex(std::uniform_int_distribution<std::size_t>{0, n - 1}(rng), m.m_Grid);
		while (wall)
		{
			// Start and goal spaces should never be barriers.
			if (u != s && u != g)
			{
				--wall;
				if (!m.has_barrier(u))
				{
					m.m_Barriers.insert(u);
					if (0 == --barriers)
					{
						break;
					}
				}
			}
			vertex_descriptor v = m.m_Grid.next(u, direction);
			// Stop creating this wall if we reached the maze's edge.
			if (u == v)
			{
				break;
			}
			u = v;
		}
	}
}

/* #############################
 * Begin sl::graph related symbols
 ############################## */

template <>
struct sl::graph::view::traits<std::reference_wrapper<const maze>>
{
	using edge_type = CommonWeightedEdge<vertex_descriptor, distance>;
};

template <>
struct sl::graph::customize::edges_fn<std::reference_wrapper<const maze>>
{
	using edge_type = view::edge_t<std::reference_wrapper<const maze>>;
	using vertex_type = edge::vertex_t<edge_type>;
	using weight_type = edge::weight_t<edge_type>;

	constexpr auto operator ()(const maze& m, const auto& current) const
	{
		const auto& g = m.get_grid();
		const auto [edgesBegin, edgesEnd] = out_edges(node::vertex(current), g);
		return std::ranges::subrange{edgesBegin, edgesEnd}
				| std::views::transform([&](const auto& e) { return edge_type{target(e, g), 1.}; });
	}
};

std::optional<std::tuple<vertex_set, distance>> sl_graph_solve(const maze& m)
{
	namespace sg = sl::graph;

	using Node = sg::decorator::PredecessorNode<sg::astar::CommonNode<vertex_descriptor, distance>>;
	using Range = sg::astar::Range<
		std::reference_wrapper<const maze>,
		euclidean_heuristic,
		Node,
		sg::tracker::CommonHashMap<vertex_descriptor, vertex_hash>>;

	Range range{
		m.source(),
		std::make_tuple(std::ref(m)),
		std::tuple{},
		std::tuple{},
		std::tuple{sg::astar::NodeFactory<Node, euclidean_heuristic>{euclidean_heuristic{m.goal()}}}
	};

	std::unordered_map<vertex_descriptor, Node, vertex_hash> nodes{};
	for (const auto& node : range)
	{
		nodes.emplace(node.vertex, node);
		if (node.vertex == m.goal())
		{
			break;
		}
	}

	if (nodes.contains(m.goal()))
	{
		// Walk backwards from the goal through the predecessor chain adding
		// vertices to the solution path.
		vertex_set solution{};
		for (Node current = nodes[m.goal()]; current.predecessor; current = nodes[*current.predecessor])
		{
			solution.emplace(current.vertex);
		}
		solution.emplace(m.source());

		return std::tuple{solution, nodes[m.goal()].cost};
	}

	return std::nullopt;
}

/* #############################
 * End sl::graph related symbols
 ############################## */

// Print the maze as an ASCII map.
std::ostream& operator<<(std::ostream& output, const maze& m)
{
	constexpr char barrier = '#';
	constexpr char start = 'S';
	constexpr char goal = 'G';

	// Header
	for (vertices_size_type i = 0; i < m.length(0) + 2; i++)
	{
		output << barrier;
	}
	output << std::endl;
	// Body
	for (vertices_size_type i = 0; i < m.length(1); ++i)
	{
		const vertices_size_type y = m.length(1) - 1 - i;
		// Enumerate rows in reverse order and columns in regular order so that
		// (0,0) appears in the lower left-hand corner.  This requires that y be
		// int and not the unsigned vertices_size_type because the loop exit
		// condition is y==-1.
		for (vertices_size_type x = 0; x < m.length(0); x++)
		{
			// Put a barrier on the left-hand side.
			if (x == 0)
			{
				output << barrier;
			}
			// Put the character representing this point in the maze grid.
			vertex_descriptor u = {{x, y}};
			if (u == m.source())
			{
				output << start;
			}
			else if (u == m.goal())
			{
				output << goal;
			}
			else if (m.solution_contains(u))
			{
				output << ".";
			}
			else if (m.has_barrier(u))
			{
				output << barrier;
			}
			else
			{
				output << " ";
			}
			// Put a barrier on the right-hand side.
			if (x == m.length(0) - 1)
			{
				output << barrier;
			}
		}
		// Put a newline after every row except the last one.
		output << std::endl;
	}
	// Footer
	for (vertices_size_type i = 0; i < m.length(0) + 2; i++)
	{
		output << barrier;
	}
	if (m.solved())
	{
		output << std::endl << "Solution length " << m.m_SolutionLength;
	}
	else
	{
		output << std::endl << "Not solvable";
	}

	return output;
}

TEMPLATE_TEST_CASE_SIG(
	"Benchmarking sl::graph vs boost::graph AStar.",
	"[vs_boost][benchmark][benchmark::graph]",
	((int width, int height), width, height),
	(8, 8),
	(16, 16),
	(32, 32),
	(64, 64),
	(128, 128),
	(256, 256),
	(512, 512),
	(1024, 1024)
)
{
	const std::uint32_t seed = GENERATE(
		// add static seeds here
		Catch::getSeed());

	maze m{width, height};
	random_maze(m, Catch::getSeed());

	SECTION("Compare the results of both implementations.")
	{
		const std::optional boostSolution = [m, fileName = std::format("./{}_{}x{}.maze.txt", seed, width, height)]() mutable
		{
			auto result = m.solve();
			const auto path = artifacts_root_path() / "graph" / "astar_vs_boost";
			create_directories(path);
			std::ofstream out{path / fileName};
			out << m;
			return result;
		}();
		const std::optional slSolution = [m] { return sl_graph_solve(m); }();

		REQUIRE(boostSolution.has_value() == slSolution.has_value());
		if (boostSolution.has_value())
		{
			REQUIRE(std::get<1>(*boostSolution) == Catch::Approx{std::get<1>(*slSolution)});
		}
	}

	BENCHMARK("boost::graph")
	{
		return m.solve();
	};

	BENCHMARK("sl::graph")
	{
		return sl_graph_solve(m);
	};
}
