
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <array>
#include <iostream>
#include <set>
#include <vector>

#include "utils.hpp"

struct Position {
  bool operator==(Position const &other) const {
    return i == other.i && j == other.j;
  }

  bool operator<(Position const &other) const {
    if (i == other.i)
      return j < other.j;
    return i < other.i;
  }
  int i;
  int j;
};

class Grid {
public:
  Grid(std::vector<std::string> const &lines) : grid(lines) {
    rows = lines.size();
    cols = lines.empty() ? 0 : lines[0].size();
  }
  std::string const &operator[](int i) const { return grid[i]; }
  char operator[](Position const &p) const { return grid[p.i][p.j]; }
  bool ascending(Position const &from, Position const &to) const {
    return grid[from.i][from.j] == grid[to.i][to.j] - 1;
  };
  bool inbound(Position const &p) const {
    return p.i >= 0 && p.j >= 0 && p.i < rows && p.j < cols;
  }
  size_t height() const { return rows; }
  size_t width() const { return cols; }

private:
  std::vector<std::string> const &grid;
  size_t rows;
  size_t cols;
};

unsigned int reachable(char target, Grid const &grid, Position const &from) {
  std::set<Position> visited{};
  unsigned int res = 0;
  std::vector<Position> q{from};
  while (!q.empty()) {
    Position p = q[q.size() - 1];
    q.pop_back();
    if (visited.find(p) != visited.end())
      continue;
    visited.insert(p);
    if (grid[p] == target)
      res++;
    std::array<Position, 4> neighbours{
        Position{p.i - 1, p.j},
        Position{p.i + 1, p.j},
        Position{p.i, p.j - 1},
        Position{p.i, p.j + 1},
    };
    for (const auto &n : neighbours) {
      if (grid.inbound(n) && grid.ascending(p, n)) {
        q.push_back(n);
      }
    }
  }
  return res;
}

unsigned int reachable(char target, Grid const &grid, Position const &from,
                       std::set<Position> const &visited) {
  if (grid[from] == target)
    return 1;
  unsigned int res = 0;
  std::vector<Position> q{from};
  std::array<Position, 4> neighbours{
      Position{from.i - 1, from.j},
      Position{from.i + 1, from.j},
      Position{from.i, from.j - 1},
      Position{from.i, from.j + 1},
  };
  for (const auto &n : neighbours) {
    if (grid.inbound(n) && grid.ascending(from, n) &&
        visited.find(n) == visited.end()) {
      std::set<Position> next = visited;
      next.insert(n);
      res += reachable(target, grid, n, next);
    }
  }
  return res;
}

unsigned int solve_part_one(std::vector<std::string> const &lines) {
  Grid grid(lines);
  unsigned int res = 0;
  for (int i = 0; i < grid.height(); i++) {
    for (int j = 0; j < grid.width(); j++) {
      Position p{i, j};
      if (grid[p] == '0') {
        res += reachable('9', grid, p);
      }
    }
  }
  return res;
}

unsigned int solve_part_two(std::vector<std::string> const &lines) {
  Grid grid(lines);
  unsigned int res = 0;
  for (int i = 0; i < grid.height(); i++) {
    for (int j = 0; j < grid.width(); j++) {
      Position p{i, j};
      if (grid[p] == '0') {
        res += reachable('9', grid, p, std::set<Position>{p});
      }
    }
  }
  return res;
}

#ifdef DOCTEST_CONFIG_DISABLE
int main(int argc, char *argv[]) {
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

TEST_CASE("Example Part One") {
  CHECK_EQ(solve_part_one({
               "89010123",
               "78121874",
               "87430965",
               "96549874",
               "45678903",
               "32019012",
               "01329801",
               "10456732",
           }),
           36);
}

TEST_CASE("Example Part Two") {
  CHECK_EQ(solve_part_two({
               "89010123",
               "78121874",
               "87430965",
               "96549874",
               "45678903",
               "32019012",
               "01329801",
               "10456732",
           }),
           81);
}
