
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

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
  char &operator[](Position const &p) { return grid[p.i][p.j]; }
  char &at(Position const &p) { return grid[p.i][p.j]; }
  bool inbound(Position const &p) const {
    return in_grid_bound(p.i, rows) && in_grid_bound(p.j, cols);
  }
  void swap(Position const &from, Position const &to) {
    char save = grid[from.i][from.j];
    grid[from.i][from.j] = grid[to.i][to.j];
    grid[to.i][to.j] = save;
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

std::optional<Position> move(Grid *grid, Position const &from,
                             Position const &m) {
  Position to = from + m;
  if (!grid->inbound(to))
    return {};
  if (grid->at(to) == '#')
    return {};
  if (grid->at(to) == '.') {
    grid->swap(from, to);
    return std::optional(to);
  }
  if (grid->at(to) == 'O') {
    std::optional<Position> movable = move(grid, to, m);
    if (movable.has_value()) {
      grid->swap(from, to);
      return std::optional(to);
    }
  }
  return {};
}

size_t solve_part_one(std::vector<std::string> const &lines) {
  Problem problem = Problem::parse(lines);
  Position robot = problem.grid.find('@');
  for (Position const &m : problem.moves) {
    robot = move(&problem.grid, robot, m).value_or(robot);
  }
  size_t res = 0;
  for (size_t i = 0; i < problem.grid.height(); i++) {
    for (size_t j = 0; j < problem.grid.width(); j++) {
      if (problem.grid[Position(i, j)] == 'O')
        res += 100 * i + j;
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
    size_t res = solve_part_one(read_input_file(filename));
    std::cout << res << std::endl;
  } else if (part == "2") {
    unsigned int res = solve_part_two(read_input_file(filename));
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
#endif

TEST_CASE("Example Part One") {
  CHECK_EQ(solve_part_one({
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
           }),
           10092);
}

TEST_CASE("Example Part Two") {
  unsigned int REPLACE_WHEN_STARTING_PART_TWO = 24;
  CHECK_EQ(solve_part_two({}), REPLACE_WHEN_STARTING_PART_TWO);
}

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
  CHECK_EQ(grid[Position(4, 4)], '@');
  move(&grid, Position(4, 4), LEFT);
  CHECK_EQ(grid[Position(4, 4)], '.');
  CHECK_EQ(grid[Position(4, 3)], '@');
  CHECK_EQ(grid[Position(4, 2)], 'O');
  CHECK_EQ(grid[Position(4, 1)], '.');
}
