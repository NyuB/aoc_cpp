
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <iostream>

#include "utils.hpp"

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

  unsigned int count(std::string const &pattern, size_t i, size_t j) const {
    unsigned int res = 0;
    const size_t pattern_size = pattern.size();

    std::string left = "";
    for (size_t h = j; h != -1 && j - h < pattern_size; h--) {
      left.push_back(grid[i][h]);
    }
    if (left == pattern)
      res++;

    std::string right = "";
    for (size_t h = j; h < cols && h - j < pattern_size; h++) {
      right.push_back(grid[i][h]);
    }
    if (right == pattern)
      res++;

    std::string up = "";
    for (size_t v = i; v != -1 && i - v < pattern_size; v--) {
      up.push_back(grid[v][j]);
    }
    if (up == pattern)
      res++;

    std::string down = "";
    for (size_t v = i; v < rows && v - i < pattern_size; v++)
      down.push_back(grid[v][j]);
    if (down == pattern)
      res++;

    std::string diagonal_up_left = "";
    for (size_t step = 0; step < pattern_size; step++) {
      long r = i - step;
      long c = j - step;
      if (r >= 0 && c >= 0)
        diagonal_up_left.push_back(grid[r][c]);
    }
    if (diagonal_up_left == pattern)
      res++;

    std::string diagonal_up_right = "";
    for (size_t step = 0; step < pattern_size; step++) {
      long r = i - step;
      long c = j + step;
      if (r >= 0 && c < cols)
        diagonal_up_right.push_back(grid[r][c]);
    }
    if (diagonal_up_right == pattern)
      res++;

    std::string diagonal_down_left = "";
    for (size_t step = 0; step < pattern_size; step++) {
      long r = i + step;
      long c = j - step;
      if (r < rows && c >= 0)
        diagonal_down_left.push_back(grid[r][c]);
    }
    if (diagonal_down_left == pattern)
      res++;

    std::string diagonal_down_right = "";
    for (size_t step = 0; step < pattern_size; step++) {
      long r = i + step;
      long c = j + step;
      if (r < rows && c < cols)
        diagonal_down_right.push_back(grid[r][c]);
    }
    if (diagonal_down_right == pattern)
      res++;
    return res;
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
  unsigned int res = 0;
  Grid grid(lines);
  for (size_t i = 0; i < grid.height(); i++) {
    for (size_t j = 0; j < grid.width(); j++) {
      res += grid.count("XMAS", i, j);
    }
  }
  return res;
}

unsigned int solve_part_two(std::vector<std::string> const &lines) {
  unsigned int res = 0;
  Grid grid(lines);
  for (size_t i = 1; i < grid.height() - 1; i++) {
    for (size_t j = 1; j < grid.width() - 1; j++) {
      if (grid[i][j] != 'A')
        continue;
      bool up_left_diag =
          (grid[i - 1][j - 1] == 'S' && grid[i + 1][j + 1] == 'M') ||
          (grid[i - 1][j - 1] == 'M' && grid[i + 1][j + 1] == 'S');
      bool down_left_diag =
          (grid[i + 1][j - 1] == 'S' && grid[i - 1][j + 1] == 'M') ||
          (grid[i + 1][j - 1] == 'M' && grid[i - 1][j + 1] == 'S');
      if (up_left_diag && down_left_diag)
        res++;
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
               "MMMSXXMASM",
               "MSAMXMSMSA",
               "AMXSXMAAMM",
               "MSAMASMSMX",
               "XMASAMXAMM",
               "XXAMMXXAMA",
               "SMSMSASXSS",
               "SAXAMASAAA",
               "MAMMMXMMMM",
               "MXMXAXMASX",
           }),
           18);
}

TEST_CASE("Example Part Two") {
  CHECK_EQ(solve_part_two({
               "MMMSXXMASM",
               "MSAMXMSMSA",
               "AMXSXMAAMM",
               "MSAMASMSMX",
               "XMASAMXAMM",
               "XXAMMXXAMA",
               "SMSMSASXSS",
               "SAXAMASAAA",
               "MAMMMXMMMM",
               "MXMXAXMASX",
           }),
           9);
}

TEST_CASE("Single lines") { CHECK_EQ(solve_part_one({"XMAS"}), 1); }