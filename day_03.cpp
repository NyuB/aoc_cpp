
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <iostream>
#include <iterator>
#include <regex>
#include <string>

#include "utils.hpp"

unsigned long mul_sum(std::string const &input) {
  std::regex r("mul\\((\\d+),(\\d+)\\)");
  auto matches_start = std::sregex_iterator(input.begin(), input.end(), r);
  auto matches_end = std::sregex_iterator();
  unsigned long res = 0;
  for (auto it = matches_start; it != matches_end; it++) {
    std::smatch match = *it;
    unsigned long left = std::stoul(match[1]);
    unsigned long right = std::stoul(match[2]);
    res += left * right;
  }
  return res;
}

unsigned long solve_part_one(std::vector<std::string> const &lines) {
  std::string full = "";
  for (auto l : lines) {
    full += l;
  }
  return mul_sum(full);
}

unsigned long solve_part_two(std::vector<std::string> const &lines) {
  return 24;
}

#ifdef DOCTEST_CONFIG_DISABLE
int main(int argc, char *argv[]) {
  std::string filename = argv[2];
  std::string part = argv[1];
  if (part == "1") {
    unsigned long res = solve_part_one(read_input_file(filename));
    std::cout << res << std::endl;
  } else if (part == "2") {
    unsigned long res = solve_part_two(read_input_file(filename));
    std::cout << res << std::endl;
  }
}
#endif

#include "doctest/doctest.h"

TEST_CASE("Example Part One") {
  CHECK_EQ(solve_part_one({"xmul(2,4)%&mul[3,7]!@^do_not_mul(5,5)+mul(32,64]"
                           "then(mul(11,8)mul(8,5))"}),
           161);
}