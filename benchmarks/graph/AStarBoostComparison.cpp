//          Copyright Dominic Koepke 2019 - 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#include <catch2/catch_approx.hpp>
#include <catch2/catch_get_random_seed.hpp>
#include <catch2/catch_template_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <experimental/mdarray>
namespace stdexp = std::experimental;

#include <boost/graph/astar_search.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/grid_graph.hpp>

#include <random>
#include <unordered_map>
#include <unordered_set>

#include <catch2/generators/catch_generators_random.hpp>

#include "Simple-Utility/graph/AStarSearch.hpp"

/* Most of this code is directly taken from https://github.com/boostorg/graph/blob/develop/example/astar_maze.cpp
 * and are used for benchmarking both, boost::graph and sl::graph.
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
	friend void random_maze(maze&, std::uint32_t);

	maze(std::size_t x, std::size_t y)
		: m_grid(create_grid(x, y)),
		m_barrier_grid(create_barrier_grid())
	{
	}

	// The length of the maze along the specified dimension.
	vertices_size_type length(std::size_t d) const { return m_grid.length(d); }

	bool has_barrier(vertex_descriptor u) const
	{
		return m_barriers.find(u) != m_barriers.end();
	}

	// Try to find a path from the lower-left-hand corner source (0,0) to the
	// upper-right-hand corner goal (x-1, y-1).
	vertex_descriptor source() const { return vertex(0, m_grid); }

	vertex_descriptor goal() const
	{
		return vertex(num_vertices(m_grid) - 1, m_grid);
	}

	std::optional<std::tuple<vertex_set, distance>> solve();

	bool solved() const { return !m_solution.empty(); }

	bool solution_contains(vertex_descriptor u) const
	{
		return m_solution.find(u) != m_solution.end();
	}

	const filtered_grid& get_grid() const noexcept
	{
		return m_barrier_grid;
	}

private:
	// Create the underlying rank-2 grid with the specified dimensions.
	grid create_grid(std::size_t x, std::size_t y)
	{
		boost::array lengths = {{x, y}};
		return grid(lengths);
	}

	// Filter the barrier vertices out of the underlying grid.
	filtered_grid create_barrier_grid()
	{
		return boost::make_vertex_subset_complement_filter(m_grid, m_barriers);
	}

	// The grid underlying the maze
	grid m_grid;
	// The barriers in the maze
	vertex_set m_barriers;
	// The underlying maze grid with barrier vertices filtered out
	filtered_grid m_barrier_grid;
	// The vertices on a solution path through the maze
	vertex_set m_solution;
	// The length of the solution path
	distance m_solution_length;
};

// Euclidean heuristic for a grid
//
// This calculates the Euclidean distance between a vertex and a goal
// vertex.
class euclidean_heuristic
	: public boost::astar_heuristic<filtered_grid, double>
{
public:
	euclidean_heuristic(vertex_descriptor goal)
		: m_goal(goal)
	{
	};

	double operator()(vertex_descriptor v) const
	{
		return sqrt(
			pow(static_cast<double>(m_goal[0] - v[0]), 2)
			+ pow(static_cast<double>(m_goal[1] - v[1]), 2));
	}

private:
	vertex_descriptor m_goal;
};

// Exception thrown when the goal vertex is found
struct found_goal
{
};

// Visitor that terminates when we find the goal vertex
struct astar_goal_visitor : public boost::default_astar_visitor
{
	astar_goal_visitor(vertex_descriptor goal)
		: m_goal(goal)
	{
	};

	void examine_vertex(vertex_descriptor u, const filtered_grid&)
	{
		if (u == m_goal)
		{
			throw found_goal();
		}
	}

private:
	vertex_descriptor m_goal;
};

// Solve the maze using A-star search.  Return true if a solution was found.
std::optional<std::tuple<vertex_set, distance>> maze::solve()
{
	boost::static_property_map<distance> weight(1);
	// The predecessor map is a vertex-to-vertex mapping.
	using pred_map = std::unordered_map<vertex_descriptor, vertex_descriptor,
										vertex_hash>;
	pred_map predecessor;
	boost::associative_property_map<pred_map> pred_pmap(predecessor);
	// The distance map is a vertex-to-distance mapping.
	using dist_map = std::unordered_map<vertex_descriptor, distance, vertex_hash>;
	dist_map distance;
	boost::associative_property_map<dist_map> dist_pmap(distance);

	vertex_descriptor s = source();
	vertex_descriptor g = goal();
	euclidean_heuristic heuristic(g);
	astar_goal_visitor visitor(g);

	try
	{
		astar_search(
			m_barrier_grid,
			s,
			heuristic,
			boost::weight_map(weight)
			.predecessor_map(pred_pmap)
			.distance_map(dist_pmap)
			.visitor(visitor));
	}
	catch (found_goal fg)
	{
		// Walk backwards from the goal through the predecessor chain adding
		// vertices to the solution path.
		for (vertex_descriptor u = g; u != s; u = predecessor[u])
		{
			m_solution.insert(u);
		}
		m_solution.insert(s);
		m_solution_length = distance[g];
		return std::tuple{m_solution, m_solution_length};
	}

	return std::nullopt;
}

// Generate a maze with a random assignment of barriers.
void random_maze(maze& m, const std::uint32_t seed)
{
	vertices_size_type n = num_vertices(m.m_grid);
	vertex_descriptor s = m.source();
	vertex_descriptor g = m.goal();
	// One quarter of the cells in the maze should be barriers.
	int barriers = n / 4;

	std::mt19937 rng{seed};
	while (barriers > 0)
	{
		// Choose horizontal or vertical direction.
		std::size_t direction = std::uniform_int<std::size_t>{0, 1}(rng);
		// Walls range up to one quarter the dimension length in this direction.
		vertices_size_type wall = std::uniform_int<std::size_t>{1, m.length(direction) / 4}(rng);
		// Create the wall while decrementing the total barrier count.
		vertex_descriptor u = vertex(std::uniform_int<std::size_t>{0, n - 1}(rng), m.m_grid);
		while (wall)
		{
			// Start and goal spaces should never be barriers.
			if (u != s && u != g)
			{
				wall--;
				if (!m.has_barrier(u))
				{
					m.m_barriers.insert(u);
					barriers--;
				}
			}
			vertex_descriptor v = m.m_grid.next(u, direction);
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
		std::tuple{euclidean_heuristic{m.goal()}, sg::astar::NodeFactory<Node>{}},
		std::tuple{}
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

TEMPLATE_TEST_CASE_SIG(
	"sl::graph and boost::graph generate equally good solutions",
	"[vs_boost][comparison]",
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
	maze m{width, height};
	random_maze(m, Catch::getSeed());

	const std::optional boostSolution = [m]() mutable { return m.solve(); }();
	const std::optional slSolution = [m] { return sl_graph_solve(m); }();

	REQUIRE(boostSolution.has_value() == slSolution.has_value());
	if (boostSolution.has_value())
	{
		REQUIRE(std::get<1>(*boostSolution) == Catch::Approx{std::get<1>(*slSolution)});
	}
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
	maze m{width, height};
	random_maze(m, Catch::getSeed());

	BENCHMARK("boost::graph")
	{
		return m.solve();
	};

	BENCHMARK("sl::graph")
	{
		return sl_graph_solve(m);
	};
}
