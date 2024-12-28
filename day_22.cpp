
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <algorithm>
#include <iostream>
#include <optional>
#include <ranges>

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

struct Stonk {
  static std::vector<Stonk> make(number seed);
  friend std::ostream &operator<<(std::ostream &os, Stonk const &stonk) {
    os << "{" << stonk.price << ", (" << stonk.change << ")}";
    return os;
  }
  bool operator==(Stonk const &other) const {
    return price == other.price && change == other.change;
  }

  number price;
  number change;
};

std::vector<Stonk> Stonk::make(number seed) {
  std::vector<Stonk> res;
  res.reserve(2000);
  for (size_t i = 0; i < 2000; i++) {
    number next_secret = next(seed);
    res.push_back({next_secret % 10, (next_secret % 10) - (seed % 10)});
    seed = next_secret;
  }
  return res;
}

bool stonk_changes_equals(Stonk const &stonk, number const &change) {
  return stonk.change == change;
}

std::optional<number>
search_changes_subsequence(std::vector<Stonk> const &stonks,
                           std::vector<number> const &changes) {
  auto search = std::search(stonks.begin(), stonks.end(), changes.begin(),
                            changes.end(), stonk_changes_equals);
  if (search == stonks.end())
    return {};
  return (search + (changes.size() - 1))->price;
}

number score_for_pattern(std::vector<std::vector<Stonk>> const &merchants,
                         std::vector<number> const &pattern) {
  number res = 0;
  for (const auto &stonks : merchants) {
    res += search_changes_subsequence(stonks, pattern).value_or(0);
  }
  return res;
}

number sum_range(auto const &r) {
  number res = 0;
  for (number n : r)
    res += n;
  return res;
}

bool valid_pattern(std::vector<number> const &pattern) {
  for (size_t i = 2; i < pattern.size(); i++) {
    auto sums = std::ranges::slide_view(pattern, i) |
                std::ranges::views::transform(
                    [](auto const &r) { return sum_range(r); });
    if (std::ranges::any_of(sums,
                            [](number sum) { return sum < -18 || sum > 18; }))
      return false;
  }
  return true;
}

number solve_part_two(std::vector<std::string> const &lines) {
  std::vector<std::vector<Stonk>> merchants;
  for (const auto &line : lines) {
    merchants.push_back(Stonk::make(std::stoull(line)));
  }

  number res = 0;
  printf("Search start\n");
  for (number a = -9; a <= 9; a++) {
    printf("\ta = %lld\n", a);
    for (number b = -9; b <= 9; b++) {
      printf("\t\tb = %lld\n", b);
      for (number c = -9; c <= 9; c++) {
        for (number d = -9; d <= 9; d++) {
          std::vector<number> pattern{a, b, c, d};
          if (!valid_pattern(pattern))
            continue;
          res = std::max(res, score_for_pattern(merchants, {a, b, c, d}));
        }
      }
    }
  }

  return res;
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
  CHECK_EQ(solve_part_two({
               "1",
               "2",
               "3",
               "2024",
           }),
           23);
}

TEST_CASE("Stonk") {
  [[maybe_unused]] std::vector<Stonk> stonks = Stonk::make(123);
  CHECK_EQ(stonks[0], Stonk{0, -3});
  CHECK_EQ(stonks[1], Stonk{6, 6});
  CHECK_EQ(stonks[2], Stonk{5, -1});
}

TEST_CASE("Search change pattern") {
  [[maybe_unused]] std::vector<Stonk> stonks = Stonk::make(123);
  [[maybe_unused]] std::vector<number> needle{-1, -1, 0, 2};
  CHECK_EQ(search_changes_subsequence(stonks, needle), 6);
}
