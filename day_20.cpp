
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <array>
#include <iostream>
#include <map>
#include <queue>
#include <set>

#include "utils.hpp"

struct Position {
  Position(int i_, int j_) : i(i_), j(j_) {}

  Position(size_t i_, size_t j_)
      : i(static_cast<int>(i_)), j(static_cast<int>(j_)) {}

  Position(unsigned int i_, unsigned int j_)
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

  friend std::ostream &operator<<(std::ostream &os, Position const &value) {
    os << value.i << "," << value.j;
    return os;
  }
};

struct Cheat {
  Position start;
  Position end;
  bool operator<(Cheat const &other) const {
    if (other.start == start)
      return end < other.end;
    return start < other.start;
  }
};

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
  char &operator[](Position const &p) { return grid[p.i][p.j]; }
  bool inbound(Position const &p) const {
    return in_grid_bound(p.i, rows) && in_grid_bound(p.j, cols);
  }
  bool free(Position const &p) const {
    return inbound(p) && grid[p.i][p.j] != '#';
  }
  std::vector<Cheat> wall_cheats(Position const &p) const {
    if (grid[p.i][p.j] != '#')
      return {};
    std::array<Position, 4> neighbours{p.up(), p.right(), p.down(), p.left()};
    std::vector<Cheat> res;
    for (size_t i = 0; i < neighbours.size(); i++) {
      for (size_t j = i + 1; j < neighbours.size(); j++) {
        Position ni = neighbours[i];
        Position nj = neighbours[j];
        if (!free(nj) || !free(ni))
          continue;
        res.push_back({ni, nj});
        res.push_back({nj, ni});
      }
    }
    return res;
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

struct BFS {
  Position p;
  unsigned int d;
};

std::vector<BFS> neighbours(Grid const &grid, std::vector<Cheat> const &cheats,
                            BFS const &item) {
  std::vector<BFS> res;
  std::array<Position, 4> next{item.p.up(), item.p.right(), item.p.down(),
                               item.p.left()};
  for (const auto &n : next) {
    if (grid.free(n))
      res.push_back({n, item.d + 1});
  }
  for (const auto &cheat : cheats) {
    if (item.p == cheat.start)
      res.push_back({cheat.end, item.d + 2});
  }
  return res;
}

std::map<Position, unsigned int> solve(Grid const &grid, Position const &start,
                                       Position const &end) {
  std::queue<BFS> q;
  std::map<Position, unsigned int> visited{{start, 0}};
  q.push({start, 0});
  while (!q.empty()) {
    BFS item = q.front();
    q.pop();
    if (item.p == end)
      return visited;
    for (const auto &n : neighbours(grid, {}, item)) {
      if (visited.find(n.p) != visited.end())
        continue;
      visited.insert({n.p, n.d});
      q.push(n);
    }
  }
  return {};
}

unsigned int solve_part_one(std::vector<std::string> const &lines,
                            unsigned int at_least) {
  Grid grid(lines);
  Position start = grid.find('S');
  Position end = grid.find('E');
  std::map<Position, unsigned int> path = solve(grid, start, end);
  unsigned int full = path[end];
  unsigned int res = 0;
  std::set<Cheat> cheats;
  for (size_t i = 0; i < grid.height(); i++) {
    for (size_t j = 0; j < grid.width(); j++) {
      for (const auto &cheat : grid.wall_cheats({i, j})) {
        if (path.contains(cheat.start) && path.contains(cheat.end) &&
            path[cheat.start] < path[cheat.end]) {
          unsigned int with_cheat =
              full - (path[cheat.end] - path[cheat.start]) + 2;
          if (with_cheat < full && full - with_cheat >= at_least)
            res++;
        }
      }
    }
  }
  return res;
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
    unsigned int res = solve_part_one(read_input_file(filename), 100);
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
  CHECK_EQ(solve_part_one(
               {

                   "###############",
                   "#...#...#.....#",
                   "#.#.#.#.#.###.#",
                   "#S#...#.#.#...#",
                   "#######.#.#.###",
                   "#######.#.#...#",
                   "#######.#.###.#",
                   "###..E#...#...#",
                   "###.#######.###",
                   "#...###...#...#",
                   "#.#####.#.###.#",
                   "#.#...#.#.#...#",
                   "#.#.#.#.#.#.###",
                   "#...#...#...###",
                   "###############",
               },
               1),
           44);
}

TEST_CASE("Example Part Two") {
  [[maybe_unused]] unsigned int REPLACE_WHEN_STARTING_PART_TWO = 24;
  CHECK_EQ(solve_part_two({}), REPLACE_WHEN_STARTING_PART_TWO);
}
