
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

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
  unsigned int times;
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

    bool digit_in_range = false;
    assert(digit_in_range);
    return {-1, -1};
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
    bool direction_in_range = false;
    assert(direction_in_range);
    return {-1, -1};
  }

  std::vector<Press> press_horizontal_first(Position const &from,
                                            Position const &to,
                                            Position const &forbidden) const {
    Position diff = to - from;
    char horizontal = (diff.j < 0) ? '<' : '>';
    char vertical = (diff.i < 0) ? '^' : 'v';
    std::vector<Press> res;
    bool horizontal_first = (from + Position(0, diff.j)) != forbidden;
    if (diff.j != 0 && horizontal_first) {
      res.push_back(
          Press{horizontal, static_cast<unsigned int>(::abs(diff.j))});
    }
    if (diff.i != 0) {
      res.push_back(Press{vertical, static_cast<unsigned int>(::abs(diff.i))});
    }
    if (diff.j != 0 && !horizontal_first) {
      res.push_back(
          Press{horizontal, static_cast<unsigned int>(::abs(diff.j))});
    }
    res.push_back(Press::ACTIVATE);
    return res;
  }

  std::vector<Press> press_vertical_first(Position const &from,
                                          Position const &to,
                                          Position const &forbidden) const {
    Position diff = to - from;
    char horizontal = (diff.j < 0) ? '<' : '>';
    char vertical = (diff.i < 0) ? '^' : 'v';
    std::vector<Press> res;
    bool vertical_first = (from + Position(diff.i, 0)) != forbidden;
    if (diff.i != 0 && vertical_first) {
      res.push_back(Press{vertical, static_cast<unsigned int>(::abs(diff.i))});
    }
    if (diff.j != 0) {
      res.push_back(
          Press{horizontal, static_cast<unsigned int>(::abs(diff.j))});
    }
    if (diff.i != 0 && !vertical_first) {
      res.push_back(Press{vertical, static_cast<unsigned int>(::abs(diff.i))});
    }
    res.push_back(Press::ACTIVATE);
    return res;
  }
};

NumericKeypad NUMERIC_KEYPAD{};
DirectionalKeypad DIRECTIONAL_KEYPAD{};

class TypingRobot {
public:
  virtual std::vector<Press> enter(char c) = 0;
  std::vector<Press> enter(std::string const &s) {
    std::vector<Press> res;
    for (char c : s)
      res.append_range(this->enter(c));
    return res;
  }
};

struct GuidingRobot : public TypingRobot {
public:
  GuidingRobot(Position p, bool vf) : numeric_robot(p), vertical_first(vf) {}
  virtual std::vector<Press> enter(char c) override {
    Position to = NUMERIC_KEYPAD[c];
    Position forbidden = Position{3, 0};
    std::vector<Press> res =
        vertical_first ? DIRECTIONAL_KEYPAD.press_vertical_first(numeric_robot,
                                                                 to, forbidden)
                       : DIRECTIONAL_KEYPAD.press_horizontal_first(
                             numeric_robot, to, forbidden);
    numeric_robot = to;
    return res;
  }
  virtual ~GuidingRobot() {}

private:
  Position numeric_robot;
  bool vertical_first;
};

struct DelegatingRobot : public TypingRobot {
public:
  DelegatingRobot(std::shared_ptr<TypingRobot> d, Position const &p, bool vf)
      : delegate(d), delegate_position(p), vertical_first(vf) {}
  virtual std::vector<Press> enter(char c) override {
    std::vector<Press> delegate_press = delegate->enter(c);
    std::vector<Press> res;
    assert(delegate_position == Position(0, 2));
    for (auto const &p : delegate_press) {
      Position to = DIRECTIONAL_KEYPAD[p.key];
      Position forbidden = Position{0, 0};
      for (size_t i = 0; i < p.times; i++) {

        res.append_range(vertical_first
                             ? DIRECTIONAL_KEYPAD.press_vertical_first(
                                   delegate_position, to, forbidden)
                             : DIRECTIONAL_KEYPAD.press_horizontal_first(
                                   delegate_position, to, forbidden));
        delegate_position = to;
      }
    }
    assert(delegate_position == Position(0, 2));
    return res;
  }
  virtual ~DelegatingRobot() {}

private:
  std::shared_ptr<TypingRobot> delegate;
  Position delegate_position;
  bool vertical_first;
};

unsigned int press_count(std::vector<Press> presses) {
  unsigned int res = 0;
  for (auto const &p : presses)
    res += p.times;
  return res;
}

std::shared_ptr<TypingRobot>
make_typing_stack(unsigned int indirections, Position const &digicodePosition) {
  std::shared_ptr<TypingRobot> guiding =
      std::make_shared<GuidingRobot>(digicodePosition, false);
  for (size_t i = 0; i < indirections; i++) {
    guiding = std::make_shared<DelegatingRobot>(guiding, Position{0, 2}, false);
  }
  return guiding;
}

std::vector<std::shared_ptr<TypingRobot>>
make_typing_stacks(unsigned int indirections,
                   Position const &digicodePosition) {
  std::vector<std::shared_ptr<TypingRobot>> res{
      std::make_shared<GuidingRobot>(digicodePosition, false),
      std::make_shared<GuidingRobot>(digicodePosition, true),
  };
  for (size_t i = 0; i < indirections; i++) {
    std::vector<std::shared_ptr<TypingRobot>> next;
    for (const auto &prev : res) {
      next.push_back(
          std::make_shared<DelegatingRobot>(prev, Position{0, 2}, false));
      next.push_back(
          std::make_shared<DelegatingRobot>(prev, Position{0, 2}, true));
    }
    res = next;
  }
  return res;
}

unsigned int digit_press_count(Position const &digicodePosition, char c) {
  std::shared_ptr<TypingRobot> myself = make_typing_stack(2, digicodePosition);
  return press_count(myself->enter(c));
}

unsigned int solve_part_one(std::vector<std::string> const &lines) {
  unsigned int res = 0;

  for (auto const &line : lines) {
    unsigned int number = stoui(line.substr(0, line.size() - 1));
    unsigned int presses = 0;
    Position digicodePosition = Position{3, 2};
    for (char c : line) {
      presses += digit_press_count(digicodePosition, c);
      digicodePosition = NUMERIC_KEYPAD[c];
    }
    res += presses * number;
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
  CHECK_EQ(solve_part_one({
               "029A",
               "980A",
               "179A",
               "456A",
               "379A",
           }),
           126384);
}

TEST_CASE("Example Sequence") {
  Position digicodeA = Position{3, 2};
  CHECK_EQ(press_count(make_typing_stack(2, digicodeA)->enter("029A")), 68);
  CHECK_EQ(press_count(make_typing_stack(2, digicodeA)->enter("980A")), 60);
  CHECK_EQ(press_count(make_typing_stack(2, digicodeA)->enter("179A")), 68);
  CHECK_EQ(press_count(make_typing_stack(2, digicodeA)->enter("456A")), 64);
  CHECK_EQ(press_count(make_typing_stack(2, digicodeA)->enter("379A")), 64);
}

TEST_CASE("DIRECTIONAL_KEYPAD") {
  CHECK_EQ(DIRECTIONAL_KEYPAD['<'], Position(1, 0));
  CHECK_EQ(DIRECTIONAL_KEYPAD.press_horizontal_first(
               Position(0, 2), Position(1, 0), Position(0, 0)),
           std::vector{Press{'v', 1}, Press{'<', 2}, Press::ACTIVATE});
}

TEST_CASE("Example Part Two") {
  [[maybe_unused]] unsigned int REPLACE_WHEN_STARTING_PART_TWO = 24;
  CHECK_EQ(solve_part_two({}), REPLACE_WHEN_STARTING_PART_TWO);
}
