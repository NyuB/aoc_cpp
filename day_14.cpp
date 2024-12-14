
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <iostream>
#include <regex>

#include "utils.hpp"

int mod(int n, int m) { return ((n % m) + m) % m; }

struct XY {
  XY(int x_, int y_) : x(x_), y(y_) {}
  XY(unsigned int x_, unsigned int y_)
      : x(static_cast<int>(x_)), y(static_cast<int>(y_)) {}
  bool operator==(XY const &other) const {
    return x == other.x && y == other.y;
  }
  bool operator!=(XY const &other) const {
    return x != other.x || y != other.y;
  }

  bool operator<(XY const &other) const {
    if (x == other.x)
      return y < other.y;
    return x < other.x;
  }

  int x;
  int y;
};

struct Position : XY {
  Position(int x_, int y_) : XY(x_, y_) {}
  Position(unsigned int x_, unsigned int y_) : XY(x_, y_) {}
};

struct Velocity : XY {
  Velocity(int x_, int y_) : XY(x_, y_) {}
  Velocity(unsigned int x_, unsigned int y_) : XY(x_, y_) {}
  Velocity operator*(unsigned int factor) const {
    return Velocity(x * factor, y * factor);
  }
};

Position operator+(Position const &p, Velocity const &v) {
  return Position(p.x + v.x, p.y + v.y);
}

struct Robot {
  static Robot parse(std::string const &line);
  static std::vector<Robot> parse(std::vector<std::string> const &lines);
  void move(unsigned int width, unsigned int height, unsigned int times) {
    p = p + (v * times);
    p.x = mod(p.x, width);
    p.y = mod(p.y, height);
  }

  bool operator==(Robot const &other) const {
    return other.p == p && other.v == v;
  }

  bool operator!=(Robot const &other) const {
    return other.p != p || other.v != v;
  }

  Position p;
  Velocity v;
};

Robot Robot::parse(std::string const &line) {
  std::regex reg("p=(\\d+),(\\d+) v=(-?\\d+),(-?\\d+)");
  std::smatch match = *(std::sregex_iterator(line.begin(), line.end(), reg));
  int x = std::stoi(match[1]);
  int y = std::stoi(match[2]);
  int vx = std::stoi(match[3]);
  int vy = std::stoi(match[4]);
  return Robot{Position(x, y), Velocity(vx, vy)};
}

std::vector<Robot> Robot::parse(std::vector<std::string> const &lines) {
  std::vector<Robot> res;
  for (const auto &line : lines) {
    res.push_back(Robot::parse(line));
  }
  return res;
}

std::ostream &operator<<(std::ostream &os, Position const &position) {
  os << "Position { " << position.x << ", " << position.y << " }";
  return os;
}

std::ostream &operator<<(std::ostream &os, Velocity const &velocity) {
  os << "Velocity { " << velocity.x << ", " << velocity.y << " }";
  return os;
}

std::ostream &operator<<(std::ostream &os, Robot const &robot) {
  os << "Robot { " << robot.p << ", " << robot.v << " }";
  return os;
}

class Problem {
public:
  Problem(std::vector<Robot> robots_, unsigned int width_, unsigned int height_)
      : robots(robots_), width(width_), height(height_) {}
  void move(unsigned int times) {
    for (auto &robot : robots) {
      robot.move(width, height, times);
    }
  }
  bool operator==(Problem const &other) const {
    if (other.height != height)
      return false;
    if (other.width != width)
      return false;
    if (other.robots.size() != robots.size())
      return false;
    for (size_t i = 0; i < robots.size(); i++) {
      if (other.robots[i] != robots[i])
        return false;
    }
    return true;
  }
  void print() const {
    std::cout << "Problem" << std::endl;
    for (const auto &robot : robots) {
      std::cout << "\t" << robot << std::endl;
    }
  }
  unsigned int score() const {
    unsigned int up_left = 0;
    unsigned int up_right = 0;
    unsigned int down_left = 0;
    unsigned int down_right = 0;
    for (const auto &robot : robots) {
      if (static_cast<unsigned int>(robot.p.x) < width / 2 &&
          static_cast<unsigned int>(robot.p.y) < height / 2)
        up_left++;
      if (static_cast<unsigned int>(robot.p.x) > width / 2 &&
          static_cast<unsigned int>(robot.p.y) < height / 2)
        up_right++;

      if (static_cast<unsigned int>(robot.p.x) < width / 2 &&
          static_cast<unsigned int>(robot.p.y) > height / 2)
        down_left++;
      if (static_cast<unsigned int>(robot.p.x) > width / 2 &&
          static_cast<unsigned int>(robot.p.y) > height / 2)
        down_right++;
    }
    return up_left * up_right * down_left * down_right;
  }

  friend std::ostream &operator<<(std::ostream &os, Problem const &problem) {
    os << "Problem" << std::endl;
    for (const auto &robot : problem.robots) {
      os << "\t" << robot << std::endl;
    }
    return os;
  }

private:
  std::vector<Robot> robots;
  unsigned int width;
  unsigned int height;
};

unsigned int solve_part_one(std::vector<std::string> const &lines,
                            unsigned int width, unsigned int height,
                            unsigned int times) {
  Problem problem(Robot::parse(lines), width, height);
  problem.move(times);
  return problem.score();
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
    unsigned int res = solve_part_one(read_input_file(filename), 101, 103, 100);
    std::cout << res << std::endl;
  } else if (part == "2") {
    unsigned int res = solve_part_two(read_input_file(filename));
    std::cout << res << std::endl;
  }
}
#endif

#include "doctest/doctest.h"

TEST_CASE("Example Part One") {
  CHECK_EQ(solve_part_one(
               {
                   "p=0,4 v=3,-3",
                   "p=6,3 v=-1,-3",
                   "p=10,3 v=-1,2",
                   "p=2,0 v=2,-1",
                   "p=0,0 v=1,3",
                   "p=3,0 v=-2,-2",
                   "p=7,6 v=-1,-3",
                   "p=3,0 v=-1,-2",
                   "p=9,3 v=2,3",
                   "p=7,3 v=-1,2",
                   "p=2,4 v=2,-3",
                   "p=9,5 v=-3,-3",
               },
               11, 7, 100),
           12);
}

TEST_CASE("Single robot example") {
  Problem actual(Robot::parse(std::vector<std::string>{"p=2,4 v=2,-3"}), 11, 7);
  Problem expected_1_sec(Robot::parse(std::vector<std::string>{"p=4,1 v=2,-3"}),
                         11, 7);
  Problem expected_2_sec(Robot::parse(std::vector<std::string>{"p=6,5 v=2,-3"}),
                         11, 7);
  actual.move(1);
  CHECK_EQ(actual, expected_1_sec);
  actual.move(1);
  CHECK_EQ(actual, expected_2_sec);
}

TEST_CASE("Modulo") {
  CHECK_EQ(mod(-2, 7), 5);
  CHECK_EQ(mod(2, 7), 2);
  CHECK_EQ(mod(8, 7), 1);
}

TEST_CASE("Robot parsing") {
  CHECK_EQ(Robot::parse("p=0,4 v=3,-3"),
           Robot{Position(0, 4), Velocity(3, -3)});
}
