
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <iostream>
#include <set>

#include "utils.hpp"

struct Position {
  unsigned int i;
  unsigned int j;

  bool operator<(Position const &other) const {
    if (i == other.i)
      return j < other.j;
    return i < other.i;
  }
};

struct Direction {
  static const Direction UP;
  static const Direction DOWN;
  static const Direction LEFT;
  static const Direction RIGHT;

  Position apply(Position const &position) const {
    if (opposite) {
      return {position.i - v, position.j - h};
    } else {
      return {position.i + v, position.j + h};
    }
  }

  bool operator==(Direction const &other) const {
    return h == other.h && v == other.v && opposite == other.opposite;
  }

  Direction turn_right() const {
    if (*this == UP) {
      return RIGHT;
    } else if (*this == RIGHT) {
      return DOWN;
    } else if (*this == DOWN) {
      return LEFT;
    } else {
      return UP;
    }
  }

  unsigned int v;
  unsigned int h;
  bool opposite;
};

const Direction Direction::UP = {1, 0, true};
const Direction Direction::DOWN = {1, 0, false};
const Direction Direction::LEFT = {0, 1, true};
const Direction Direction::RIGHT = {0, 1, false};

class Grid {
public:
  Grid(std::vector<std::string> const &lines) : grid(lines) {
    if (lines.empty()) {
      rows = 0;
      cols = 0;
    } else {
      rows = lines.size();
      cols = lines[0].size();
    }
  }

  Position end() const { return {(unsigned int)rows, (unsigned int)cols}; }

  Position find(char c) const {
    for (unsigned int i = 0; i < rows; i++) {
      for (unsigned int j = 0; j < cols; j++) {
        if (grid[i][j] == c)
          return {i, j};
      }
    }
    return end();
  }

  bool inbound(Position const &p) const {
    return p.i != -1 && p.j != -1 && p.i != height() && p.j != width();
  }

  const std::string &operator[](size_t i) const { return grid[i]; }

  size_t width() const { return cols; }

  size_t height() const { return rows; }

private:
  std::vector<std::string> const &grid;
  size_t rows;
  size_t cols;
};

unsigned int solve_part_one(std::vector<std::string> const &lines) {
  std::set<Position> visited;
  Grid grid(lines);
  Position current = grid.find('^');
  Direction direction = Direction::UP;
  while (grid.inbound(current)) {
    visited.insert(current);
    Position next = direction.apply(current);
    while (grid.inbound(next) && grid[next.i][next.j] == '#') {
      direction = direction.turn_right();
      next = direction.apply(current);
    }
    current = next;
  }
  return visited.size();
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
               "....#.....",
               ".........#",
               "..........",
               "..#.......",
               ".......#..",
               "..........",
               ".#..^.....",
               "........#.",
               "#.........",
               "......#...",
           }),
           41);
}
