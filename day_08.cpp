
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <iostream>
#include <map>
#include <numeric>
#include <optional>
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

  std::optional<Position> opposite(Position const &other, unsigned int rows,
                                   unsigned int cols) const {
    Position res{i - (other.i - i), j - (other.j - j)};
    if (res.i < 0 || static_cast<unsigned int>(res.i) >= rows || res.j < 0 ||
        static_cast<unsigned int>(res.j) >= cols)
      return {};
    return res;
  }

  std::vector<Position> opposites(Position const &other, unsigned int rows,
                                  unsigned int cols) const {
    Position delta{i - other.i, j - other.j};
    int gcd = std::gcd(delta.i, delta.j);
    delta = {delta.i / gcd, delta.j / gcd};
    std::vector<Position> res;
    Position current = other;
    while (in_grid_bound(current.i, rows) && in_grid_bound(current.j, cols)) {
      res.push_back(current);
      current = {current.i + delta.i, current.j + delta.j};
    }
    return res;
  }

  int i;
  int j;
};

class AntinodesModel {
public:
  virtual std::set<Position> antinodes(Position const &a, Position const &b,
                                       unsigned int rows,
                                       unsigned int cols) const = 0;

private:
};

class EquidistantAntinodes : public AntinodesModel {
public:
  virtual std::set<Position> antinodes(Position const &a, Position const &b,
                                       unsigned int rows,
                                       unsigned int cols) const {
    std::set<Position> res;
    const auto opposite_a = a.opposite(b, rows, cols);
    const auto opposite_b = b.opposite(a, rows, cols);
    if (opposite_a.has_value())
      res.insert(opposite_a.value());
    if (opposite_b.has_value())
      res.insert(opposite_b.value());
    return res;
  }
};

class AlignedAntinodes : public AntinodesModel {
public:
  virtual std::set<Position> antinodes(Position const &a, Position const &b,
                                       unsigned int rows,
                                       unsigned int cols) const {
    std::set<Position> res;
    for (const auto &o : a.opposites(b, rows, cols))
      res.insert(o);
    for (const auto &o : b.opposites(a, rows, cols))
      res.insert(o);
    return res;
  }
};

class Grid {
public:
  Grid(std::vector<std::string> const &lines) : grid(lines) {
    if (lines.empty()) {
      rows_ = 0;
      cols_ = 0;
    } else {
      rows_ = lines.size();
      cols_ = lines[0].size();
    }
  }

  Position end() const { return {(int)rows_, (int)cols_}; }

  std::string const &operator[](size_t i) const { return grid[i]; }
  char operator[](Position const &p) const { return grid[p.i][p.j]; }
  Grid clone() { return Grid(grid); }

  size_t rows() const { return cols_; }

  size_t cols() const { return rows_; }

private:
  std::vector<std::string> const &grid;
  size_t rows_;
  size_t cols_;
};

class AntennaMap {
public:
  void add(char id, Position const &pos) {
    if (antennas.find(id) == antennas.end()) {
      antennas[id] = std::vector<Position>();
    }
    antennas[id].push_back(pos);
  }
  std::vector<Position> const &operator[](char id) {
    if (antennas.find(id) == antennas.end()) {
      antennas[id] = std::vector<Position>();
    }
    return antennas[id];
  }

private:
  std::map<char, std::vector<Position>> antennas;
};

unsigned int solve(std::vector<std::string> const &lines,
                   AntinodesModel const &model) {
  Grid grid(lines);
  std::set<Position> antinodes;
  AntennaMap antennaMap;
  for (unsigned int i = 0; i < grid.rows(); i++) {
    for (unsigned int j = 0; j < grid.cols(); j++) {
      const Position position{static_cast<int>(i), static_cast<int>(j)};
      const char symbol = grid[position];
      if (symbol != '.') {
        for (const auto &other : antennaMap[symbol]) {
          for (const auto &antinode :
               model.antinodes(position, other, grid.rows(), grid.cols()))
            antinodes.insert(antinode);
        }
        antennaMap.add(symbol, position);
      }
    }
  }
  return antinodes.size();
}

unsigned int solve_part_one(std::vector<std::string> const &lines) {
  return solve(lines, EquidistantAntinodes());
}

unsigned int solve_part_two(std::vector<std::string> const &lines) {
  return solve(lines, AlignedAntinodes());
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
               "............",
               "........0...",
               ".....0......",
               ".......0....",
               "....0.......",
               "......A.....",
               "............",
               "............",
               "........A...",
               ".........A..",
               "............",
               "............",
           }),
           14);
}

TEST_CASE("Example Part Two") {
  CHECK_EQ(solve_part_two({
               "............",
               "........0...",
               ".....0......",
               ".......0....",
               "....0.......",
               "......A.....",
               "............",
               "............",
               "........A...",
               ".........A..",
               "............",
               "............",
           }),
           34);
}

TEST_CASE("Vertical opposites") {
  [[maybe_unused]] Position a{5, 0};
  [[maybe_unused]] Position b{10, 0};
  CHECK_EQ(a.opposite(b, 500, 500), std::optional(Position{0, 0}));
  CHECK_EQ(b.opposite(a, 500, 500), std::optional(Position{15, 0}));
}

TEST_CASE("Horizontal opposites") {
  [[maybe_unused]] Position a{0, 5};
  [[maybe_unused]] Position b{0, 10};
  CHECK_EQ(a.opposite(b, 500, 500), std::optional(Position{0, 0}));
  CHECK_EQ(b.opposite(a, 500, 500), std::optional(Position{0, 15}));
}

TEST_CASE("Out of bound opposites") {
  [[maybe_unused]] Position a{0, 5};
  [[maybe_unused]] Position b{0, 6};
  CHECK_EQ(a.opposite(b, 500, 7), std::optional(Position{0, 4}));
  CHECK_EQ(b.opposite(a, 500, 7), std::optional<Position>({}));

  [[maybe_unused]] Position c{0, 0};
  [[maybe_unused]] Position d{2, 0};
  CHECK_EQ(c.opposite(d, 500, 500), std::optional<Position>({}));
  CHECK_EQ(d.opposite(c, 500, 500), std::optional(Position{4, 0}));
}
