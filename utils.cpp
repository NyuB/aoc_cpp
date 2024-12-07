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
  return result;
}

unsigned int svtoi(std::string_view const &str) {
  unsigned int result;
  (void)std::from_chars(str.data(), str.data() + str.size(), result);
  return result;
}

std::vector<unsigned int> ints(std::string line, char delimiter) {

  StringIterator it(line, delimiter);
  std::vector<unsigned int> result;
  while (it.hasNext()) {
    result.push_back(std::stoi(std::string(it.next())));
  }
  return result;
}

std::vector<unsigned long> longs(std::string line, char delimiter) {

  StringIterator it(line, delimiter);
  std::vector<unsigned long> result;
  while (it.hasNext()) {
    result.push_back(std::stoul(std::string(it.next())));
  }
  return result;
}

std::vector<unsigned int> ints(std::string line) { return ints(line, ' '); }
std::vector<unsigned long> longs(std::string line) { return longs(line, ' '); }

std::vector<unsigned long long> llongs(std::string line, char delimiter) {
  StringIterator it(line, delimiter);
  std::vector<unsigned long long> result;
  while (it.hasNext()) {
    result.push_back(std::stoull(std::string(it.next())));
  }
  return result;
}

std::vector<unsigned long long> llongs(std::string line) {
  return llongs(line, ' ');
}

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
