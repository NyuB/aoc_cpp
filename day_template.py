#! python3
template = """
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <iostream>

#include "utils.hpp"

unsigned int solve_part_one(std::vector<std::string> const &lines) { return 42; }

unsigned int solve_part_two(std::vector<std::string> const &lines) { return 24; }

#ifdef DOCTEST_CONFIG_DISABLE
int main(int argc, char *argv[]) {
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
"""

if __name__ == '__main__':
    import sys
    day = sys.argv[1]
    filename = f"day_{day}.cpp"
    print(f"Generating {filename}")
    with open(filename, 'w') as f:
        f.write(template)
    print(f"Generated {filename}")
