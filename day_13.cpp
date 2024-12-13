
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <iostream>
#include <limits>
#include <numeric>
#include <optional>
#include <regex>

#include "utils.hpp"

using number = long long;

struct Vector {
  static Vector parse_button(std::string const &line);
  static Vector parse_prize(std::string const &line);

  bool can_reach(Vector const &from, Vector const &target) const {
    number dx = target.x - from.x;
    number dy = target.y - from.y;
    return (dx / x == dy / y) && (dx % x == 0) && (dy % y == 0);
  }

  Vector operator*(number factor) const {
    return Vector{x * factor, y * factor};
  }

  Vector operator+(Vector const &other) const {
    return Vector{x + other.x, y + other.y};
  }

  Vector operator-() const { return Vector{-x, -y}; }

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

struct Line {
  Line(Vector const &origin_, Vector const &direction_)
      : origin(origin_), direction(direction_) {
    number gcd = std::gcd(direction.x, direction.y);
    direction.x /= gcd;
    direction.y /= gcd;
  }
  static bool parallel(Line const &la, Line const &lb);
  static std::optional<Vector> intersection(Line const &a, Line const &b);
  Vector origin;
  Vector direction;
};

bool Line::parallel(Line const &a, Line const &b) {
  return a.direction == b.direction || a.direction == (-b.direction);
}

/**
 * @return the intersection of `la` and `lb` if it has discrete coordinates,
 * empty otherwise
 */
std::optional<Vector> Line::intersection(Line const &la, Line const &lb) {
  // https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection#Given_two_points_on_each_line
  Vector vla = (la.origin + la.direction);
  Vector vlb = (lb.origin + lb.direction);

  number x1 = la.origin.x;
  number y1 = la.origin.y;

  number x2 = vla.x;
  number y2 = vla.y;

  number x3 = lb.origin.x;
  number y3 = lb.origin.y;

  number x4 = vlb.x;
  number y4 = vlb.y;

  number px_num =
      (x1 * y2 - y1 * x2) * (x3 - x4) - (x1 - x2) * (x3 * y4 - y3 * x4);
  number py_num =
      (x1 * y2 - y1 * x2) * (y3 - y4) - (y1 - y2) * (x3 * y4 - y3 * x4);

  number det = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);

  if (px_num % det != 0 || py_num % det != 0)
    return {};
  return Vector{px_num / det, py_num / det};
}

struct Machine {
  static std::vector<Machine>
  parse_machines(std::vector<std::string> const &lines);

  static std::vector<Machine>
  parse_big_machines(std::vector<std::string> const &lines);

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

std::vector<Machine>
Machine::parse_big_machines(std::vector<std::string> const &lines) {
  std::vector<Machine> result;
  Vector big_offset{10000000000000, 10000000000000};
  for (unsigned int i = 0; i < lines.size(); i += 4) {
    result.push_back(Machine{
        Vector::parse_button(lines[i]),
        Vector::parse_button(lines[i + 1]),
        Vector::parse_prize(lines[i + 2]) + big_offset,
    });
  }
  return result;
}

number min_cost(Machine const &machine) {
  // See the solution as composed of B * the b button vector from the origin to
  // an inflexion point, then A * the A vector button to the prize Compute the
  // inflexion point a the intersection of these two lines Then check if this
  // point exists an can be reached from origin with a discrete number of B, and
  // if prize can be reached from this point with an integer number of A

  Line b_line{{0, 0}, machine.b};
  Line a_line{machine.prize, -machine.a};
  std::optional<Vector> cross = Line::intersection(b_line, a_line);
  if (!cross.has_value()) {
    return 0;
  }
  if (machine.b.can_reach({0, 0}, cross.value()) &&
      machine.a.can_reach(cross.value(), machine.prize)) {
    number na = (machine.prize.x - cross.value().x) / machine.a.x;
    number nb = (cross.value().x) / machine.b.x;
    return na * 3 + nb;
  }
  return 0;
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
  std::vector<Machine> machines = Machine::parse_big_machines(lines);
  number res = 0;
  for (const auto &machine : machines) {
    res += min_cost(machine);
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
std::ostream &operator<<(std::ostream &os, Vector const &v) {
  os << "(" << v.x << ", " << v.y << ")";
  return os;
}

std::ostream &operator<<(std::ostream &os, std::optional<Vector> const &v) {
  if (v.has_value()) {
    os << v.value();

  } else {
    os << "None";
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, Line const &l) {
  os << "Origin:" << l.origin << " | Direction: " << l.direction;
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

TEST_CASE("Line intersect") {
  Line la({0, 0}, {1, 4});
  Line lb({1, 0}, {1, 8});
  CHECK_EQ(Line::intersection(la, lb), std::optional<Vector>{Vector{2, 8}});
  CHECK_EQ(Line::intersection(lb, la), std::optional<Vector>{Vector{2, 8}});
  Line exa{{8400, 5400}, {-94, -34}};
  Line exb{{0, 0}, {22, 67}};
  CHECK_EQ(Line::intersection(exa, exb), std::optional<Vector>({880, 2680}));
}

TEST_CASE("Single machine example") {
  CHECK_EQ(solve_part_one({"Button A: X+94, Y+34", "Button B: X+22, Y+67",
                           "Prize: X=8400, Y=5400"}),
           280);
}
