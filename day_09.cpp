
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <cassert>
#include <iostream>
#include <optional>

#include "utils.hpp"

using number = unsigned long long;

struct Block {
  static std::vector<Block> parse(std::string const &line);
  static Block compact(unsigned int id, unsigned int space);
  bool operator==(Block const &other) const {
    return id == other.id && space == other.space && free == other.free;
  }
  unsigned int id;
  unsigned int space;
  unsigned int free;
};

std::vector<Block> Block::parse(std::string const &line) {
  std::vector<Block> res;
  unsigned int id = 0;
  for (unsigned int i = 0; i < line.size(); i += 2) {
    unsigned int space = svtoi(line.substr(i, 1));
    assert(space > 0);
    unsigned int free = 0;
    if (i + 1 < line.size()) {
      free = svtoi(line.substr(i + 1, 1));
    }
    res.push_back({id, space, free});
    id++;
  }
  return res;
}

Block Block::compact(unsigned int id, unsigned int space) {
  return Block{id, space, 0};
}

class Memory {
public:
  Memory(std::vector<Block> const &available_)
      : available(available_), start(0), end(available_.size()) {}
  Block pop_head() {
    Block res = available[start];
    start++;
    consistent();
    return res;
  }
  Block pop_tail() {
    Block res = available[end - 1];
    end--;
    consistent();
    return res;
  }
  void consistent() { assert(end >= start); }
  bool empty() { return end == start; }

private:
  std::vector<Block> const &available;
  unsigned int start;
  unsigned int end;
};

number checksum(std::vector<Block> const &memory) {
  number res = 0;
  number position = 0;
  for (const auto &block : memory) {
    for (unsigned int i = 0; i < block.space; i++) {
      res += (position + i) * block.id;
    }
    position += block.space + block.free;
  }
  return res;
}

std::vector<Block> compact(std::string const &line) {
  std::vector<Block> parsed = Block::parse(line);
  Memory to_compact(parsed);
  std::vector<Block> res;

  while (!to_compact.empty()) {
    Block to_add = to_compact.pop_tail();
    while (to_add.space != 0) {
      if (res.empty() || res[res.size() - 1].free == 0) {
        if (to_compact.empty()) {
          res.push_back(Block::compact(to_add.id, to_add.space));
          to_add.space = 0;
        } else {
          Block padding = to_compact.pop_head();
          res.push_back(padding);
        }
      } else {
        Block &last = res[res.size() - 1];
        unsigned int space = std::min(last.free, to_add.space);
        unsigned int free = last.free - space;
        last.free = 0;
        res.push_back(Block{to_add.id, space, free});
        to_add.space -= space;
      }
    }
  }
  return res;
}

number solve_part_one(std::vector<std::string> const &lines) {
  return checksum(compact(lines[0]));
}

number solve_part_two(std::vector<std::string> const &lines) { return 24; }

#ifdef DOCTEST_CONFIG_DISABLE
int main(int argc, char *argv[]) {
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

std::ostream &operator<<(std::ostream &os, Block const &block) {
  os << "{ [" << block.id << "], " << block.space << ", " << block.free << " }";
  return os;
}

std::ostream &operator<<(std::ostream &os, std::vector<Block> const &blocks) {
  os << "[ ";
  for (const auto &block : blocks) {
    os << block << ", ";
  }
  os << "]";
  return os;
}

TEST_CASE("Example Part One") {
  CHECK_EQ(solve_part_one({"2333133121414131402"}), 1928);
}

TEST_CASE("Parse 12345") {
  std::vector<Block> memory = Block::parse("12345");
  CHECK_EQ(memory,
           std::vector<Block>{Block{0, 1, 2}, Block{1, 3, 4}, Block{2, 5, 0}});
}

TEST_CASE("Compact 12345") {
  std::vector<Block> compacted = compact("12345");
  CHECK_EQ(compacted, std::vector<Block>{
                          Block::compact(0, 1),
                          Block::compact(2, 2),
                          Block::compact(1, 3),
                          Block{2, 3, 1},
                      });
}

TEST_CASE("Checksum 12345") {
  CHECK_EQ(solve_part_one({"12345"}),
           0 + 2 + 2 * 2 + 3 + 4 + 5 + 2 * 6 + 2 * 7 + 2 * 8);
}

TEST_CASE("Compact example") {
  std::vector<Block> compacted = compact("2333133121414131402");
  CHECK_EQ(compacted, std::vector<Block>{
                          Block::compact(0, 2),
                          Block::compact(9, 2),
                          Block::compact(8, 1),
                          Block::compact(1, 3),
                          Block::compact(8, 3),
                          Block::compact(2, 1),
                          Block::compact(7, 3),
                          Block::compact(3, 3),
                          Block::compact(6, 1),
                          Block::compact(4, 2),
                          Block::compact(6, 1),
                          Block::compact(5, 4),
                          Block::compact(6, 1),
                          Block::compact(6, 1),
                      });
}

TEST_CASE("Empty block") {
  std::vector<Block> compacted = compact("1010");
  CHECK_EQ(compacted, std::vector<Block>{
                          Block::compact(0, 1),
                          Block::compact(1, 1),
                      });
}

TEST_CASE("One step compacting") {
  std::vector<Block> compacted = compact("1910");
  CHECK_EQ(compacted, std::vector<Block>{
                          Block::compact(0, 1),
                          Block{1, 1, 8},
                      });
}

TEST_CASE("No free space") {
  std::vector<Block> compacted = compact("102030");
  CHECK_EQ(compacted, std::vector<Block>{
                          Block::compact(0, 1),
                          Block::compact(1, 2),
                          Block::compact(2, 3),
                      });
}

TEST_CASE("Perfect fit") {
  std::vector<Block> compacted = compact("132030");
  CHECK_EQ(compacted, std::vector<Block>{
                          Block::compact(0, 1),
                          Block::compact(2, 3),
                          Block::compact(1, 2),
                      });
}

TEST_CASE("Parse") {
  CHECK_EQ(Block::parse("12345"), std::vector<Block>{
                                      Block{0, 1, 2},
                                      Block{1, 3, 4},
                                      Block{2, 5, 0},
                                  });
}
