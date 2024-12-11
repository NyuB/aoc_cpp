#include "utils.hpp"
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using skip_at = std::vector<unsigned int>::const_iterator;

bool consistent(std::vector<unsigned int> const &numbers, skip_at skipped) {
  int slope = 0;
  if (numbers.size() < 2)
    return false;
  for (auto it = numbers.begin(); it < numbers.end() - 1; it++) {
    if (it == skipped)
      continue;
    auto next = it + 1;
    if (next == skipped)
      next++;
    if (next == numbers.end())
      break;
    if (*it > *next) {
      if (slope > 0)
        return false;
      slope = -1;
    } else if (*it < *next) {
      if (slope < 0)
        return false;
      slope = 1;
    } else {
      return false;
    }
  }
  return true;
}

bool slow(std::vector<unsigned int> const &numbers, skip_at skipped) {
  for (auto it = numbers.begin(); it < numbers.end() - 1; it++) {
    if (it == skipped)
      continue;
    auto next = it + 1;
    if (next == skipped)
      next++;
    if (next == numbers.end())
      break;
    unsigned int diff = std::max(*it, *next) - std::min(*it, *next);
    if (diff > 3)
      return false;
    if (diff < 1)
      return false;
  }
  return true;
}

bool safe(std::vector<unsigned int> const &numbers) {
  return slow(numbers, numbers.end()) && consistent(numbers, numbers.end());
}

bool almost_safe(std::vector<unsigned int> const &numbers) {
  for (auto skipped = numbers.begin(); skipped != numbers.end(); skipped++) {
    if (slow(numbers, skipped) && consistent(numbers, skipped))
      return true;
  }
  return false;
}

unsigned int solve_part_one(std::vector<std::string> const &lines) {
  return std::count_if(lines.begin(), lines.end(), [](auto line) {
    const auto is = ints(line);
    return safe(is);
  });
}

unsigned int solve_part_two(std::vector<std::string> const &lines) {
  return std::count_if(lines.begin(), lines.end(), [](auto line) {
    const auto is = ints(line);
    return almost_safe(is);
  });
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

#include "doctest.h"

TEST_CASE("Example Part One") {
  CHECK_EQ(solve_part_one({
               "7 6 4 2 1",
               "1 2 7 8 9",
               "9 7 6 2 1",
               "1 3 2 4 5",
               "8 6 4 4 1",
               "1 3 6 7 9",
           }),
           2);
}

TEST_CASE("Example Part Two") {
  CHECK_EQ(solve_part_two({
               "7 6 4 2 1",
               "1 2 7 8 9",
               "9 7 6 2 1",
               "1 3 2 4 5",
               "8 6 4 4 1",
               "1 3 6 7 9",
           }),
           4);
}