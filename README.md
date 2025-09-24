# Advent of code 2024 in C++

Archive of my setup and solutions.

The day 11 is in python because big numbers :P

## Dev instructions

Requires **python** and **cmake**. Default compiler is **clang** and default generator is **ninja**.

- Configure: `make configure`
- Build: `make rebuild`
- Run all tests (may be long since some solutions are a kinda brute-forcing): `make tests`
- Run the tests for a given day: `make test_day_DD`
- Run a given day solver on the corresponding input (NB: the tests also do that): `make run_day_DD`
- Create a template for a given day and download the corresponding input (NB: this requires that you create a aoc_session_token file containing your aoc api token): `make day_DD`
