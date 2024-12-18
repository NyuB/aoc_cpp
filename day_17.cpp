
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <cassert>
#include <iostream>
#include <numeric>
#include <ranges>
#include <regex>

using number = unsigned long long;

#include "utils.hpp"

struct State {
  static State parse(std::vector<std::string> const &lines);
  bool operator==(State const &other) const {
    return a == other.a && b == other.b && c == other.c;
  }
  bool operator<(State const &other) const {
    if (a != other.a)
      return a < other.a;
    if (b != other.b)
      return b < other.b;
    return c < other.c;
  }
  number dv(number opcode) const {
    number num = a;
    number d = 1 << read_combo(opcode);
    return num / d;
  }

  void adv(number opcode) { a = dv(opcode); }

  void bdv(number opcode) { b = dv(opcode); }

  void cdv(number opcode) { c = dv(opcode); }

  void bxl(number opcode) { b = opcode ^ b; }
  void bst(number opcode) { b = read_combo(opcode) % 8; }

  // jnz omitted

  void bxc(number _) { b = c ^ b; }

  // out omitted

  number read_combo(number opcode) const {
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

  number a;
  number b;
  number c;

  friend std::ostream &operator<<(std::ostream &os, State const &state) {
    os << "{ " << state.a << ", " << state.b << ", " << state.c << " }";
    return os;
  }
};

number parse_register(std::string const &line) {
  std::regex r("^Register .: (.*)$");
  std::smatch match = *(std::sregex_iterator(line.begin(), line.end(), r));
  return numbers<number, std::stoull>(match[1], ',')[0];
}

State State::parse(std::vector<std::string> const &lines) {
  number a = parse_register(lines[0]);
  number b = parse_register(lines[1]);
  number c = parse_register(lines[2]);
  return {a, b, c};
}

const number ADV = 0;
const number BXL = 1;
const number BST = 2;
const number JNZ = 3;
const number BXC = 4;
const number OUT = 5;
const number BDV = 6;
const number CDV = 7;

struct Program {
  static Program parse(std::vector<std::string> const &lines);
  Program(State const &s, std::vector<number> const &is)
      : state(s), instructions(is) {}
  bool operator==(Program const &other) const {
    return pointer == other.pointer && state == other.state &&
           instructions == other.instructions;
  }
  void run() {
    if (pointer >= instructions.size())
      return;
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
  State state;
  std::vector<number> instructions;
  std::vector<number> out;
  size_t pointer = 0;

  friend std::ostream &operator<<(std::ostream &os, Program const &program) {
    os << program.state << "[";
    for (const auto i : program.instructions) {
      os << i << ", ";
    }
    os << "]" << "(" << program.pointer << ")";
    for (const auto o : program.out) {
      os << o << ", ";
    }
    os << "]";
    return os;
  }
};

std::vector<number> parse_instructions(std::string const &line) {
  std::regex r("^Program: (.*)$");
  std::smatch match = *(std::sregex_iterator(line.begin(), line.end(), r));
  return numbers<number, std::stoull>(match[1], ',');
}

Program Program::parse(std::vector<std::string> const &lines) {
  State s = State::parse(lines);
  std::vector<number> instructions = parse_instructions(lines[4]);
  return Program(s, instructions);
}

std::string solve_part_one(std::vector<std::string> const &lines) {
  Program program = Program::parse(lines);
  while (program.pointer < program.instructions.size())
    program.run();
  return program.out | std::views::transform([](auto const &i) {
           return std::to_string(i);
         }) |
         std::ranges::views::join_with(',') | std::ranges::to<std::string>();
}

number run_once(std::vector<number> const &instructions, number a) {
  Program p({a, 0, 0}, instructions);
  while (p.out.empty())
    p.run();
  return p.out[0];
}

void expect(bool b, std::string const &msg) {
  if (!b) {
    printf("[Assertion failed] %s\n", msg.c_str());
  }
  assert(b);
}

void assert_one_out_per_loop(std::vector<number> const &instructions) {
  bool found = false;
  for (size_t i = 0; i < instructions.size(); i += 2) {
    if (instructions[i] == OUT) {
      expect(!found, "Should not have found more than one OUT instruction");
      found = true;
    }
  }
  expect(found, "Should have found one OUT instruction");
}

void assert_ends_with_a_jnz_loop_to_start(
    std::vector<number> const &instructions) {
  expect(instructions.size() >= 2, "Should have at least one instructions");
  expect(*(instructions.end() - 2) == JNZ,
         "Should have one jump as last instruction");
  expect(*(instructions.end() - 1) == 0,
         "Last jump should go back to program start");
}

void assert_shifts_a_once_by_3_at_each_loop(
    std::vector<number> const &instructions) {
  bool found = false;
  for (size_t i = 0; i < instructions.size(); i += 2) {
    if (instructions[i] == ADV) {
      expect(!found, "Should have found only one ADV");
      expect(instructions[i + 1] == 3, "Should shift A by 3");
      found = true;
    }
  }
  expect(found, "Should have found one ADV");
}

number solve_part_two(std::vector<std::string> const &lines) {
  number a = 0;
  auto instructions = parse_instructions(lines[4]);
  assert_one_out_per_loop(instructions);
  assert_shifts_a_once_by_3_at_each_loop(instructions);
  assert_ends_with_a_jnz_loop_to_start(instructions);
  for (size_t i = 0; i < instructions.size(); i++) {
    bool found = false;
    for (number a_8 = 0; a_8 < 8; a++) {
      number total = (a << 3llu) + a_8;
      if (run_once(instructions, total) ==
          instructions[instructions.size() - i - 1]) {
        a = total;
        found = true;
        break;
      }
    }
    expect(found, "Should have found a valid 3 bits number");
  }
  return a;
}

#ifdef DOCTEST_CONFIG_DISABLE
int main(int _, char *argv[]) {
  std::string filename = argv[2];
  std::string part = argv[1];
  if (part == "1") {
    std::string res = solve_part_one(read_input_file(filename));
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
               "Register A: 729",
               "Register B: 0",
               "Register C: 0",
               "",
               "Program: 0,1,5,4,3,0",
           }),
           "4,6,3,5,6,3,5,2,1,0");
}

TEST_CASE("Example Part Two Bis") {
  CHECK_EQ(solve_part_two({
               "Register A: 2024",
               "Register B: 0",
               "Register C: 0",
               "",
               "Program: 0,3,5,4,3,0",
           }),
           117440);
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