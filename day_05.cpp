
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <algorithm>
#include <iostream>
#include <set>

#include "utils.hpp"

struct Precedence {
  bool respected(std::vector<unsigned int> const &numbers) const {
    const auto beforeIndex = std::find(numbers.begin(), numbers.end(), before);
    const auto afterIndex = std::find(numbers.begin(), numbers.end(), after);
    return beforeIndex == numbers.end() || afterIndex == numbers.end() ||
           beforeIndex < afterIndex;
  }

  const unsigned int before;
  const unsigned int after;
};

struct PrecedenceOrdering {
  bool respected(std::vector<unsigned int> const &numbers) const {
    return std::all_of(
        precedences.begin(), precedences.end(),
        [&numbers](const auto &p) { return p.respected(numbers); });
  }

  bool lt(unsigned int a, unsigned int b, std::set<unsigned int> *memo) const {
    memo->insert(a);
    for (const auto &p : precedences) {
      if (p.before == a) {
        if (p.after == b)
          return true;
        if (memo->find(p.after) != memo->end())
          continue;
        if (lt(p.after, b, memo))
          return true;
      }
    }
    return false;
  }

  PrecedenceOrdering only(std::vector<unsigned int> const &numbers) const {
    std::vector<Precedence> filtered_precedences;
    for (const auto &p : precedences) {
      if (std::find(numbers.begin(), numbers.end(), p.after) != numbers.end()) {
        if (std::find(numbers.begin(), numbers.end(), p.before) !=
            numbers.end())
          filtered_precedences.push_back(p);
      }
    }
    return {filtered_precedences};
  }
  bool operator()(unsigned int a, unsigned int b) const {
    std::set<unsigned int> memo;
    return lt(a, b, &memo);
  }
  std::vector<Precedence> const precedences;
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
    return {{precedences}, updates};
  }

  const PrecedenceOrdering ordering;
  const std::vector<std::vector<unsigned int>> updates;
};

unsigned int middle(std::vector<unsigned int> const &numbers) {
  return numbers[numbers.size() / 2];
}

unsigned int solve_part_one(std::vector<std::string> const &lines) {
  Problem problem = Problem::parse(lines);
  unsigned int res = 0;
  for (auto &update : problem.updates) {
    if (problem.ordering.respected(update)) {
      res += middle(update);
    }
  }
  return res;
}

unsigned int min(Problem const &problem,
                 std::vector<unsigned int> const &update) {
  unsigned int res = update[0];
  for (const auto &n : update) {
    if (problem.ordering(n, res))
      res = n;
  }
  return res;
}

unsigned int solve_part_two(std::vector<std::string> const &lines) {
  Problem problem = Problem::parse(lines);
  unsigned int res = 0;
  for (const auto &update : problem.updates) {
    if (!problem.ordering.respected(update)) {
      std::vector<unsigned int> sorted = update;
      std::sort(sorted.begin(), sorted.end(), problem.ordering.only(update));
      res += middle(sorted);
    }
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

TEST_CASE("Example Part One") {
  CHECK(solve_part_one({
            "47|53",          "97|13",
            "97|61",          "97|47",
            "75|29",          "61|13",
            "75|53",          "29|13",
            "97|29",          "53|29",
            "61|53",          "97|53",
            "61|29",          "47|13",
            "75|47",          "97|75",
            "47|61",          "75|61",
            "47|29",          "75|13",
            "53|13",          "",
            "75,47,61,53,29", "97,61,53,29,13",
            "75,29,13",       "75,97,47,61,53",
            "61,13,29",       "97,13,75,29,47",
        }) == 143);
}

TEST_CASE("Example Part Two") {
  CHECK(solve_part_two({
            "47|53",          "97|13",
            "97|61",          "97|47",
            "75|29",          "61|13",
            "75|53",          "29|13",
            "97|29",          "53|29",
            "61|53",          "97|53",
            "61|29",          "47|13",
            "75|47",          "97|75",
            "47|61",          "75|61",
            "47|29",          "75|13",
            "53|13",          "",
            "75,47,61,53,29", "97,61,53,29,13",
            "75,29,13",       "75,97,47,61,53",
            "61,13,29",       "97,13,75,29,47",
        }) == 123);
}
