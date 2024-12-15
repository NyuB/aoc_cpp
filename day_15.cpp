
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <cassert>
#include <iostream>
#include <optional>

#include "utils.hpp"

struct Position {
  Position(int i_, int j_) : i(i_), j(j_) {}

  Position(size_t i_, size_t j_)
      : i(static_cast<int>(i_)), j(static_cast<int>(j_)) {}

  bool operator==(Position const &other) const {
    return i == other.i && j == other.j;
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

Position LEFT(0, -1);
Position RIGHT(0, +1);
Position UP(-1, 0);
Position DOWN(+1, 0);

class Grid {
public:
  Grid(std::vector<std::string> lines) : grid(lines) {
    rows = lines.size();
    cols = lines.empty() ? 0 : lines[0].size();
  }
  Grid(Grid const &other) { operator=(other); }
  Grid(Grid &&other) {
    grid = std::move(other.grid);
    rows = grid.size();
    cols = grid.empty() ? 0 : grid[0].size();
  }
  void operator=(Grid const &other) {
    grid = other.grid;
    rows = grid.size();
    cols = grid.empty() ? 0 : grid[0].size();
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
  char &at(Position const &p) { return grid[p.i][p.j]; }
  bool inbound(Position const &p) const {
    return in_grid_bound(p.i, rows) && in_grid_bound(p.j, cols);
  }
  void swap(Position const &from, Position const &to) {
    std::swap(grid[from.i][from.j], grid[to.i][to.j]);
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

struct Problem {
  static Problem parse(std::vector<std::string> const &lines);
  static Problem parse_two(std::vector<std::string> const &lines);
  Grid grid;
  std::vector<Position> moves;
};

void add_moves(std::string const &line, std::vector<Position> *moves) {
  for (char c : line) {
    switch (c) {
    case '<':
      moves->push_back(LEFT);
      break;
    case '>':
      moves->push_back(RIGHT);
      break;
    case '^':
      moves->push_back(UP);
      break;
    case 'v':
      moves->push_back(DOWN);
      break;
    }
  }
}

Problem Problem::parse(std::vector<std::string> const &lines) {
  std::vector<std::string> grid_lines;
  size_t index = 0;
  while (!lines[index].empty()) {
    grid_lines.push_back(lines[index]);
    index++;
  }
  index++;
  std::vector<Position> moves;
  while (index < lines.size()) {
    add_moves(lines[index], &moves);
    index++;
  }
  return Problem{Grid(grid_lines), std::move(moves)};
}

std::string expand_line(std::string const &line) {
  std::string res = "";
  res.reserve(line.size() * 2);
  for (char c : line) {
    if (c == '#')
      res += "##";
    if (c == '.')
      res += "..";
    if (c == '@')
      res += "@.";
    if (c == 'O')
      res += "[]";
  }
  return res;
}

Problem Problem::parse_two(std::vector<std::string> const &lines) {
  std::vector<std::string> grid_lines;
  size_t index = 0;
  while (!lines[index].empty()) {
    grid_lines.push_back(expand_line(lines[index]));
    index++;
  }
  index++;
  std::vector<Position> moves;
  while (index < lines.size()) {
    add_moves(lines[index], &moves);
    index++;
  }
  return Problem{Grid(grid_lines), std::move(moves)};
}

inline bool is_box(char c) { return c == 'O' || c == '[' || c == ']'; }
inline bool count_as_box(char c) { return c == 'O' || c == '['; }
inline bool is_free(char c) { return c == '.'; }
inline bool is_wall(char c) { return c == '#'; }
inline bool is_vertical_move(Position const &m) { return m.j == 0; }

/**
 * try_vertical_box_move(grid, to, m) Attempt to move a box tile vertically by
 * `m`
 * @return `true` if this move is possible, false otherwise
 * @warning this is a destructive operation, `grid` is left in an invalid state
 * when this function returns `false`. `grid` is guaranteed to be in a stable
 * state when this function returns `true`
 */
bool try_vertical_box_move(Grid *grid, Position const &box, Position const &m);

/**
 * try_move(grid, from, m) Attempt to move a tile by `m`
 * @return the position of the tile after moving if this move is possible, empty
 * otherwise
 * @warning this is a destructive operation, `grid` is left in an invalid state
 * when this function returns empty. `grid` is guaranteed to be in a stable
 * state when this function returns a non-empty value.
 */
std::optional<Position> try_move(Grid *grid, Position const &from,
                                 Position const &m) {
  Position to = from + m;
  if (!grid->inbound(to))
    return {};
  if (is_wall(grid->at(to)))
    return {};
  if (is_free(grid->at(to))) {
    grid->swap(from, to);
    return std::optional(to);
  }
  if (is_box(grid->at(to))) {
    bool movable;
    if (is_vertical_move(m)) {
      movable = try_vertical_box_move(grid, to, m);
    } else {
      movable = try_move(grid, to, m).has_value();
    }
    if (movable) {
      grid->swap(from, to);
      return std::optional(to);
    }
  }
  return {};
}

bool try_vertical_box_move(Grid *grid, Position const &box, Position const &m) {
  if (grid->at(box) == 'O')
    return try_move(grid, box, m).has_value();
  else if (grid->at(box) == '[') {
    std::optional<Position> left = try_move(grid, box, m);
    std::optional<Position> right = try_move(grid, box + RIGHT, m);
    return left.has_value() && right.has_value();
  } else if (grid->at(box) == ']') {
    std::optional<Position> left = try_move(grid, box + LEFT, m);
    std::optional<Position> right = try_move(grid, box, m);
    return left.has_value() && right.has_value();
  }

  assert(false);
  return {};
}

std::optional<Position> move(Grid *grid, Position const &from,
                             Position const &m) {
  Grid copy = *grid;
  std::optional<Position> attempt = try_move(&copy, from, m);
  if (attempt.has_value()) {
    // Replace grid with a known-valid updated state
    *grid = std::move(copy);
  }
  return attempt;
}

size_t solve(Problem *problem) {
  Position robot = problem->grid.find('@');
  for (Position const &m : problem->moves) {
    robot = move(&problem->grid, robot, m).value_or(robot);
  }
  size_t res = 0;
  for (size_t i = 0; i < problem->grid.height(); i++) {
    for (size_t j = 0; j < problem->grid.width(); j++) {
      if (count_as_box(problem->grid[Position(i, j)]))
        res += 100 * i + j;
    }
  }

  return res;
}

size_t solve_part_one(std::vector<std::string> const &lines) {
  Problem problem = Problem::parse(lines);
  return solve(&problem);
}

size_t solve_part_two(std::vector<std::string> const &lines) {
  Problem problem = Problem::parse_two(lines);
  return solve(&problem);
}

#ifdef DOCTEST_CONFIG_DISABLE
int main(int _, char *argv[]) {
  std::string filename = argv[2];
  std::string part = argv[1];
  if (part == "1") {
    size_t res = solve_part_one(read_input_file(filename));
    std::cout << res << std::endl;
  } else if (part == "2") {
    size_t res = solve_part_two(read_input_file(filename));
    std::cout << res << std::endl;
  }
}
#endif

#include "doctest/doctest.h"

#ifndef DOCTEST_CONFIG_DISABLE
std::ostream &operator<<(std::ostream &os, Position const &value) {
  os << "(" << value.i << ", " << value.j << ")";
  return os;
}
std::vector<std::string> large_example{
    "##########",
    "#..O..O.O#",
    "#......O.#",
    "#.OO..O.O#",
    "#..O@..O.#",
    "#O#..O...#",
    "#O..O..O.#",
    "#.OO.O.OO#",
    "#....O...#",
    "##########",
    "",
    "<vv>^<v^>v>^vv^v>v<>v^v<v<^vv<<<^><<><>>v<vvv<>^v^>^<<<><<v<<<"
    "v^vv^v>^",
    "vvv<<^>^v^^><<>>><>^<<><^vv^^<>vvv<>><^^v>^>vv<>v<<<<v<^v>^<^^>"
    ">>^<v<v",
    "><>vv>v^v^<>><>>>><^^>vv>v<^^^>>v^v^<^^>v^^>v^<^v>v<>>v^v^<v>v^"
    "^<^^vv<",
    "<<v<^>>^^^^>>>v^<>vvv^><v<<<>^^^vv^<vvv>^>v<^^^^v<>^>vvvv><>>v^"
    "<<^^^^^",
    "^><^><>>><>^^<<^^v>>><^<v>^<vv>>v>>>^v><>^v><<<<v>>v<v<v>vvv>^<"
    "><<>^><",
    "^>><>^v<><^vvv<^^<><v<<<<<><^v<<<><<<^^<v<^^^><^>>^<v^><<<^>>^"
    "v<v^v<v^",
    ">^>>^v>vv>^<<^v<>><<><<v<<v><>v<^vv<<<>^^v^>^^>>><<^v>>v^v><^^>"
    ">^<>vv^",
    "<><^^>^^^<><vvvvv^v<v<<>^v<v>v<<^><<><<><<<^^<<<^<<>><<><^^^>^^"
    "<>^>v<>",
    "^^>vv<^v^v<vv>^<><v<^v>^^^>>>^^vvv^>vvv<>>>^<^>>>>>^<<^v>^vvv<>"
    "^<><<v>",
    "v^^>>><<^^<>>^v^<v^vv<>v^<<>^<^v^v><^<<<><<^<v><v<>vv>>v><v^<"
    "vv<>v^<<^",
};
#endif

TEST_CASE("Example Part One") {
  CHECK_EQ(solve_part_one(large_example), 10092);
}

TEST_CASE("Example Part Two") { CHECK_EQ(solve_part_two(large_example), 9021); }

TEST_CASE("Grid ops") {
  Grid grid({
      "##########",
      "#..O..O.O#",
      "#......O.#",
      "#.OO..O.O#",
      "#..O@..O.#",
      "#O#..O...#",
      "#O..O..O.#",
      "#.OO.O.OO#",
      "#....O...#",
      "##########",
  });
  try_move(&grid, Position(4, 4), LEFT);
  CHECK_EQ(grid, Grid({
                     "##########",
                     "#..O..O.O#",
                     "#......O.#",
                     "#.OO..O.O#",
                     "#.O@...O.#",
                     "#O#..O...#",
                     "#O..O..O.#",
                     "#.OO.O.OO#",
                     "#....O...#",
                     "##########",
                 }));
}

TEST_CASE("Big boxes") {
  Grid grid({
      "##########",
      "#........#",
      "#..[][]..#",
      "#...[]...#",
      "#...@....#",
      "##########",
  });
  CHECK_EQ(grid[Position(4, 4)], '@');
  try_move(&grid, Position(4, 4), UP);
  CHECK_EQ(grid, Grid({
                     "##########",
                     "#..[][]..#",
                     "#...[]...#",
                     "#...@....#",
                     "#........#",
                     "##########",
                 }));
}
