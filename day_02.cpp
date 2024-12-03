#include "utils.hpp"
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

bool consistent(std::vector<unsigned int> const &numbers) {
  int slope = 0;
  if (numbers.size() < 2)
    return false;
  for (auto it = numbers.begin(); it < numbers.end() - 1; it++) {
    if (*it > *(it + 1)) {
      if (slope > 0)
        return false;
      slope = -1;
    } else if (*it < *(it + 1)) {
      if (slope < 0)
        return false;
      slope = 1;
    } else {
      return false;
    }
  }
  return true;
}

bool slow(std::vector<unsigned int> const &numbers) {
  for (auto it = numbers.begin(); it < numbers.end() - 1; it++) {
    unsigned int diff = std::max(*it, *(it + 1)) - std::min(*it, *(it + 1));
    if (diff > 3)
      return false;
    if (diff < 1)
      return false;
  }
  return true;
}

bool safe(std::vector<unsigned int> const &numbers) {
  return slow(numbers) && consistent(numbers);
}

unsigned int solve_part_one(std::vector<std::string> const &lines) {
  return std::count_if(lines.begin(), lines.end(), [](auto line) {
    const auto is = ints(line);
    return slow(is) && consistent(is);
  });
}

unsigned int solve_part_two(std::vector<std::string> const &lines) { return 0; }

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