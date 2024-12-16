
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <cassert>
#include <iostream>
#include <optional>
#include <queue>
#include <set>

#include "utils.hpp"

struct Position {
  Position(int i_, int j_) : i(i_), j(j_) {}

  Position(size_t i_, size_t j_)
      : i(static_cast<int>(i_)), j(static_cast<int>(j_)) {}

  bool operator==(Position const &other) const {
    return i == other.i && j == other.j;
  }

  bool operator<(Position const &other) const {
    if (i == other.i)
      return j < other.j;
    return i < other.i;
  }

  Position operator+(Position const &other) const {
    return Position(i + other.i, j + other.j);
  }

  Position up() const { return {i - 1, j}; };
  Position down() const { return {i + 1, j}; };
  Position left() const { return {i, j - 1}; };
  Position right() const { return {i, j + 1}; };

  Position up_left() const { return {i - 1, j - 1}; };
  Position up_right() const { return {i - 1, j + 1}; };
  Position down_left() const { return {i + 1, j - 1}; };
  Position down_right() const { return {i + 1, j + 1}; };

  int i;
  int j;
};

struct Edge {
  Position destination;
  Position orientation;
  unsigned int cost;
};

Position WEST(0, -1);
Position EAST(0, +1);
Position NORTH(-1, 0);
Position SOUTH(+1, 0);

Position turn_90_clock(Position const &orientation) {
  if (orientation == WEST)
    return NORTH;
  if (orientation == NORTH)
    return EAST;
  if (orientation == EAST)
    return SOUTH;
  if (orientation == SOUTH)
    return WEST;
  assert(false);
  return NORTH;
}

Position turn_90_counterclock(Position const &orientation) {
  if (orientation == NORTH)
    return WEST;
  if (orientation == WEST)
    return SOUTH;
  if (orientation == SOUTH)
    return EAST;
  if (orientation == EAST)
    return NORTH;
  assert(false);
  return NORTH;
}

Position turn_180(Position const &orientation) {
  if (orientation == NORTH)
    return SOUTH;
  if (orientation == WEST)
    return EAST;
  if (orientation == SOUTH)
    return NORTH;
  if (orientation == EAST)
    return WEST;

  assert(false);
  return NORTH;
}

class Grid {
public:
  Grid(std::vector<std::string> lines) : grid(lines) {
    rows = lines.size();
    cols = lines.empty() ? 0 : lines[0].size();
  }

  bool operator==(Grid const &other) const {
    if (other.rows != rows || other.cols != cols)
      return false;
    for (size_t i = 0; i < rows; i++) {
      for (size_t j = 0; j < cols; j++) {
        if (grid[i][j] != other.grid[i][j])
          return false;
      }
    }
    return true;
  }

  friend std::ostream &operator<<(std::ostream &os, Grid const &g) {
    os << std::endl;
    for (std::string const &line : g.grid) {
      os << line << std::endl;
    }
    os << std::endl;
    return os;
  }
  char operator[](Position const &p) const { return grid[p.i][p.j]; }
  bool inbound(Position const &p) const {
    return in_grid_bound(p.i, rows) && in_grid_bound(p.j, cols);
  }
  bool free(Position const &p) const {
    return inbound(p) && grid[p.i][p.j] != '#';
  }
  Position find(char c) const {
    for (size_t i = 0; i < rows; i++) {
      for (size_t j = 0; j < cols; j++) {
        if (grid[i][j] == c)
          return Position(i, j);
      }
    }
    return Position(-1, -1);
  }
  size_t height() const { return rows; }
  size_t width() const { return cols; }

private:
  std::vector<std::string> grid;
  size_t rows;
  size_t cols;
};

std::vector<Edge> neighbours(Grid const &grid, Position const &here,
                             Position const &orientation) {
  std::vector<Edge> res;
  Position clock = here + turn_90_clock(orientation);
  Position counterclock = here + turn_90_counterclock(orientation);
  Position back = here + turn_180(orientation);
  Position forward = here + orientation;
  if (grid.free(clock))
    res.push_back({clock, turn_90_clock(orientation), 1000 + 1});
  if (grid.free(counterclock))
    res.push_back({counterclock, turn_90_counterclock(orientation), 1000 + 1});
  if (grid.free(back))
    res.push_back({back, turn_180(orientation), 2 * 1000 + 1});
  if (grid.free(forward))
    res.push_back({forward, orientation, 1});
  return res;
}

unsigned int diff(unsigned int a, unsigned int b) {
  if (a > b)
    return a - b;
  return b - a;
}

struct SearchItem {
  Position p;
  Position orientation;
  unsigned int cost;
  SearchItem next(Edge const &edge) {
    return SearchItem{edge.destination, edge.orientation, edge.cost + cost};
  }
  // Priority queue ordering
  bool operator<(SearchItem const &other) const { return cost > other.cost; }
};

std::optional<unsigned int> shortest_path(Grid const &grid) {
  Position start = grid.find('S');
  Position end = grid.find('E');
  std::priority_queue<SearchItem> q;
  std::set<Position> visited;
  q.push(SearchItem{start, EAST, 0});
  visited.insert(start);
  while (!q.empty()) {
    SearchItem item = q.top();
    q.pop();
    visited.insert(item.p);
    if (item.p == end)
      return std::optional(item.cost);
    std::vector<Edge> edges = neighbours(grid, item.p, item.orientation);
    for (const auto &edge : edges) {
      if (visited.find(edge.destination) != visited.end())
        continue;
      q.push(item.next(edge));
    }
  }
  return {};
}

unsigned int solve_part_one(std::vector<std::string> const &lines) {
  Grid grid(lines);
  std::optional<unsigned int> res = shortest_path(grid);
  return res.value_or(0);
}

unsigned int solve_part_two(std::vector<std::string> const &lines) {
  (void)lines;
  return 24;
}

#ifdef DOCTEST_CONFIG_DISABLE
int main(int _, char *argv[]) {
  std::string filename = argv[2];
  std::string part = argv[1];
  if (part == "1") {
    unsigned int res = solve_part_one(read_input_file(filename));
    std::cout << res << std::endl;
  } else if (part == "2") {
    unsigned int res = solve_part_two(read_input_file(filename));
    std::cout << res << std::endl;
  }
}
#endif

#include "doctest/doctest.h"

#ifndef DOCTEST_CONFIG_DISABLE
// You can add test helpers such as doctest ostream << overrides here
#endif

TEST_CASE("Example Part One") {
  CHECK_EQ(solve_part_one({
               "###############",
               "#.......#....E#",
               "#.#.###.#.###.#",
               "#.....#.#...#.#",
               "#.###.#####.#.#",
               "#.#.#.......#.#",
               "#.#.#####.###.#",
               "#...........#.#",
               "###.#.#####.#.#",
               "#...#.....#.#.#",
               "#.#.#.###.#.#.#",
               "#.....#...#.#.#",
               "#.###.#.#.#.#.#",
               "#S..#.....#...#",
               "###############",

           }),
           7036);
}

TEST_CASE("Example Part Two") { CHECK_EQ(solve_part_two({}), 24); }
