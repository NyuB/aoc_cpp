
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <iostream>

#include "utils.hpp"

using number = long long;

inline number mix(number value, number secret) { return value ^ secret; }

inline number prune(number n) { return n % 16777216llu; }

inline number next(number secret) {
  number step_1 = prune(mix(secret * 64, secret));
  number step_2 = mix(step_1 / 32, step_1);
  number step_3 = prune(mix(2048 * step_2, step_2));
  return step_3;
}

number nth(number seed, unsigned int n) {
  for (size_t i = 0; i < n; i++) {
    seed = next(seed);
  }
  return seed;
}

number solve_part_one(std::vector<std::string> const &lines) {
  number res = 0;
  for (const auto &line : lines) {
    number seed = std::stoull(line);
    res += nth(seed, 2000);
  }
  return res;
}

number solve_part_two(std::vector<std::string> const &lines) {
  (void)lines;
  return 24;
}

#ifdef DOCTEST_CONFIG_DISABLE
int main(int _, char *argv[]) {
  std::string filename = argv[2];
  std::string part = argv[1];
  if (part == "1") {
    number res = solve_part_one(read_input_file(filename));
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
               "1",
               "10",
               "100",
               "2024",
           }),
           37327623);
}

TEST_CASE("Single numbers examples") {
  CHECK_EQ(nth(1, 2000), 8685429);

  CHECK_EQ(nth(123, 1), 15887950);
  CHECK_EQ(nth(123, 2), 16495136);
  CHECK_EQ(nth(123, 3), 527345);
  CHECK_EQ(nth(123, 4), 704524);
  CHECK_EQ(nth(123, 5), 1553684);
  CHECK_EQ(nth(123, 6), 12683156);
  CHECK_EQ(nth(123, 7), 11100544);
  CHECK_EQ(nth(123, 8), 12249484);
  CHECK_EQ(nth(123, 9), 7753432);
  CHECK_EQ(nth(123, 10), 5908254);
}

TEST_CASE("prune") { CHECK_EQ(prune(100000000), 16113920); }
TEST_CASE("mix") { CHECK_EQ(mix(15, 42), 37); }

TEST_CASE("Example Part Two") {
  [[maybe_unused]] number REPLACE_WHEN_STARTING_PART_TWO = 24;
  CHECK_EQ(solve_part_two({}), REPLACE_WHEN_STARTING_PART_TWO);
}
