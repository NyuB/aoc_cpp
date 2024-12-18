
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <array>
#include <concepts>
#include <iostream>
#include <list>
#include <map>
#include <optional>
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

class Falls {
public:
  static Falls parse(std::vector<std::string> const &lines);
  std::optional<unsigned int> fall_time(Position const &p) const {
    auto find = fall_times.find(p);
    if (find == fall_times.end())
      return {};
    return find->second;
  }
  bool free(Position const &p, unsigned int time) const {
    std::optional<unsigned int> opt = fall_time(p);
    return !opt.has_value() || opt.value() > time;
  }

private:
  void record_fall(Position const &p, unsigned int t) { fall_times[p] = t; }
  std::map<Position, unsigned int> fall_times;
};

Falls Falls::parse(std::vector<std::string> const &lines) {
  Falls falls;
  unsigned int ft = 1;
  for (const auto &line : lines) {
    auto coords = ints(line, ',');
    falls.record_fall(Position(coords[1], coords[0]), ft);
    ft++;
  }
  return falls;
}

class Grid {
public:
  Grid(size_t height, size_t width) : rows(height), cols(width) {}
  bool inbound(Position const &p) const {
    return in_grid_bound(p.i, rows) && in_grid_bound(p.j, cols);
  }
  size_t height() const { return rows; }
  size_t width() const { return cols; }

private:
  size_t rows;
  size_t cols;
};

struct BFS {
  Position p;
  unsigned int t;
};

std::optional<unsigned int> solve(Grid const &grid, Falls const &falls,
                                  unsigned int time) {
  std::list<BFS> q;
  BFS start{{0, 0}, 0};
  q.push_back(start);
  std::set<Position> visited{start.p};
  while (!q.empty()) {
    BFS item = q.front();
    q.pop_front();
    if (item.p == Position{grid.height() - 1, grid.width() - 1})
      return item.t;
    std::array<Position, 4> neighbours{item.p.up(), item.p.right(),
                                       item.p.down(), item.p.left()};
    for (auto const &n : neighbours) {
      if (!grid.inbound(n) || !falls.free(n, time) ||
          (visited.find(n) != visited.end()))
        continue;
      visited.insert(n);
      q.push_back(BFS{n, item.t + 1});
    }
  }
  return {};
}

unsigned int solve_part_one(std::vector<std::string> const &lines, size_t side,
                            unsigned int time) {
  Grid grid(side, side);
  Falls falls = Falls::parse(lines);
  return solve(grid, falls, time).value_or(0);
}

class Unsolvable {
public:
  bool operator()(unsigned int t) const {
    return !solve(grid, falls, t).has_value();
  }
  Grid const &grid;
  Falls const &falls;
};

std::string solve_part_two(std::vector<std::string> const &lines, size_t side) {
  (void)lines;
  (void)side;
  Grid grid(side, side);
  Falls falls = Falls::parse(lines);
  unsigned int tmin = 1;
  unsigned int tmax = lines.size();

  unsigned int t = bisect(tmin, tmax, Unsolvable{grid, falls}).value_or(0);
  return lines[t - 1];
}

#ifdef DOCTEST_CONFIG_DISABLE
int main(int _, char *argv[]) {
  std::string filename = argv[2];
  std::string part = argv[1];
  if (part == "1") {
    unsigned int res = solve_part_one(read_input_file(filename), 71, 1024);
    std::cout << res << std::endl;
  } else if (part == "2") {
    std::cout << solve_part_two(read_input_file(filename), 71) << std::endl;
  }
}
#endif

#include "doctest/doctest.h"

#ifndef DOCTEST_CONFIG_DISABLE
// You can add test helpers such as doctest ostream << overrides here
#endif

TEST_CASE("Example Part One") {
  CHECK_EQ(
      solve_part_one(
          {
              "5,4", "4,2", "4,5", "3,0", "2,1", "6,3", "2,4", "1,5", "0,6",
              "3,3", "2,6", "5,1", "1,2", "5,5", "2,5", "6,5", "1,4", "0,4",
              "6,4", "1,1", "6,1", "1,0", "0,5", "1,6", "2,0",
          },
          7, 12),
      22);
}

TEST_CASE("Example Part Two") {
  CHECK_EQ(
      solve_part_two(
          {
              "5,4", "4,2", "4,5", "3,0", "2,1", "6,3", "2,4", "1,5", "0,6",
              "3,3", "2,6", "5,1", "1,2", "5,5", "2,5", "6,5", "1,4", "0,4",
              "6,4", "1,1", "6,1", "1,0", "0,5", "1,6", "2,0",
          },
          7),
      "6,1");
}
