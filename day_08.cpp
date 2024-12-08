
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <iostream>
#include <map>
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
    if (other.i > i && other.i - i > i)
      return {};
    if (other.i < i && i - other.i + i >= rows)
      return {};
    if (other.j > j && other.j - j > j)
      return {};
    if (other.j < j && j - other.j + j >= cols)
      return {};
    unsigned int oi;
    unsigned int oj;
    if (other.i > i) {
      oi = i - (other.i - i);
    } else {
      oi = (i + (i - other.i));
    }
    if (other.j > j) {
      oj = j - (other.j - j);
    } else {
      oj = (j + (j - other.j));
    }
    return std::optional(Position{oi, oj});
  }
  unsigned int i;
  unsigned int j;
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

  Position end() const { return {(unsigned int)rows_, (unsigned int)cols_}; }

  const std::string &operator[](size_t i) const { return grid[i]; }
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

unsigned int solve_part_one(std::vector<std::string> const &lines) {
  Grid grid(lines);
  std::set<Position> antinodes;
  AntennaMap antennaMap;
  for (unsigned int i = 0; i < grid.rows(); i++) {
    for (unsigned int j = 0; j < grid.cols(); j++) {
      const Position position{i, j};
      const char symbol = grid[position];
      if (symbol != '.') {
        for (const auto &other : antennaMap[symbol]) {
          const auto opposite_a =
              position.opposite(other, grid.rows(), grid.cols());
          const auto opposite_b =
              other.opposite(position, grid.rows(), grid.cols());
          if (opposite_a.has_value())
            antinodes.insert(opposite_a.value());
          if (opposite_b.has_value())
            antinodes.insert(opposite_b.value());
        }
        antennaMap.add(symbol, position);
      }
    }
  }
  return antinodes.size();
}

unsigned int solve_part_two(std::vector<std::string> const &lines) {
  return 24;
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
TEST_CASE("Vertical opposites") {
  Position a{5, 0};
  Position b{10, 0};
  CHECK_EQ(a.opposite(b, 500, 500), std::optional(Position{0, 0}));
  CHECK_EQ(b.opposite(a, 500, 500), std::optional(Position{15, 0}));
}

TEST_CASE("Horizontal opposites") {
  Position a{0, 5};
  Position b{0, 10};
  CHECK_EQ(a.opposite(b, 500, 500), std::optional(Position{0, 0}));
  CHECK_EQ(b.opposite(a, 500, 500), std::optional(Position{0, 15}));
}

TEST_CASE("Out of bound opposites") {
  Position a{0, 5};
  Position b{0, 6};
  CHECK_EQ(a.opposite(b, 500, 7), std::optional(Position{0, 4}));
  CHECK_EQ(b.opposite(a, 500, 7), std::optional<Position>({}));

  Position c{0, 0};
  Position d{2, 0};
  CHECK_EQ(c.opposite(d, 500, 500), std::optional<Position>({}));
  CHECK_EQ(d.opposite(c, 500, 500), std::optional(Position{4, 0}));
}
