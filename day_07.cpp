
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <iostream>

#include "utils.hpp"

using number = unsigned long long;
typedef number (*binary_operator)(number, number);

number concat(number left, number right) {
  std::string left_str = std::to_string(left);
  std::string right_str = std::to_string(right);
  return std::stoll(left_str + right_str);
}

number add(number left, number right) { return left + right; }
number mul(number left, number right) { return left * right; }

bool solvable(number target, number current, std::vector<number> const &numbers,
              size_t index, std::vector<binary_operator> const &operators) {
  if (index == numbers.size())
    return current == target;
  if (current > target)
    return false;
  number next = numbers[index];
  number next_index = index + 1;
  for (const auto &op : operators) {
    if (solvable(target, op(current, next), numbers, next_index, operators))
      return true;
  }
  return false;
}

bool solvable(number target, std::vector<number> const &numbers,
              std::vector<binary_operator> const &operators) {
  if (numbers.empty())
    return target == 0;
  return solvable(target, numbers[0], numbers, 1, operators);
}

number solve(std::vector<std::string> const &lines,
             std::vector<binary_operator> const &operators) {
  number res = 0;
  for (const auto &line : lines) {
    size_t split_index = line.find(": ");
    std::string target_str = line.substr(0, split_index);
    number target = std::stoull(target_str);
    std::string numbers_str =
        line.substr(split_index + 2, line.size() - split_index - 2);
    std::vector<number> numbers = llongs(numbers_str);
    if (solvable(target, numbers, operators))
      res += target;
  }
  return res;
}

number solve_part_one(std::vector<std::string> const &lines) {
  return solve(lines, {add, mul});
}

number solve_part_two(std::vector<std::string> const &lines) {
  return solve(lines, {add, mul, concat});
}

#ifdef DOCTEST_CONFIG_DISABLE
int main(int argc, char *argv[]) {
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

TEST_CASE("Example Part One") {
  CHECK_EQ(solve_part_one({
               "190: 10 19",
               "3267: 81 40 27",
               "83: 17 5",
               "156: 15 6",
               "7290: 6 8 6 15",
               "161011: 16 10 13",
               "192: 17 8 14",
               "21037: 9 7 18 13",
               "292: 11 6 16 20",
           }),
           3749);
}

TEST_CASE("Example Part Two") {
  CHECK_EQ(solve_part_two({
               "190: 10 19",
               "3267: 81 40 27",
               "83: 17 5",
               "156: 15 6",
               "7290: 6 8 6 15",
               "161011: 16 10 13",
               "192: 17 8 14",
               "21037: 9 7 18 13",
               "292: 11 6 16 20",
           }),
           11387);
}
