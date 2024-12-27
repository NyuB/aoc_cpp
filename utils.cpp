#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif
#include "utils.hpp"

#include <charconv>
#include <fstream>

std::string_view StringIterator::next() {
  while (_end != _string.size() && _string.at(_end) != _delimiter) {
    _end++;
  }
  size_t resEnd = _end;
  size_t resBegin = _begin;
  while (_end != _string.size() && _string.at(_end) == _delimiter) {
    _end++;
  }
  _begin = _end;

  return std::basic_string_view(_string.data() + resBegin, resEnd - resBegin);
}

bool StringIterator::hasNext() { return _begin < _string.size(); }

std::vector<std::string> read_input_file(std::string const &filename) {
  std::ifstream file(filename);

  std::string line;
  std::vector<std::string> result;
  while (getline(file, line)) {
    result.push_back(line);
  }
  file.close();

  // Remove trailing empty lines
  while (!result.empty() && result[result.size() - 1].empty())
    result.pop_back();
  return result;
}

unsigned int svtoui(std::string_view const &str) {
  unsigned int result;
  (void)std::from_chars(str.data(), str.data() + str.size(), result);
  return result;
}

unsigned int stoui(std::string const &str) {
  unsigned int result;
  (void)std::from_chars(str.data(), str.data() + str.size(), result);
  return result;
}

std::vector<unsigned int> ints(std::string line, char delimiter) {
  return numbers<unsigned int, stoui>(line, delimiter);
}

std::vector<unsigned long> longs(std::string line, char delimiter) {
  return numbers<unsigned long, std::stoul>(line, delimiter);
}

std::vector<unsigned long long> llongs(std::string line, char delimiter) {
  return numbers<unsigned long long, std::stoull>(line, delimiter);
}

std::vector<unsigned int> ints(std::string line) { return ints(line, ' '); }
std::vector<unsigned long> longs(std::string const &line) {
  return longs(line, ' ');
}
std::vector<unsigned long long> llongs(std::string line) {
  return llongs(line, ' ');
}

namespace aoc {
namespace grid {

bool Position::operator==(Position const &other) const {
  return i == other.i && j == other.j;
}

bool Position::operator<(Position const &other) const {
  if (i == other.i)
    return j < other.j;
  return i < other.i;
}

Position Position::operator+(Position const &other) const {
  return Position(i + other.i, j + other.j);
}

Position Position::operator-(Position const &other) const {
  return Position(i - other.i, j - other.j);
}

Position Position::up() const { return {i - 1, j}; };
Position Position::down() const { return {i + 1, j}; };
Position Position::left() const { return {i, j - 1}; };
Position Position::right() const { return {i, j + 1}; };

Position Position::up_left() const { return {i - 1, j - 1}; };
Position Position::up_right() const { return {i - 1, j + 1}; };
Position Position::down_left() const { return {i + 1, j - 1}; };
Position Position::down_right() const { return {i + 1, j + 1}; };

int i;
int j;

std::ostream &operator<<(std::ostream &os, Position const &value) {
  os << value.i << "," << value.j;
  return os;
}
} // namespace grid
} // namespace aoc

#include "doctest.h"
TEST_CASE("Split single char") {
  std::string s = "|Abc|Def|Ghi|";
  StringIterator it(s, '|');
  CHECK_EQ(it.next(), "");
  CHECK_EQ(it.next(), "Abc");
  CHECK_EQ(it.next(), "Def");
  CHECK_EQ(it.next(), "Ghi");
  CHECK_FALSE(it.hasNext());
}

TEST_CASE("ints") {
  CHECK_EQ(ints("1 2 3 4"), std::vector<unsigned int>{1, 2, 3, 4});
  CHECK_EQ(ints("1|2|3|4", '|'), std::vector<unsigned int>{1, 2, 3, 4});
}

TEST_CASE("in_grid_bounds") {
  [[maybe_unused]] int negative = -1;
  [[maybe_unused]] int zero = 0;
  [[maybe_unused]] int in = 2;
  [[maybe_unused]] int out = 6;
  [[maybe_unused]] int just_out = 5;
  [[maybe_unused]] size_t bound = 5;

  CHECK(in_grid_bound(zero, bound));
  CHECK(in_grid_bound(in, bound));

  CHECK_FALSE(in_grid_bound(just_out, bound));
  CHECK_FALSE(in_grid_bound(out, bound));
  CHECK_FALSE(in_grid_bound(negative, bound));
}
