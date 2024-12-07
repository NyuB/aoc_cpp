#ifndef UTILS_HPP
#define UTILS_HPP
#include <string>
#include <string_view>
#include <vector>

std::vector<std::string> read_input_file(std::string const &filename);
std::vector<unsigned int> ints(std::string line, char delimiter);
std::vector<unsigned int> ints(std::string line);
std::vector<unsigned long> longs(std::string line, char delimiter);
std::vector<unsigned long> longs(std::string line);
std::vector<unsigned long long> llongs(std::string line, char delimiter);
std::vector<unsigned long long> llongs(std::string line);

unsigned int svtoi(std::string_view const &str);

class StringIterator {
public:
  StringIterator(std::string const &string, char delimiter)
      : _string(string), _delimiter(delimiter), _begin(0), _end(0) {}
  std::string_view next();
  bool hasNext();

private:
  std::string const &_string;
  char _delimiter;
  size_t _begin;
  size_t _end;
};

#endif // UTILS_HPP