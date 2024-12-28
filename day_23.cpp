
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <algorithm>
#include <iostream>
#include <map>
#include <queue>
#include <ranges>
#include <set>

#include "utils.hpp"

class Lan {
public:
  static Lan parse(std::vector<std::string> const &lines);
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

  bool is_neighbour_of_each(std::string const &from, auto const &all) const {
    for (auto const &each : all) {
      if (!are_neighbors(from, each))
        return false;
    }
    return true;
  }

private:
  void link(std::string const &a, std::string const &b) {
    link_uni(a, b);
    link_uni(b, a);
  }
  void link_uni(std::string const &from, std::string const &to) {
    if (!links.contains(from)) {
      links[from] = {to};
    } else {
      links[from].insert(to);
    }
  }
  std::map<std::string, std::set<std::string>> links;
};

Lan Lan::parse(std::vector<std::string> const &lines) {
  Lan lan;
  for (auto const &line : lines) {
    StringIterator split(line, '-');
    std::string a(split.next());
    std::string b(split.next());
    lan.link(a, b);
  }
  return lan;
}

struct SearchItem {
  SearchItem(SearchItem &&other)
      : members(std::move(other.members)), next(other.next),
        total(other.total) {}
  SearchItem(SearchItem const &other)
      : members(other.members), next(other.next), total(other.total) {}

  void operator=(SearchItem &&other) {
    members = std::move(other.members);
    next = other.next;
    total = other.total;
  }

  void operator=(SearchItem const &other) {
    members = other.members;
    next = other.next;
    total = other.total;
  }

  static SearchItem init(unsigned int total);

  std::set<std::string> members;
  unsigned int next;
  unsigned int total;

  void pick_next(std::vector<std::string> const &all, Lan const &lan) {
    members.insert(all[next]);
    next++;
    while (next < all.size() && !lan.is_neighbour_of_each(all[next], members)) {
      next++;
    }
  }

  void skip_next(std::vector<std::string> const &all, Lan const &lan) {
    unsigned int next_next = next + 1;
    while (next_next < all.size() &&
           !lan.is_neighbour_of_each(all[next_next], members)) {
      next_next++;
    }
    next = next_next;
  }

private:
  SearchItem(std::set<std::string> m, unsigned int n, unsigned int t)
      : members(m), next(n), total(t) {}
};

SearchItem SearchItem::init(unsigned int total) {
  return SearchItem({}, 0, total);
}

struct Search {
  std::vector<std::string> const &members;
  unsigned int max_possible(SearchItem const &item) const {
    return members.size() - item.next + item.members.size();
  }
  bool operator()(SearchItem const &l, SearchItem const &r) const {
    return max_possible(l) < max_possible(r);
  }
};

std::set<std::string> max_tightly_connected_components(Lan const &lan) {
  std::set<std::string> res;

  std::vector<std::string> members;
  members.append_range(lan.members());
  Search search{members};

  std::priority_queue<SearchItem, std::vector<SearchItem>, Search> q(
      search, {SearchItem::init(members.size())});
  while (!q.empty()) {
    SearchItem item = q.top();
    q.pop();
    if (search.max_possible(item) <= res.size())
      return res;
    if (item.members.size() > res.size())
      res = item.members;

    if (item.next < members.size()) {
      SearchItem copy = item;
      copy.pick_next(members, lan);
      q.push(std::move(copy));
    }

    if (item.next < members.size() - 1) {
      item.skip_next(members, lan);
      q.push(std::move(item));
    }
  }

  return res;
}

bool at_least_one_starts_with(char letter, std::string const &a,
                              std::string const &b, std::string const &c) {
  return a.starts_with(letter) || b.starts_with(letter) ||
         c.starts_with(letter);
}

unsigned int solve_part_one(std::vector<std::string> const &lines) {
  Lan lan = Lan::parse(lines);
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

std::string make_password(std::set<std::string> const &members) {
  std::string res;
  std::vector<std::string> sorted_names;
  sorted_names.append_range(members);
  std::ranges::sort(sorted_names);
  for (unsigned int i = 0; i < sorted_names.size(); i++) {
    if (i != 0)
      res += ",";
    res += sorted_names[i];
  }
  return res;
}

std::string solve_part_two(std::vector<std::string> const &lines) {
  Lan lan = Lan::parse(lines);
  std::set<std::string> max_scc = max_tightly_connected_components(lan);
  return make_password(max_scc);
}

#ifdef DOCTEST_CONFIG_DISABLE
int main(int _, char *argv[]) {
  std::string filename = argv[2];
  std::string part = argv[1];
  if (part == "1") {
    unsigned int res = solve_part_one(read_input_file(filename));
    std::cout << res << std::endl;
  } else if (part == "2") {
    std::string res = solve_part_two(read_input_file(filename));
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
  CHECK_EQ(solve_part_two({
               "kh-tc", "qp-kh", "de-cg", "ka-co", "yn-aq", "qp-ub", "cg-tb",
               "vc-aq", "tb-ka", "wh-tc", "yn-cg", "kh-ub", "ta-co", "de-co",
               "tc-td", "tb-wq", "wh-td", "ta-ka", "td-qp", "aq-cg", "wq-ub",
               "ub-vc", "de-ta", "wq-aq", "wq-vc", "wh-yn", "ka-de", "kh-ta",
               "co-tc", "wh-qp", "tb-vc", "td-yn",

           }),
           "co,de,ka,ta");
}
