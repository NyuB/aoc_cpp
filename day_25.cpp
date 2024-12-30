
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <array>
#include <iostream>

#include "utils.hpp"

const size_t BARS_COUNT = 5;
const unsigned int MAX_BAR_VALUE = 5;

struct Bars {
  std::array<unsigned int, BARS_COUNT> bars;
  bool overlap(Bars const &other) const {
    for (unsigned int i = 0; i < BARS_COUNT; i++) {
      if (bars[i] + other.bars[i] > MAX_BAR_VALUE)
        return true;
    }
    return false;
  }
  bool operator==(Bars const &other) const {
    for (size_t i = 0; i < bars.size(); i++) {
      if (bars[i] != other.bars[i])
        return false;
    }
    return true;
  }
  friend std::ostream &operator<<(std::ostream &os, Bars const &b) {
    os << "[";
    for (auto i : b.bars) {
      os << " " << i;
    }
    os << " ]";
    return os;
  }
};

struct Key : public Bars {};
static_assert(sizeof(Key) == sizeof(Bars));
struct Lock : public Bars {};
static_assert(sizeof(Lock) == sizeof(Bars));

Bars parse_bars(std::vector<std::string> const &lines, unsigned int start) {
  Bars result;
  std::fill(result.bars.begin(), result.bars.end(), 0);
  for (size_t j = 0; j < result.bars.size(); j++) {
    for (size_t i = start + 1; i <= start + MAX_BAR_VALUE; i++) {
      if (lines[i][j] == '#')
        result.bars[j]++;
    }
  }
  return result;
}

Key parse_key(std::vector<std::string> const &lines, unsigned int start) {
  return static_cast<Key>(parse_bars(lines, start));
}

Lock parse_lock(std::vector<std::string> const &lines, unsigned int start) {
  return static_cast<Lock>(parse_bars(lines, start));
}

struct Schematics {
  std::vector<Lock> locks;
  std::vector<Key> keys;
};

Schematics parse_schematics(std::vector<std::string> const &lines) {
  Schematics res;
  for (size_t i = 0; i < lines.size(); i += MAX_BAR_VALUE + 3) {
    if (lines[i].starts_with('#'))
      res.locks.push_back(parse_lock(lines, i));
    if (lines[i].starts_with('.'))
      res.keys.push_back(parse_key(lines, i));
  }
  return res;
}

unsigned int solve_part_one(std::vector<std::string> const &lines) {
  Schematics sh = parse_schematics(lines);
  unsigned int res = 0;
  for(auto const& lock: sh.locks) {
    for(auto const& key: sh.keys) {
      if(!lock.overlap(key)) res++;
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
    unsigned int res = solve_part_one(read_input_file(filename));
    std::cout << res << std::endl;
  } else if (part == "2") {
    unsigned int res = solve_part_two(read_input_file(filename));
    std::cout << res << std::endl;
  }
}
#endif

#include "doctest/doctest.h"

#ifndef DOCTEST_CONFIG_DISABLE
// You can add test helpers such as doctest ostream << overrides here
#endif

TEST_CASE("Example Part One") {
  CHECK_EQ(
      solve_part_one({
          "#####", ".####", ".####", ".####", ".#.#.", ".#...", ".....", "",
          "#####", "##.##", ".#.##", "...##", "...#.", "...#.", ".....", "",
          ".....", "#....", "#....", "#...#", "#.#.#", "#.###", "#####", "",
          ".....", ".....", "#.#..", "###..", "###.#", "###.#", "#####", "",
          ".....", ".....", ".....", "#....", "#.#..", "#.#.#", "#####",

      }),
      3);
}

TEST_CASE("Example Part Two") {
  [[maybe_unused]] unsigned int REPLACE_WHEN_STARTING_PART_TWO = 24;
  CHECK_EQ(solve_part_two({}), REPLACE_WHEN_STARTING_PART_TWO);
}

TEST_CASE("Parse bars") {
  CHECK_EQ(parse_key(
               {
                   ".....",
                   "#....",
                   "#....",
                   "#...#",
                   "#.#.#",
                   "#.###",
                   "#####",
               },
               0),
           Key{{5, 0, 2, 1, 3}});
  CHECK_EQ(parse_lock(
               {
                   "#####",
                   "##.##",
                   ".#.##",
                   "...##",
                   "...#.",
                   "...#.",
                   ".....",
               },
               0),
           Lock{{1, 2, 0, 5, 3}});
}
