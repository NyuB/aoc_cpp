
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <iostream>
#include <set>
#include <thread>

#include "utils.hpp"

struct Position {
  unsigned int i;
  unsigned int j;

  bool operator==(Position const &other) const {
    return i == other.i && j == other.j;
  }
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

  bool operator<(Direction const &other) const {
    if (h != other.h)
      return h < other.h;
    if (v != other.v)
      return v < other.v;
    return !opposite && other.opposite;
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

struct GuardPathPosition {
  Position position;
  Direction direction;

  bool operator<(GuardPathPosition const &other) const {
    if (other.position == position)
      return direction < other.direction;
    return position < other.position;
  }
};

class GuardPath {
public:
  void append(GuardPathPosition p) {
    path.push_back(p);
    auto insertion = uniques.insert(p);
    cyclic = !insertion.second;
  }
  std::set<Position> unique_positions() const {
    std::set<Position> upos;
    for (const auto &p : path) {
      upos.insert(p.position);
    }
    return upos;
  }
  bool is_cyclic() const { return cyclic; }

private:
  std::vector<GuardPathPosition> path;
  std::set<GuardPathPosition> uniques;
  bool cyclic = false;
};

class Grid {
public:
  Grid(std::vector<std::string> lines) : grid(lines) {
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
  char &operator[](Position const &p) { return grid[p.i][p.j]; }
  Grid clone() { return Grid(grid); }

  size_t width() const { return cols; }

  size_t height() const { return rows; }

private:
  std::vector<std::string> grid;
  size_t rows;
  size_t cols;
};

class GridView {
public:
  virtual bool blocked(Position const &p) const = 0;
  virtual bool inbound(Position const &p) const = 0;
  virtual Position start() const = 0;
  virtual ~GridView() {};
};

class SimpleGrid : public GridView {
public:
  SimpleGrid(Grid const &grid_) : grid(grid_) {}
  virtual bool blocked(Position const &p) const override {
    return grid[p.i][p.j] == '#';
  };
  virtual bool inbound(Position const &p) const override {
    return grid.inbound(p);
  }
  virtual Position start() const override { return grid.find('^'); }
  ~SimpleGrid() {};

private:
  Grid const &grid;
};

class BlockedGrid : public GridView {
public:
  BlockedGrid(GridView const &grid_, Position const &blocked_position_)
      : grid(grid_), blocked_position(blocked_position_) {}
  virtual bool blocked(Position const &p) const override {
    return p == blocked_position || grid.blocked(p);
  };
  virtual bool inbound(Position const &p) const override {
    return grid.inbound(p);
  }
  virtual Position start() const override { return grid.start(); }
  ~BlockedGrid() {};

private:
  GridView const &grid;
  Position const &blocked_position;
};

GuardPath guard_path(GridView const &grid) {
  GuardPath path;
  Position position = grid.start();
  Direction direction = Direction::UP;
  while (grid.inbound(position) && !path.is_cyclic()) {
    path.append({position, direction});
    Position next_position = direction.apply(position);
    while (grid.inbound(next_position) && grid.blocked(next_position)) {
      direction = direction.turn_right();
      next_position = direction.apply(position);
    }
    position = next_position;
  }
  return path;
}

unsigned int solve_part_one(std::vector<std::string> const &lines) {
  Grid grid(lines);
  return guard_path(SimpleGrid(grid)).unique_positions().size();
}

std::vector<std::vector<Position>> split_workloads(GuardPath const &path,
                                                   size_t factor) {
  std::set<Position> all = path.unique_positions();
  const size_t minimal_workload = 1;
  const size_t per_workload = std::max(minimal_workload, all.size() / factor);
  std::vector<Position> current;
  std::vector<std::vector<Position>> res;
  for (const auto &pos : all) {
    current.push_back(pos);
    if (current.size() == per_workload) {
      res.push_back(current);
      current.clear();
    }
  }
  if (!current.empty()) {
    res.push_back(std::move(current));
  }
  return res;
}

unsigned int solve_part_two(std::vector<std::string> const &lines) {
  const Grid grid(lines);
  const SimpleGrid base_view = SimpleGrid(grid);
  const GuardPath path = guard_path(SimpleGrid(grid));
  std::atomic<unsigned int> res = 0;
  std::vector<std::vector<Position>> workloads = split_workloads(path, 2);
  std::vector<std::thread> workers;
  for (const auto &w : workloads) {
    std::thread t([&w, &grid, &res, &base_view]() {
      for (const auto &pos : w) {
        if (base_view.start() == pos)
          continue;
        GuardPath alternative = guard_path(BlockedGrid(base_view, pos));
        if (alternative.is_cyclic())
          res++;
      }
    });
    workers.push_back(std::move(t));
  }

  for (auto &worker : workers) {
    worker.join();
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

TEST_CASE("Example Part Two") {
  CHECK_EQ(solve_part_two({
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
           6);
}

TEST_CASE("Clone") {
  Grid a({
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
  });
  Grid b = a.clone();
  a[{0, 0}] = 'X';
  CHECK_EQ(b[{0, 0}], '.');
}
