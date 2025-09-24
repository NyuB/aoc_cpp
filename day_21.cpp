
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#endif
#include <format>
#include <limits>
#include <map>
#include <stdexcept>

#include <cassert>
#include <iostream>

#include "utils.hpp"

using Position = aoc::grid::Position;

struct Press {
  static Press ACTIVATE;
  bool operator==(Press const &other) const {
    return key == other.key && times == other.times;
  }
  friend std::ostream &operator<<(std::ostream &os, Press const &value) {
    os << "{'" << value.key << "'(" << value.times << ")}";
    return os;
  }
  friend std::ostream &operator<<(std::ostream &os,
                                  std::vector<Press> const &values) {
    os << "['";
    for (auto const &value : values) {
      for (size_t i = 0; i < value.times; i++) {
        os << value.key;
      }
    }
    os << "']";
    return os;
  }
  char key;
  unsigned long long times;
};

Press Press::ACTIVATE{'A', 1};

struct NumericKeypad {
  inline Position operator[](char key) const { return get(key); }
  Position get(char key) const {
    if (key == '7')
      return {0, 0};
    if (key == '8')
      return {0, 1};
    if (key == '9')
      return {0, 2};

    if (key == '4')
      return {1, 0};
    if (key == '5')
      return {1, 1};
    if (key == '6')
      return {1, 2};

    if (key == '1')
      return {2, 0};
    if (key == '2')
      return {2, 1};
    if (key == '3')
      return {2, 2};

    if (key == '0')
      return {3, 1};
    if (key == 'A')
      return {3, 2};

    throw std::invalid_argument(std::string{"Invalid key: "} + key);
  }
};

struct PressOptions {
  std::vector<std::vector<Press>> options;
};

struct DirectionalKeypad {
  inline Position operator[](char key) const { return get(key); }
  Position get(char key) const {
    if (key == '^')
      return {0, 1};
    if (key == 'A')
      return {0, 2};
    if (key == '<')
      return {1, 0};
    if (key == 'v')
      return {1, 1};
    if (key == '>')
      return {1, 2};
    throw std::invalid_argument(std::string{"Invalid key: "} + key);
  }

  std::vector<std::vector<Press>> press(Position const &from,
                                        Position const &to,
                                        Position const &forbidden) const {
    Position diff = to - from;
    char horizontal = (diff.j < 0) ? '<' : '>';
    char vertical = (diff.i < 0) ? '^' : 'v';
    bool can_vertical_first = (from + Position(diff.i, 0)) != forbidden;
    bool can_horizontal_first = (from + Position(0, diff.j)) != forbidden;
    if (diff.i == 0 && diff.j == 0) {
      return {{Press::ACTIVATE}};
    }

    if (diff.j == 0) {
      return {{Press{vertical, static_cast<unsigned long long>(::abs(diff.i))},
               Press::ACTIVATE}};
    }

    if (diff.i == 0) {
      return {
          {Press{horizontal, static_cast<unsigned long long>(::abs(diff.j))},
           Press::ACTIVATE}};
    }

    std::vector<std::vector<Press>> res{};

    if (can_vertical_first) {
      res.push_back({
          Press{vertical, static_cast<unsigned long long>(::abs(diff.i))},
          Press{horizontal, static_cast<unsigned long long>(::abs(diff.j))},
          Press::ACTIVATE,
      });
    }

    if (can_horizontal_first) {
      res.push_back({
          Press{horizontal, static_cast<unsigned long long>(::abs(diff.j))},
          Press{vertical, static_cast<unsigned long long>(::abs(diff.i))},
          Press::ACTIVATE,
      });
    }
    return res;
  }
};

NumericKeypad NUMERIC_KEYPAD{};
DirectionalKeypad DIRECTIONAL_KEYPAD{};

std::map<std::string, unsigned long long> memo{};
std::string expand(std::vector<Press> const &moves) {
  std::string res = "";
  for (auto const &move : moves) {
    for (unsigned int i = 0; i < move.times; i++) {
      res += move.key;
    }
  }
  return res;
}

std::string hash(std::string const &moves, unsigned int robots) {
  return std::format("{}|{}", moves, robots);
}

unsigned long long shortest_sequence_through_robots(std::string const &moves,
                                                    unsigned int robots) {
  auto h = hash(moves, robots);
  if (memo.find(h) != memo.end()) {
    return memo[h];
  }
  unsigned long long res;
  if (robots == 0) {
    res = moves.length();
  } else {
    char current_move = 'A';
    unsigned long long total = 0L;
    for (const auto m : moves) {
      auto press_options =
          DIRECTIONAL_KEYPAD.press(DIRECTIONAL_KEYPAD.get(current_move),
                                   DIRECTIONAL_KEYPAD.get(m), {0, 0});
      unsigned long long best_solution =
          std::numeric_limits<unsigned long long>::max();
      for (const auto &branch : press_options) {
        best_solution = std::min(
            shortest_sequence_through_robots(expand(branch), robots - 1),
            best_solution);
      }
      current_move = m;
      total += best_solution;
    }
    res = total;
  }

  memo[h] = res;
  return res;
}

unsigned long long click_through_robots(char from_digit, char to_digit,
                                        unsigned int nb_robots) {
  auto moveLists = DIRECTIONAL_KEYPAD.press(
      NUMERIC_KEYPAD.get(from_digit), NUMERIC_KEYPAD.get(to_digit), {3, 0});
  unsigned long long best = std::numeric_limits<unsigned long long>::max();
  for (const auto &moves : moveLists) {
    best = std::min(best,
                    shortest_sequence_through_robots(expand(moves), nb_robots));
  }
  return best;
}

unsigned long long click_code_through_robots(unsigned int nb_robots,
                                             std::string const &code) {
  char current = 'A';
  unsigned long long res = 0;
  for (const auto digit : code) {
    res += click_through_robots(current, digit, nb_robots);
    current = digit;
  }
  return res;
}

unsigned long long solve(std::vector<std::string> const &lines,
                         unsigned int nb_robots) {
  unsigned long long res = 0;

  for (auto const &line : lines) {
    unsigned long long number = stoui(line.substr(0, line.size() - 1));
    unsigned long long presses = click_code_through_robots(nb_robots, line);
    res += presses * number;
  }
  return res;
}

unsigned long long solve_part_one(std::vector<std::string> const &lines) {
  return solve(lines, 2);
}

unsigned long long solve_part_two(std::vector<std::string> const &lines) {
  return solve(lines, 25);
}

#ifdef DOCTEST_CONFIG_DISABLE
int main(int _, char *argv[]) {
  std::string filename = argv[2];
  std::string part = argv[1];
  if (part == "1") {
    unsigned long long res = solve_part_one(read_input_file(filename));
    std::cout << res << std::endl;
  } else if (part == "2") {
    unsigned long long res = solve_part_two(read_input_file(filename));
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
               "029A",
               "980A",
               "179A",
               "456A",
               "379A",
           }),
           126384);
}
