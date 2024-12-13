
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <iostream>
#include <limits>
#include <optional>
#include <regex>

#include "utils.hpp"

using number = unsigned long long;

struct Vector {
  static Vector parse_button(std::string const &line);
  static Vector parse_prize(std::string const &line);

  Vector operator*(number factor) const {
    return Vector{x * factor, y * factor};
  }

  Vector operator+(Vector const &other) const {
    return Vector{x + other.x, y + other.y};
  }

  bool operator==(Vector const &other) const {
    return x == other.x && y == other.y;
  }

  bool operator<(Vector const &other) const {
    if (x == other.x)
      return y < other.y;
    return x < other.x;
  }

  number x;
  number y;
};

struct Machine {
  static std::vector<Machine>
  parse_machines(std::vector<std::string> const &lines);

  bool operator==(Machine const &other) const {
    return a == other.a && b == other.b && prize == other.prize;
  }
  Vector a;
  Vector b;
  Vector prize;
};

Vector Vector::parse_button(std::string const &line) {
  std::regex r("X[+](\\d+), Y[+](\\d+)$");
  std::smatch match = *(std::sregex_iterator(line.begin(), line.end(), r));
  number x = std::stoul(match[1]);
  number y = std::stoul(match[2]);
  return Vector{x, y};
}

Vector Vector::parse_prize(std::string const &line) {
  std::regex r("X=(\\d+), Y=(\\d+)$");
  std::smatch match = *(std::sregex_iterator(line.begin(), line.end(), r));
  number x = std::stoul(match[1]);
  number y = std::stoul(match[2]);
  return Vector{x, y};
};

std::vector<Machine>
Machine::parse_machines(std::vector<std::string> const &lines) {
  std::vector<Machine> result;
  for (unsigned int i = 0; i < lines.size(); i += 4) {
    result.push_back(Machine{
        Vector::parse_button(lines[i]),
        Vector::parse_button(lines[i + 1]),
        Vector::parse_prize(lines[i + 2]),
    });
  }
  return result;
}

number min_cost(Machine const &machine) {
  std::optional<number> res{};
  for (number use_a = 0; use_a <= 100; use_a++) {
    for (number use_b = 0; use_b <= 100; use_b++) {
      Vector position = (machine.a * use_a) + (machine.b * use_b);
      number cost = use_a * 3 + use_b;
      if (position == machine.prize) {
        if (!res.has_value()) {
          res = std::optional(cost);
        } else {
          res = std::optional(std::min(res.value(), cost));
        }
      }
    }
  }
  return res.value_or(0);
}

number solve_part_one(std::vector<std::string> const &lines) {
  std::vector<Machine> machines = Machine::parse_machines(lines);
  number res = 0;
  for (const auto &machine : machines) {
    res += min_cost(machine);
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
std::ostream &operator<<(std::ostream &os, Vector const &v) {
  os << "(" << v.x << ", " << v.y << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, Machine const &m) {
  os << "A: " << m.a << " | B: " << m.b << "| Prize: " << m.prize;
  return os;
}

std::ostream &operator<<(std::ostream &os,
                         std::vector<Machine> const &machines) {
  os << "[" << std::endl;
  for (const auto &machine : machines) {
    os << "\t" << machine << std::endl;
  }
  os << "]";
  return os;
}
#endif

TEST_CASE("Example Part One") {
  CHECK_EQ(solve_part_one({
               "Button A: X+94, Y+34",
               "Button B: X+22, Y+67",
               "Prize: X=8400, Y=5400",
               "",
               "Button A: X+26, Y+66",
               "Button B: X+67, Y+21",
               "Prize: X=12748, Y=12176",
               "",
               "Button A: X+17, Y+86",
               "Button B: X+84, Y+37",
               "Prize: X=7870, Y=6450",
               "",
               "Button A: X+69, Y+23",
               "Button B: X+27, Y+71",
               "Prize: X=18641, Y=10279",
           }),
           480);
}

TEST_CASE("Parse button vector") {
  CHECK_EQ(Vector::parse_button("Button A: X+94, Y+34"), Vector{94, 34});
}

TEST_CASE("Parse button vector") {
  CHECK_EQ(Vector::parse_prize("Prize: X=18641, Y=10279"),
           Vector{18641, 10279});
}

TEST_CASE("Parse machines") {
  CHECK_EQ(Machine::parse_machines({
               "Button A: X+94, Y+34",
               "Button B: X+22, Y+67",
               "Prize: X=8400, Y=5400",
               "",
               "Button A: X+26, Y+66",
               "Button B: X+67, Y+21",
               "Prize: X=12748, Y=12176",
               "",
               "Button A: X+17, Y+86",
               "Button B: X+84, Y+37",
               "Prize: X=7870, Y=6450",
               "",
               "Button A: X+69, Y+23",
               "Button B: X+27, Y+71",
               "Prize: X=18641, Y=10279",
           }),
           std::vector<Machine>{
               Machine{Vector{94, 34}, Vector{22, 67}, Vector{8400, 5400}},
               Machine{Vector{26, 66}, Vector{67, 21}, Vector{12748, 12176}},
               Machine{Vector{17, 86}, Vector{84, 37}, Vector{7870, 6450}},
               Machine{Vector{69, 23}, Vector{27, 71}, Vector{18641, 10279}},
           });
}
