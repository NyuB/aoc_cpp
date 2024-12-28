
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <iostream>
#include <map>
#include <ranges>
#include <set>

#include "utils.hpp"

class Lan {
public:
  void link(std::string const &a, std::string const &b) {
    link_uni(a, b);
    link_uni(b, a);
  }

  std::set<std::string> const &neighbors(std::string const &from) const {
    return links.find(from)->second;
  }

  auto members() const {
    return std::ranges::transform_view(links,
                                       [](auto const &a) { return a.first; });
  }

  bool are_neighbors(std::string const &a, std::string const &b) const {
    auto find = links.find(a);
    return find != links.end() && find->second.contains(b);
  }

private:
  void link_uni(std::string const &from, std::string const &to) {
    if (!links.contains(from)) {
      links[from] = {to};
    } else {
      links[from].insert(to);
    }
  }
  std::map<std::string, std::set<std::string>> links;
};

bool at_least_one_starts_with(char letter, std::string const &a,
                              std::string const &b, std::string const &c) {
  return a.starts_with(letter) || b.starts_with(letter) ||
         c.starts_with(letter);
}

unsigned int solve_part_one(std::vector<std::string> const &lines) {
  Lan lan;
  for (auto const &line : lines) {
    StringIterator split(line, '-');
    std::string a(split.next());
    std::string b(split.next());
    lan.link(a, b);
  }
  std::set<std::set<std::string>> res;
  for (auto const &a : lan.members()) {
    for (auto const &b : lan.neighbors(a)) {
      for (auto const &c : lan.neighbors(b)) {
        if (lan.are_neighbors(a, b) && lan.are_neighbors(b, c) &&
            lan.are_neighbors(c, a) && at_least_one_starts_with('t', a, b, c)) {
          res.insert({a, b, c});
        }
      }
    }
  }
  return res.size();
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
               "kh-tc", "qp-kh", "de-cg", "ka-co", "yn-aq", "qp-ub", "cg-tb",
               "vc-aq", "tb-ka", "wh-tc", "yn-cg", "kh-ub", "ta-co", "de-co",
               "tc-td", "tb-wq", "wh-td", "ta-ka", "td-qp", "aq-cg", "wq-ub",
               "ub-vc", "de-ta", "wq-aq", "wq-vc", "wh-yn", "ka-de", "kh-ta",
               "co-tc", "wh-qp", "tb-vc", "td-yn",
           }),
           7);
}

TEST_CASE("Example Part Two") {
  [[maybe_unused]] unsigned int REPLACE_WHEN_STARTING_PART_TWO = 24;
  CHECK_EQ(solve_part_two({}), REPLACE_WHEN_STARTING_PART_TWO);
}
