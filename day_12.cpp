
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <array>
#include <iostream>
#include <set>
#include <vector>

#include "utils.hpp"

struct Position {
  Position(int i_, int j_) : i(i_), j(j_) {}

  Position(size_t i_, size_t j_)
      : i(static_cast<int>(i_)), j(static_cast<int>(j_)) {}

  bool operator<(Position const &other) const {
    if (i == other.i)
      return j < other.j;
    return i < other.i;
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

class Grid {
public:
  Grid(std::vector<std::string> const &lines) : grid(lines) {
    rows = lines.size();
    cols = lines.empty() ? 0 : lines[0].size();
  }
  char operator[](Position const &p) const { return grid[p.i][p.j]; }
  bool inbound(Position const &p) const {
    return in_grid_bound(p.i, rows) && in_grid_bound(p.j, cols);
  }
  size_t height() const { return rows; }
  size_t width() const { return cols; }

private:
  std::vector<std::string> const &grid;
  size_t rows;
  size_t cols;
};

bool start_of_side(Grid const &grid, Position const &here, Position const &side,
                   Position const &before, Position const &before_side) {
  bool before_same = grid.inbound(before) && grid[before] == grid[here];
  bool side_same = grid.inbound(side) && grid[side] == grid[here];
  bool before_side_same =
      grid.inbound(before_side) && grid[before_side] == grid[here];
  if (side_same)
    return false;
  if (!before_same)
    return true;
  return before_side_same;
}

bool start_of_down_side(Grid const &grid, Position const &here) {
  return start_of_side(grid, here, here.down(), here.right(),
                       here.down_right());
}

bool start_of_up_side(Grid const &grid, Position const &here) {
  return start_of_side(grid, here, here.up(), here.left(), here.up_left());
}

bool start_of_left_side(Grid const &grid, Position const &here) {
  return start_of_side(grid, here, here.left(), here.down(), here.down_left());
}

bool start_of_right_side(Grid const &grid, Position const &here) {
  return start_of_side(grid, here, here.right(), here.up(), here.up_right());
}

unsigned int count_of_started_sides(Grid const &grid, Position const &here) {
  return start_of_left_side(grid, here) + start_of_down_side(grid, here) +
         start_of_up_side(grid, here) + start_of_right_side(grid, here);
}

void fill_area_one(Grid const &grid, Position const &p, unsigned int *area,
                   unsigned int *fences, std::set<Position> *visited) {
  std::vector<Position> q{p};
  while (!q.empty()) {
    Position here = q[q.size() - 1];
    q.pop_back();
    *area = *area + 1;
    std::array<Position, 4> neighbours{here.left(), here.right(), here.up(),
                                       here.down()};
    for (const auto &n : neighbours) {
      if (!grid.inbound(n) || grid[n] != grid[here])
        *fences = *fences + 1;
      else if (visited->find(n) == visited->end()) {
        visited->insert(n);
        q.push_back(n);
      }
    }
  }
}

void fill_area_two(Grid const &grid, Position const &p, unsigned int *area,
                   unsigned int *fences, std::set<Position> *visited) {
  std::vector<Position> q{p};
  while (!q.empty()) {
    Position here = q[q.size() - 1];
    q.pop_back();
    *area = *area + 1;
    std::array<Position, 4> neighbours{here.left(), here.right(), here.up(),
                                       here.down()};
    *fences = *fences + count_of_started_sides(grid, here);
    for (const auto &n : neighbours) {
      if (grid.inbound(n) && grid[n] == grid[here] &&
          visited->find(n) == visited->end()) {
        visited->insert(n);
        q.push_back(n);
      }
    }
  }
}

typedef void (*fill_area)(Grid const &, Position const &, unsigned int *,
                          unsigned int *, std::set<Position> *);

unsigned int solve(std::vector<std::string> const &lines, fill_area fill) {
  unsigned int res = 0;
  Grid grid(lines);
  std::set<Position> visited;
  for (size_t i = 0; i < grid.height(); i++) {
    for (size_t j = 0; j < grid.width(); j++) {
      Position p(i, j);
      if (visited.find(p) != visited.end())
        continue;
      visited.insert(p);
      unsigned int area = 0;
      unsigned int fences = 0;
      fill(grid, p, &area, &fences, &visited);
      res += area * fences;
    }
  }
  return res;
}

unsigned int solve_part_one(std::vector<std::string> const &lines) {
  return solve(lines, fill_area_one);
}

unsigned int solve_part_two(std::vector<std::string> const &lines) {
  return solve(lines, fill_area_two);
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

TEST_CASE("Example Part One") {
  CHECK_EQ(solve_part_one({
               "RRRRIICCFF",
               "RRRRIICCCF",
               "VVRRRCCFFF",
               "VVRCCCJFFF",
               "VVVVCJJCFE",
               "VVIVCCJJEE",
               "VVIIICJJEE",
               "MIIIIIJJEE",
               "MIIISIJEEE",
               "MMMISSJEEE",
           }),
           1930);
}

TEST_CASE("Example Part Two") {
  CHECK_EQ(solve_part_two({
               "RRRRIICCFF",
               "RRRRIICCCF",
               "VVRRRCCFFF",
               "VVRCCCJFFF",
               "VVVVCJJCFE",
               "VVIVCCJJEE",
               "VVIIICJJEE",
               "MIIIIIJJEE",
               "MIIISIJEEE",
               "MMMISSJEEE",
           }),
           1206);
}
