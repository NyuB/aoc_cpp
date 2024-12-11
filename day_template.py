#! python3
import sys
import os

template = """
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <iostream>

#include "utils.hpp"

unsigned int solve_part_one(std::vector<std::string> const &lines) { (void) lines; return 42; }

unsigned int solve_part_two(std::vector<std::string> const &lines) { (void) lines; return 24; }

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

TEST_CASE("Example Part One") {
  CHECK_EQ(solve_part_one({}), 0);
}

"""


def download_daily_input(day: str):
    with open("aoc_session_token", "r") as token_file:
        token = token_file.read()
    os.system(
        f'curl --raw --cookie "session={token}" https://adventofcode.com/2024/day/{day}/input -o inputs/day_{day}.txt'
    )


if __name__ == "__main__":
    day = sys.argv[1]
    filename = f"day_{day}.cpp"
    print(f"Generating {filename}")
    with open(filename, "w") as f:
        f.write(template)
    print(f"Generated {filename}")
    print("Downloading input file")
    download_daily_input(day)
