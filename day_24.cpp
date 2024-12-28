
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <cassert>
#include <functional>
#include <iostream>
#include <map>
#include <ranges>
#include <regex>

#include "utils.hpp"
using number = unsigned long long;

using fetch_f = std::function<bool(std::string const &)>;

class BoolGate {
public:
  using instance = std::shared_ptr<BoolGate>;
  virtual bool get(fetch_f fetch) const = 0;
  virtual ~BoolGate() {};
};

struct Constant : public BoolGate {
  Constant(bool v) : value(v) {}
  virtual bool get(fetch_f _) const override { return value; }
  bool value;
  virtual ~Constant() {}
};

struct AND : public BoolGate {
  AND(std::string const &l, std::string const &r) : left(l), right(r) {}
  virtual bool get(fetch_f fetch) const override {
    return fetch(left) && fetch(right);
  }
  std::string left;
  std::string right;
  virtual ~AND() {}
};

struct OR : public BoolGate {
  OR(std::string const &l, std::string const &r) : left(l), right(r) {}
  virtual bool get(fetch_f fetch) const override {
    return fetch(left) || fetch(right);
  }
  std::string left;
  std::string right;
  virtual ~OR() {}
};

struct XOR : public BoolGate {
  XOR(std::string const &l, std::string const &r) : left(l), right(r) {}
  virtual bool get(fetch_f fetch) const override {
    return fetch(left) ^ fetch(right);
  }
  std::string left;
  std::string right;
  virtual ~XOR() {}
};

class Gates {
public:
  static Gates parse(std::vector<std::string> const &lines);
  using label = std::string;
  using gates_map_t = std::map<label, BoolGate::instance>;

  auto labels() const {
    return std::ranges::transform_view(gates,
                                       [](auto const &a) { return a.first; });
  }

  bool fetch(std::string const &label) {
    auto find = cache.find(label);
    if (find != cache.end())
      return find->second;
    bool res = gates[label]->get([this](auto const &l) { return fetch(l); });
    cache[label] = res;
    return res;
  }

private:
  gates_map_t gates;
  std::map<label, bool> cache;
};

std::pair<std::string, BoolGate::instance>
parse_constant(std::string const &line) {
  std::regex r("^(.*): ([0-1])$");
  auto match = *std::sregex_iterator(line.begin(), line.end(), r);
  std::string left = match[1];
  BoolGate::instance right = (match[2] == "0")
                                 ? std::make_shared<Constant>(false)
                                 : std::make_shared<Constant>(true);
  return {left, right};
}

std::pair<std::string, BoolGate::instance> parse_gate(std::string const &line) {
  std::regex r("^(.*) (.*) (.*) -> (.*)$");
  auto match = *std::sregex_iterator(line.begin(), line.end(), r);
  std::string left = match[1];
  std::string op = match[2];
  std::string right = match[3];
  std::string label = match[4];

  if (op == "OR") {
    return {label, std::make_shared<OR>(left, right)};
  } else if (op == "AND") {
    return {label, std::make_shared<AND>(left, right)};
  } else if (op == "XOR") {
    return {label, std::make_shared<XOR>(left, right)};
  } else {
    assert(false);
    return {label, std::make_shared<OR>(left, right)};
  }
}

Gates Gates::parse(std::vector<std::string> const &lines) {
  bool parsing_constants = true;
  Gates result;
  for (auto const &line : lines) {
    if (line.empty()) {
      parsing_constants = false;
      continue;
    }
    if (parsing_constants) {
      auto c = parse_constant(line);
      result.gates[c.first] = c.second;
    } else {
      auto g = parse_gate(line);
      result.gates[g.first] = g.second;
    }
  }
  return result;
}
number solve_part_one(std::vector<std::string> const &lines) {
  Gates gates = Gates::parse(lines);
  std::vector<std::string> zs;
  for (const auto &label : gates.labels()) {
    if (label.starts_with('z'))
      zs.push_back(label);
  }
  std::ranges::sort(zs);
  number res = 0;
  for (number i = 0; i < zs.size(); i++) {
    bool z_bit = gates.fetch(zs[i]);
    number bit_value = static_cast<number>(z_bit) << i;
    res |= bit_value;
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

TEST_CASE("Example Part One (small)") {
  CHECK_EQ(solve_part_one({
               "x00: 1",
               "x01: 1",
               "x02: 1",
               "y00: 0",
               "y01: 1",
               "y02: 0",
               "",
               "x00 AND y00 -> z00",
               "x01 XOR y01 -> z01",
               "x02 OR y02 -> z02",
           }),
           4);
}

TEST_CASE("Example Part One (larger)") {
  CHECK_EQ(solve_part_one({
               "x00: 1",
               "x01: 0",
               "x02: 1",
               "x03: 1",
               "x04: 0",
               "y00: 1",
               "y01: 1",
               "y02: 1",
               "y03: 1",
               "y04: 1",
               "",
               "ntg XOR fgs -> mjb",
               "y02 OR x01 -> tnw",
               "kwq OR kpj -> z05",
               "x00 OR x03 -> fst",
               "tgd XOR rvg -> z01",
               "vdt OR tnw -> bfw",
               "bfw AND frj -> z10",
               "ffh OR nrd -> bqk",
               "y00 AND y03 -> djm",
               "y03 OR y00 -> psh",
               "bqk OR frj -> z08",
               "tnw OR fst -> frj",
               "gnj AND tgd -> z11",
               "bfw XOR mjb -> z00",
               "x03 OR x00 -> vdt",
               "gnj AND wpb -> z02",
               "x04 AND y00 -> kjc",
               "djm OR pbm -> qhw",
               "nrd AND vdt -> hwm",
               "kjc AND fst -> rvg",
               "y04 OR y02 -> fgs",
               "y01 AND x02 -> pbm",
               "ntg OR kjc -> kwq",
               "psh XOR fgs -> tgd",
               "qhw XOR tgd -> z09",
               "pbm OR djm -> kpj",
               "x03 XOR y03 -> ffh",
               "x00 XOR y04 -> ntg",
               "bfw OR bqk -> z06",
               "nrd XOR fgs -> wpb",
               "frj XOR qhw -> z04",
               "bqk OR frj -> z07",
               "y03 OR x01 -> nrd",
               "hwm AND bqk -> z03",
               "tgd XOR rvg -> z12",
               "tnw OR pbm -> gnj",
           }),
           2024);
}

TEST_CASE("Example Part Two") {
  [[maybe_unused]] unsigned int REPLACE_WHEN_STARTING_PART_TWO = 24;
  CHECK_EQ(solve_part_two({}), REPLACE_WHEN_STARTING_PART_TWO);
}
