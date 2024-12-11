
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <iostream>
#include <map>

#include "utils.hpp"

using number = unsigned long long;

number digits(number stone) {
  if (stone == 0)
    return 1;
  unsigned res = 0;
  while (stone > 0) {
    stone /= 10;
    res++;
  }
  return res;
}

bool even_digits(number stone) { return digits(stone) % 2 == 0; }

std::vector<number> blink(number stone) {
  if (stone == 0)
    return std::vector<number>{1};

  number d = digits(stone);
  if (d % 2 != 0)
    return std::vector<number>{stone * 2024};

  number left = 0;
  number right = 0;
  number power = 1;
  for (number i = 0; i < d / 2; i++) {
    right += (stone % 10) * power;
    power *= 10;
    stone /= 10;
  }
  power = 1;
  for (number i = 0; i < d / 2; i++) {
    left += (stone % 10) * power;
    power *= 10;
    stone /= 10;
  }
  return std::vector<number>{left, right};
}

struct StoneKey {
  bool operator<(StoneKey const &other) const {
    if (stone == other.stone)
      return blinks < other.blinks;
    return stone < other.stone;
  }
  number stone;
  size_t blinks;
};

number count_after_n_blinks(number stone, size_t blinks,
                            std::map<StoneKey, number> *memo) {
  if (blinks == 0)
    return 1;
  StoneKey k{stone, blinks};
  if (memo->find(k) != memo->end())
    return memo->at(k);
  std::vector<number> blinked = blink(stone);
  number res = 0;
  for (number s : blinked) {
    res += count_after_n_blinks(s, blinks - 1, memo);
  }
  memo->emplace(k, res);
  return res;
}

number solve(std::vector<number> stones, unsigned int n) {
  std::map<StoneKey, number> memo;
  number res = 0;
  for (number s : stones) {
    res += count_after_n_blinks(s, n, &memo);
  }
  return res;
}

number solve_part_one(std::vector<std::string> const &lines) {
  std::vector<number> stones = llongs(lines[0]);
  return solve(stones, 25);
}

number solve_part_two(std::vector<std::string> const &lines) {
  std::vector<number> stones = llongs(lines[0]);
  return solve(stones, 75);
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

TEST_CASE("Example Part One") { CHECK_EQ(solve_part_one({"125 17"}), 55312); }
TEST_CASE("Example Part Two") {
  CHECK_EQ(solve_part_two({"125 17"}), 65601038650482);
}
