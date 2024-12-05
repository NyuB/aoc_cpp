
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <algorithm>
#include <iostream>

#include "utils.hpp"

struct Precedence {
  bool respected(std::vector<unsigned int> const &numbers) const {
    const auto beforeIndex = std::find(numbers.begin(), numbers.end(), before);
    const auto afterIndex = std::find(numbers.begin(), numbers.end(), after);
    return beforeIndex == numbers.end() || afterIndex == numbers.end() ||
           beforeIndex < afterIndex;
  }
  static bool respected(std::vector<Precedence> const &precedences,
                        std::vector<unsigned int> const &numbers) {
    return std::all_of(
        precedences.begin(), precedences.end(),
        [&numbers](const auto &p) { return p.respected(numbers); });
  }
  const unsigned int before;
  const unsigned int after;
};

struct Problem {
  static Problem parse(std::vector<std::string> const &lines) {
    std::vector<Precedence> precedences;
    std::vector<std::vector<unsigned int>> updates;
    auto it = lines.begin();
    for (; !(it->empty()); it++) {
      std::vector<unsigned int> lr = ints(*it, '|');
      precedences.push_back({lr[0], lr[1]});
    }
    it++;
    for (; it != lines.end(); it++) {
      updates.push_back(ints(*it, ','));
    }
    return {precedences, updates};
  }

  const std::vector<Precedence> precedences;
  const std::vector<std::vector<unsigned int>> updates;
};

unsigned int middle(std::vector<unsigned int> const &numbers) {
  return numbers[numbers.size() / 2];
}

unsigned int solve_part_one(std::vector<std::string> const &lines) {
  Problem problem = Problem::parse(lines);
  unsigned int res = 0;
  for (auto &update : problem.updates) {
    if (Precedence::respected(problem.precedences, update)) {
      res += middle(update);
    }
  }
  return res;
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
