
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <algorithm>
#include <iostream>
#include <map>
#include <ranges>

#include "utils.hpp"

using number = unsigned long long;

bool possible(std::string const &target,
              std::vector<std::string> const &available,
              std::map<std::string, bool> *memo) {

  auto m = memo->find(target);
  if (m != memo->end())
    return m->second;
  bool res = false;
  if (target.empty())
    res = true;
  else {
    for (const auto &a : available) {
      if (std::ranges::starts_with(target, a)) {
        std::string next =
            std::ranges::subrange(target.begin() + a.size(), target.end()) |
            std::ranges::to<std::string>();
        if (possible(next, available, memo)) {
          res = true;
          break;
        }
      }
    }
  }
  memo->insert_or_assign(target, res);
  return res;
}

number all_possible(std::string const &target,
                    std::vector<std::string> const &available,
                    std::map<std::string, number> *memo) {

  auto m = memo->find(target);
  if (m != memo->end())
    return m->second;
  number res = 0;
  if (target.empty())
    res = 1;
  else {
    for (const auto &a : available) {
      if (std::ranges::starts_with(target, a)) {
        std::string next =
            std::ranges::subrange(target.begin() + a.size(), target.end()) |
            std::ranges::to<std::string>();
        res += all_possible(next, available, memo);
      }
    }
  }
  memo->insert_or_assign(target, res);
  return res;
}

std::vector<std::string> parse_available_patterns(std::string const &line) {
  std::vector<std::string> available;
  for (const auto pattern :
       std::ranges::split_view(line, std::string_view(", "))) {
    std::string copy = pattern | std::ranges::to<std::string>();
    available.push_back(copy);
  }
  return available;
}

bool descending_string_size(std::string const &a, std::string const &b) {
  return a.size() > b.size();
}

unsigned int solve_part_one(std::vector<std::string> const &lines) {
  std::vector<std::string> available = parse_available_patterns(lines[0]);

  // Try biggest substrings first
  std::ranges::sort(available, descending_string_size);

  unsigned int res = 0;
  std::map<std::string, bool> memo;
  for (const auto &l : std::ranges::subrange(lines.begin() + 2, lines.end())) {
    if (possible(l, available, &memo))
      res++;
  }
  return res;
}

number solve_part_two(std::vector<std::string> const &lines) {
  std::vector<std::string> available = parse_available_patterns(lines[0]);
  number res = 0;
  std::map<std::string, number> memo;
  for (const auto &l : std::ranges::subrange(lines.begin() + 2, lines.end())) {
    res += all_possible(l, available, &memo);
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
    number res = solve_part_two(read_input_file(filename));
    std::cout << res << std::endl;
  }
}
#endif

#include "doctest/doctest.h"

#ifndef DOCTEST_CONFIG_DISABLE
// You can add test helpers such as doctest ostream << overrides here
#endif

TEST_CASE("Example Part One") {
  CHECK_EQ(solve_part_one({
               "r, wr, b, g, bwu, rb, gb, br",
               "",
               "brwrr",
               "bggr",
               "gbbr",
               "rrbgbr",
               "ubwu",
               "bwurrg",
               "brgr",
               "bbrgwb",

           }),
           6);
}

TEST_CASE("Example Part Two") {
  CHECK_EQ(solve_part_two({
               "r, wr, b, g, bwu, rb, gb, br",
               "",
               "brwrr",
               "bggr",
               "gbbr",
               "rrbgbr",
               "ubwu",
               "bwurrg",
               "brgr",
               "bbrgwb",

           }),
           16);
}
