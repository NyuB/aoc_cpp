#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <algorithm>
#include <iostream>
#include <numeric>
#include <ranges>
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
    std::ranges::sort(this->lefts);
    std::ranges::sort(this->rights);
  }

  std::vector<unsigned int> lefts;
  std::vector<unsigned int> rights;
};

inline unsigned int diff(unsigned int l, unsigned int r) {
  unsigned int high = std::max(l, r);
  unsigned int low = std::min(l, r);
  return high - low;
};

inline unsigned int add(unsigned int l, unsigned int r) { return l + r; }
inline unsigned int mul(unsigned int l, unsigned int r) { return l * r; }

unsigned int solve_part_one(std::vector<std::string> const &lines) {
  Problem problem(lines);
  problem.sort();
  const auto diffs =
      std::ranges::zip_transform_view(diff, problem.lefts, problem.rights);
  return std::accumulate(diffs.begin(), diffs.end(), 0, add);
}

unsigned int solve_part_two(std::vector<std::string> const &lines) {
  Problem problem(lines);
  const auto counts =
      problem.lefts | std::views::transform([&problem](unsigned int left) {
        return std::ranges::count(problem.rights, left);
      });
  const auto scores =
      std::ranges::zip_transform_view(mul, problem.lefts, counts);
  return std::accumulate(scores.begin(), scores.end(), 0, add);
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
