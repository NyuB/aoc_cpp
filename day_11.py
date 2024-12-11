import unittest
from functools import cache


def blink(stone: int) -> list[int]:
    if stone == 0:
        return [1]
    dstr = str(stone)
    d = len(dstr)
    if d % 2 == 0:
        left = int(dstr[: d // 2])
        right = int(dstr[d // 2 :])
        return [left, right]
    else:
        return [stone * 2024]


@cache
def count_after_n_blinks(stone: int, blinks: int) -> int:
    if blinks == 0:
        return 1
    blinked = blink(stone)
    res = 0
    for s in blinked:
        res += count_after_n_blinks(s, blinks - 1)
    return res


def solve(stones: list[int], blinks: int) -> int:
    res = 0
    for s in stones:
        res += count_after_n_blinks(s, blinks)
    return res


def solve_part_one(s: str) -> int:
    return solve([int(i) for i in s.split(" ")], 25)


def solve_part_two(s: str) -> int:
    return solve([int(i) for i in s.split(" ")], 75)


class Tests(unittest.TestCase):
    def input_file(self) -> str:
        with open("inputs/day_11.txt", "r") as intput_file:
            return intput_file.read().split("\n")[0]

    def test_example_part_one(self):
        self.assertEqual(solve_part_one("125 17"), 55312)

    def test_example_part_two(self):
        self.assertEqual(solve_part_two("125 17"), 65601038650482)

    def test_part_one(self):
        self.assertEqual(solve_part_one(self.input_file()), 186175)

    def test_part_two(self):
        self.assertEqual(solve_part_two(self.input_file()), 220566831337810)
