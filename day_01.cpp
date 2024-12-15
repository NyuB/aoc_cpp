#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "utils.hpp"

struct Problem {
  Problem(std::vector<std::string> const &lines) {
    for (auto line : lines) {
      StringIterator pair(line, ' ');
      auto left = pair.next();
      auto right = pair.next();
      lefts.push_back(svtoui(left));
      rights.push_back(svtoui(right));
    }
  }

  size_t size() { return lefts.size(); }

  void sort() {
    std::sort(lefts.begin(), lefts.end());
    std::sort(rights.begin(), rights.end());
  }

  std::vector<unsigned int> lefts;
  std::vector<unsigned int> rights;
};

unsigned int solve_part_one(std::vector<std::string> const &lines) {
  Problem problem(lines);
  problem.sort();
  unsigned int res = 0;
  for (size_t i = 0; i < problem.size(); i++) {
    unsigned int high = std::max(problem.lefts[i], problem.rights[i]);
    unsigned int low = std::min(problem.lefts[i], problem.rights[i]);
    res += high - low;
  }
  return res;
}

unsigned int solve_part_two(std::vector<std::string> const &lines) {
  Problem problem(lines);
  unsigned int res = 0;
  for (size_t i = 0; i < problem.size(); i++) {
    unsigned int left = problem.lefts[i];
    unsigned int count =
        std::count(problem.rights.begin(), problem.rights.end(), left);
    res += left * count;
  }
  return res;
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

TEST_CASE("Example part 1") {
  std::vector<std::string> const lines = {
      "3   4", "4   3", "2   5", "1   3", "3   9", "3   3",
  };
  CHECK_EQ(solve_part_one(lines), 11);
}

TEST_CASE("Example part 2") {
  std::vector<std::string> const lines = {
      "3   4", "4   3", "2   5", "1   3", "3   9", "3   3",
  };
  CHECK_EQ(solve_part_two(lines), 31);
}
