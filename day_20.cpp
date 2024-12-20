
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <array>
#include <iostream>
#include <map>
#include <queue>

#include "utils.hpp"

using Position = aoc::grid::Position;

struct Cheat {
  Position start;
  Position end;
  bool operator<(Cheat const &other) const {
    if (other.start == start)
      return end < other.end;
    return start < other.start;
  }
};

struct Cheats {
  void insert(Cheat cheat, unsigned int cost) {
    auto find = cheats.find(cheat);
    if (find != cheats.end()) {
      cheats[cheat] = std::min(cost, find->second);
    } else {
      cheats[cheat] = cost;
    }
  }
  std::map<Cheat, unsigned int> cheats;
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
  Cheats cheats_within(Position const &p, int range) const {
    Cheats res;
    for (int i = -range; i <= range; i++) {
      for (int j = -range + std::abs(i); j <= range - std::abs(i); j++) {
        if (i == 0 && j == 0)
          continue;
        Position n = {p.i + i, p.j + j};
        if (!free(n))
          continue;
        res.insert({p, n}, std::abs(i) + std::abs(j));
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

std::map<Position, unsigned int> bfs(Grid const &grid, Position const &start,
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
      if (visited.contains(n.p))
        continue;
      visited.insert({n.p, n.d});
      q.push(n);
    }
  }
  return {};
}

unsigned int solve(std::vector<std::string> const &lines, unsigned int at_least,
                   int range) {
  Grid grid(lines);
  Position start = grid.find('S');
  Position end = grid.find('E');
  std::map<Position, unsigned int> path = bfs(grid, start, end);
  unsigned int full = path[end];
  unsigned int res = 0;
  for (const auto &[p, _] : path) {
    auto cheats = grid.cheats_within(p, range);
    for (const auto &[cheat, cost] : cheats.cheats) {
      if (path.contains(cheat.end) && path[cheat.start] < path[cheat.end]) {
        unsigned int with_cheat =
            full - (path[cheat.end] - path[cheat.start]) + cost;
        if (with_cheat < full && full - with_cheat >= at_least)
          res++;
      }
    }
  }
  return res;
}

unsigned int solve_part_one(std::vector<std::string> const &lines,
                            unsigned int at_least) {
  return solve(lines, at_least, 2);
}

unsigned int solve_part_two(std::vector<std::string> const &lines,
                            unsigned int at_least) {
  return solve(lines, at_least, 20);
}

#ifdef DOCTEST_CONFIG_DISABLE
int main(int _, char *argv[]) {
  std::string filename = argv[2];
  std::string part = argv[1];
  if (part == "1") {
    unsigned int res = solve_part_one(read_input_file(filename), 100);
    std::cout << res << std::endl;
  } else if (part == "2") {
    unsigned int res = solve_part_two(read_input_file(filename), 100);
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
  CHECK_EQ(solve_part_two(
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
               50),
           32 + 31 + 29 + 39 + 25 + 23 + 20 + 19 + 12 + 14 + 12 + 22 + 4 + 3);
}

/**
 There are 32 cheats that save 50 picoseconds.
There are 31 cheats that save 52 picoseconds.
There are 29 cheats that save 54 picoseconds.
There are 39 cheats that save 56 picoseconds.
There are 25 cheats that save 58 picoseconds.
There are 23 cheats that save 60 picoseconds.
There are 20 cheats that save 62 picoseconds.
There are 19 cheats that save 64 picoseconds.
There are 12 cheats that save 66 picoseconds.
There are 14 cheats that save 68 picoseconds.
There are 12 cheats that save 70 picoseconds.
There are 22 cheats that save 72 picoseconds.
There are 4 cheats that save 74 picoseconds.
There are 3 cheats that save 76 picoseconds.
 */
