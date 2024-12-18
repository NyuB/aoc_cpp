#ifndef UTILS_HPP
#define UTILS_HPP
#include <concepts>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

std::vector<std::string> read_input_file(std::string const &filename);
std::vector<unsigned int> ints(std::string line, char delimiter);
std::vector<unsigned int> ints(std::string line);
std::vector<unsigned long> longs(std::string line, char delimiter);
std::vector<unsigned long> longs(std::string const &line);
std::vector<unsigned long long> llongs(std::string line, char delimiter);
std::vector<unsigned long long> llongs(std::string line);

unsigned int svtoui(std::string_view const &str);
unsigned int stoui(std::string const &str);

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

template <typename Number,
          Number (*From_String)(const std::string &, size_t *, int)>
std::vector<Number> numbers(std::string const &line, char delimiter) {
  StringIterator it(line, delimiter);
  std::vector<Number> result;
  while (it.hasNext()) {
    result.push_back(From_String(std::string(it.next()), nullptr, 10));
  }
  return result;
}

template <typename Number, Number (*From_String)(const std::string &)>
std::vector<Number> numbers(std::string const &line, char delimiter) {
  StringIterator it(line, delimiter);
  std::vector<Number> result;
  while (it.hasNext()) {
    result.push_back(From_String(std::string(it.next())));
  }
  return result;
}

template <typename Checked, typename Bound>
inline bool in_grid_bound(Checked const &checked, Bound const &bound) {
  return checked >= 0 && static_cast<Bound>(checked) < bound;
}

template <typename T>
concept MidPointArithmetic = requires(T a, T b) {
  { a + b } -> std::same_as<T>;
  { a - b } -> std::same_as<T>;
  { a == b } -> std::same_as<bool>;
  { a + 1 } -> std::same_as<T>;
  { a / 2 } -> std::same_as<T>;
};

template <MidPointArithmetic Bound>
Bound arithmetic_mid_point(Bound const &low, Bound const &high) {
  return low + (high - low) / 2;
}

/**
 * Returns the first `Bound b` for which `p(b) == true`
 * The next point to search is computed with `Mid(min, max)`
 * @warning assumes `p(min) = false` and `p(max) = true`
 */
template <typename Bound, typename Predicate,
          Bound (*Mid)(Bound const &,
                       Bound const &) = arithmetic_mid_point<Bound>>
std::optional<unsigned int> bisect(Bound const &min, Bound const &max,
                                   Predicate const &p) {
  if (min == max) {
    return max;
  }
  if (max == min + 1)
    return max;
  Bound mid = Mid(min, max);
  if (mid == min || mid == max)
    return {};
  if (p(mid)) {
    return bisect(min, mid, p);
  } else {
    return bisect(mid, max, p);
  }
}

#endif // UTILS_HPP