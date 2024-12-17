
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <cassert>
#include <iostream>
#include <ranges>
#include <regex>

#include "utils.hpp"

struct State {
  bool operator==(State const &other) const {
    return a == other.a && b == other.b && c == other.c;
  }
  unsigned int dv(unsigned int opcode) const {
    unsigned int num = a;
    unsigned int d = 1 << read_combo(opcode);
    return num / d;
  }

  void adv(unsigned int opcode) { a = dv(opcode); }

  void bdv(unsigned int opcode) { b = dv(opcode); }

  void cdv(unsigned int opcode) { c = dv(opcode); }

  void bxl(unsigned int opcode) { b = opcode ^ b; }
  void bst(unsigned int opcode) { b = read_combo(opcode) % 8; }

  // jnz omitted

  void bxc(unsigned int _) { b = c ^ b; }

  // out omitted

  unsigned int read_combo(unsigned int opcode) const {
    switch (opcode) {
    case 0:
    case 1:
    case 2:
    case 3:
      return opcode;
      break;
    case 4:
      return a;
      break;
    case 5:
      return b;
      break;
    case 6:
      return c;
      break;
    default:
      assert(false);
      return 0;
    }
  }

  unsigned int a;
  unsigned int b;
  unsigned int c;

  friend std::ostream &operator<<(std::ostream &os, State const &state) {
    os << "{ " << state.a << ", " << state.b << ", " << state.c << " }";
    return os;
  }
};

const unsigned int ADV = 0;
const unsigned int BXL = 1;
const unsigned int BST = 2;
const unsigned int JNZ = 3;
const unsigned int BXC = 4;
const unsigned int OUT = 5;
const unsigned int BDV = 6;
const unsigned int CDV = 7;

struct Program {
  static Program parse(std::vector<std::string> const &lines);
  Program(State const &s, std::vector<unsigned int> const &is)
      : state(s), instructions(is) {}
  bool operator==(Program const &other) const {
    return pointer == other.pointer && state == other.state &&
           instructions == other.instructions;
  }
  void run() {
    while (pointer < instructions.size()) {
      switch (instructions[pointer]) {
      case ADV:
        state.adv(instructions[pointer + 1]);
        pointer += 2;
        break;
      case BDV:
        state.bdv(instructions[pointer + 1]);
        pointer += 2;
        break;
      case CDV:
        state.cdv(instructions[pointer + 1]);
        pointer += 2;
        break;
      case BXL:
        state.bxl(instructions[pointer + 1]);
        pointer += 2;
        break;
      case BST:
        state.bst(instructions[pointer + 1]);
        pointer += 2;
        break;
      case BXC:
        state.bxc(instructions[pointer + 1]);
        pointer += 2;
        break;
      case JNZ:
        if (state.a != 0) {
          pointer = instructions[pointer + 1];
        } else {
          pointer += 2;
        }
        break;
      case OUT:
        out.push_back(state.read_combo(instructions[pointer + 1]) % 8);
        pointer += 2;
        break;
      default:
        assert(false);
        break;
      }
    }
  }
  State state;
  std::vector<unsigned int> instructions;
  std::vector<unsigned int> out;
  size_t pointer = 0;

  friend std::ostream &operator<<(std::ostream &os, Program const &program) {
    os << program.state << "[";
    for (const auto i : program.instructions) {
      os << i << ", ";
    }
    os << "]" << "(" << program.pointer << ")";
    return os;
  }
};

unsigned int parse_register(std::string const &line) {
  std::regex r("^Register .: (.*)$");
  std::smatch match = *(std::sregex_iterator(line.begin(), line.end(), r));
  return ints(match[1])[0];
}

std::vector<unsigned int> parse_instructions(std::string const &line) {
  std::regex r("^Program: (.*)$");
  std::smatch match = *(std::sregex_iterator(line.begin(), line.end(), r));
  return ints(match[1], ',');
}

Program Program::parse(std::vector<std::string> const &lines) {
  unsigned int a = parse_register(lines[0]);
  unsigned int b = parse_register(lines[1]);
  unsigned int c = parse_register(lines[2]);
  std::vector<unsigned int> instructions = parse_instructions(lines[4]);
  return Program({a, b, c}, instructions);
}

std::string solve_part_one(std::vector<std::string> const &lines) {
  Program program = Program::parse(lines);
  program.run();
  return program.out | std::views::transform([](auto const &i) {
           return std::to_string(i);
         }) |
         std::ranges::views::join_with(',') | std::ranges::to<std::string>();
}

unsigned int solve_part_two(std::vector<std::string> const &lines) {
  (void)lines;
  return 0;
}

#ifdef DOCTEST_CONFIG_DISABLE
int main(int _, char *argv[]) {
  std::string filename = argv[2];
  std::string part = argv[1];
  if (part == "1") {
    std::string res = solve_part_one(read_input_file(filename));
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
               "Register A: 729",
               "Register B: 0",
               "Register C: 0",
               "",
               "Program: 0,1,5,4,3,0",
           }),
           "4,6,3,5,6,3,5,2,1,0");
}

TEST_CASE("Example Part Two") {
  CHECK_EQ(solve_part_two({
               "Register A: 729",
               "Register B: 0",
               "Register C: 0",
               "",
               "Program: 0,1,5,4,3,0",
           }),
           0);
}

TEST_CASE("Read state") {
  [[maybe_unused]] State state{1, 2, 3};
  CHECK_EQ(state.read_combo(0), 0);
  CHECK_EQ(state.read_combo(1), 1);
  CHECK_EQ(state.read_combo(2), 2);
  CHECK_EQ(state.read_combo(3), 3);
  CHECK_EQ(state.read_combo(4), 1);
  CHECK_EQ(state.read_combo(5), 2);
  CHECK_EQ(state.read_combo(6), 3);
}

TEST_CASE("dv") {
  [[maybe_unused]] State state{10, 1, 3};
  CHECK_EQ(state.dv(0), 10);
  CHECK_EQ(state.dv(1), 5);
  CHECK_EQ(state.dv(3), 1);
  CHECK_EQ(state.dv(4), 0);
  CHECK_EQ(state.dv(5), 5);
  CHECK_EQ(state.dv(6), 1);
}

TEST_CASE("parse program") {
  CHECK_EQ(Program::parse({
               "Register A: 729",
               "Register B: 0",
               "Register C: 0",
               "",
               "Program: 0,1,5,4,3,0",
           }),
           Program({729, 0, 0}, {0, 1, 5, 4, 3, 0}));
}
