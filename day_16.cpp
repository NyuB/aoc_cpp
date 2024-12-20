
#ifndef DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#endif

#include <cassert>
#include <iostream>
#include <iterator> // For std::forward_iterator_tag
#include <map>
#include <optional>
#include <queue>
#include <set>

#include "utils.hpp"

using Position = aoc::grid::Position;

struct Edge {
  Position destination;
  Position orientation;
  unsigned int cost;
};

Position WEST(0, -1);
Position EAST(0, +1);
Position NORTH(-1, 0);
Position SOUTH(+1, 0);

Position turn_90_clock(Position const &orientation) {
  if (orientation == WEST)
    return NORTH;
  if (orientation == NORTH)
    return EAST;
  if (orientation == EAST)
    return SOUTH;
  if (orientation == SOUTH)
    return WEST;
  assert(false);
  return NORTH;
}

Position turn_90_counterclock(Position const &orientation) {
  if (orientation == NORTH)
    return WEST;
  if (orientation == WEST)
    return SOUTH;
  if (orientation == SOUTH)
    return EAST;
  if (orientation == EAST)
    return NORTH;
  assert(false);
  return NORTH;
}

Position turn_180(Position const &orientation) {
  if (orientation == NORTH)
    return SOUTH;
  if (orientation == WEST)
    return EAST;
  if (orientation == SOUTH)
    return NORTH;
  if (orientation == EAST)
    return WEST;

  assert(false);
  return NORTH;
}

class Grid {
public:
  Grid(std::vector<std::string> lines) : grid(lines) {
    rows = lines.size();
    cols = lines.empty() ? 0 : lines[0].size();
  }

  bool operator==(Grid const &other) const {
    if (other.rows != rows || other.cols != cols)
      return false;
    for (size_t i = 0; i < rows; i++) {
      for (size_t j = 0; j < cols; j++) {
        if (grid[i][j] != other.grid[i][j])
          return false;
      }
    }
    return true;
  }

  friend std::ostream &operator<<(std::ostream &os, Grid const &g) {
    os << std::endl;
    for (std::string const &line : g.grid) {
      os << line << std::endl;
    }
    os << std::endl;
    return os;
  }
  char &operator[](Position const &p) { return grid[p.i][p.j]; }
  bool inbound(Position const &p) const {
    return in_grid_bound(p.i, rows) && in_grid_bound(p.j, cols);
  }
  bool free(Position const &p) const {
    return inbound(p) && grid[p.i][p.j] != '#';
  }
  Position find(char c) const {
    for (size_t i = 0; i < rows; i++) {
      for (size_t j = 0; j < cols; j++) {
        if (grid[i][j] == c)
          return Position(i, j);
      }
    }
    return Position(-1, -1);
  }
  size_t height() const { return rows; }
  size_t width() const { return cols; }

private:
  std::vector<std::string> grid;
  size_t rows;
  size_t cols;
};

std::vector<Edge> neighbours(Grid const &grid, Position const &here,
                             Position const &orientation) {
  std::vector<Edge> res;
  Position clock = turn_90_clock(orientation);
  Position counterclock = turn_90_counterclock(orientation);
  Position forward = here + orientation;
  res.push_back({here, clock, 1000});
  res.push_back({here, counterclock, 1000});
  if (grid.free(forward))
    res.push_back({forward, orientation, 1});
  return res;
}

unsigned int diff(unsigned int a, unsigned int b) {
  if (a > b)
    return a - b;
  return b - a;
}

struct SearchItem {
  Position p;
  Position orientation;
  unsigned int cost;
  SearchItem next(Edge const &edge) {
    return SearchItem{edge.destination, edge.orientation, edge.cost + cost};
  }
  // Priority queue ordering
  bool operator<(SearchItem const &other) const { return cost > other.cost; }
};

template <typename T> struct LinkedList {
  LinkedList(T const &t) : head(t), tail(NULL) {}
  LinkedList(T const &t, std::shared_ptr<LinkedList<T>> l) : head(t), tail(l) {}
  LinkedList(LinkedList<T> const &other) : head(other.head), tail(other.tail) {}
  void operator=(LinkedList<T> const &other) {
    head = other.head;
    tail = other.tail;
  };

  LinkedList operator+(T const &t) const {
    std::shared_ptr<LinkedList<T>> preserve =
        std::make_shared<LinkedList<T>>(head, tail);
    return LinkedList(t, preserve);
  }

  T head;
  std::shared_ptr<LinkedList<T>> tail;

  struct Iterator {
    using iterator_category = std::input_iterator_tag;
    using value_type = T;

    Iterator &operator++() {
      current = current->tail;
      return *this;
    }
    Iterator operator++(int) {
      Iterator tmp{current};
      operator++();
      return tmp;
    }
    Position operator*() const { return current->head; }
    Position operator->() const { return current->head; }

    friend bool operator==(const Iterator &a, const Iterator &b) {
      return a.current == b.current;
    };
    friend bool operator!=(const Iterator &a, const Iterator &b) {
      return a.current != b.current;
    };

    std::shared_ptr<LinkedList<T>> current;
  };

  Iterator begin() const { return {std::make_shared<LinkedList<T>>(*this)}; }

  Iterator end() const { return {NULL}; }
};

struct SearchItemTracking {
  Position p;
  Position orientation;
  unsigned int cost;
  LinkedList<Position> visited{p, NULL};
  SearchItemTracking next(Edge const &edge) {
    return SearchItemTracking{edge.destination, edge.orientation,
                              edge.cost + cost, visited + edge.destination};
  }
  // Priority queue ordering
  bool operator<(SearchItemTracking const &other) const {
    return cost > other.cost;
  }
};

struct Vertex {
  Position p;
  Position orientation;
  bool operator<(Vertex const &other) const {
    if (other.p == p)
      return orientation < other.orientation;
    return p < other.p;
  }
};

template <typename Item> class AlreadyVisited {
public:
  AlreadyVisited(Item const &start) {
    visited[{start.p, start.orientation}] = start.cost;
  }
  void set(Item const &item) {
    visited[{item.p, item.orientation}] = item.cost;
  }
  bool already_seen_better(Item const &item) const {
    return already_seen_better(Vertex{item.p, item.orientation}, item.cost);
  }

  bool already_seen_better(Edge const &e, unsigned int cost) const {
    return already_seen_better(Vertex{e.destination, e.orientation},
                               e.cost + cost);
  }

  bool already_seen_better(Vertex const &v, unsigned int cost) const {
    auto find = visited.find(v);
    return find != visited.end() && find->second < cost;
  }

private:
  std::map<Vertex, unsigned int> visited;
};

std::optional<unsigned int> shortest_path(Grid const &grid) {
  Position start = grid.find('S');
  Position end = grid.find('E');
  std::priority_queue<SearchItem> q;
  SearchItem start_item{start, EAST, 0};
  q.push(start_item);
  AlreadyVisited visited(start_item);
  while (!q.empty()) {
    SearchItem item = q.top();
    q.pop();
    visited.set(item);
    if (item.p == end)
      return std::optional(item.cost);
    std::vector<Edge> edges = neighbours(grid, item.p, item.orientation);
    for (const auto &edge : edges) {
      SearchItem next = item.next(edge);
      if (visited.already_seen_better(next))
        continue;
      q.push(std::move(next));
    }
  }
  return {};
}

std::set<Position> shortest_positions(Grid const &grid) {
  Position start = grid.find('S');
  Position end = grid.find('E');

  std::priority_queue<SearchItemTracking> q;
  std::set<Position> result{start};
  std::optional<unsigned int> shortest_distance{};

  SearchItemTracking start_item{start, EAST, 0, LinkedList(start)};
  q.push(start_item);
  AlreadyVisited visited(start_item);
  while (!q.empty()) {
    SearchItemTracking item = q.top();
    q.pop();
    visited.set(item);
    if (item.p == end && (shortest_distance.value_or(item.cost) == item.cost)) {
      shortest_distance = std::optional(item.cost);
      for (auto const &position : item.visited) {
        result.insert(position);
      }
    } else if (item.cost > shortest_distance.value_or(item.cost))
      break;
    std::vector<Edge> edges = neighbours(grid, item.p, item.orientation);
    for (const auto &edge : edges) {
      if (visited.already_seen_better(edge, item.cost))
        continue;
      q.push(item.next(edge));
    }
  }
  return result;
}

unsigned int solve_part_one(std::vector<std::string> const &lines) {
  Grid grid(lines);
  std::optional<unsigned int> res = shortest_path(grid);
  return res.value_or(0);
}

unsigned int solve_part_two(std::vector<std::string> const &lines) {
  Grid grid(lines);
  auto res = shortest_positions(grid);
  for (const auto &p : res) {
    grid[p] = 'O';
  }
  return res.size();
}

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

#ifndef DOCTEST_CONFIG_DISABLE
// You can add test helpers such as doctest ostream << overrides here
#endif

TEST_CASE("Example Part One") {
  CHECK_EQ(solve_part_one({
               "###############",
               "#.......#....E#",
               "#.#.###.#.###.#",
               "#.....#.#...#.#",
               "#.###.#####.#.#",
               "#.#.#.......#.#",
               "#.#.#####.###.#",
               "#...........#.#",
               "###.#.#####.#.#",
               "#...#.....#.#.#",
               "#.#.#.###.#.#.#",
               "#.....#...#.#.#",
               "#.###.#.#.#.#.#",
               "#S..#.....#...#",
               "###############",

           }),
           7036);
}

TEST_CASE("Example Part Two") {
  CHECK_EQ(solve_part_two({
               "###############",
               "#.......#....E#",
               "#.#.###.#.###.#",
               "#.....#.#...#.#",
               "#.###.#####.#.#",
               "#.#.#.......#.#",
               "#.#.#####.###.#",
               "#...........#.#",
               "###.#.#####.#.#",
               "#...#.....#.#.#",
               "#.#.#.###.#.#.#",
               "#.....#...#.#.#",
               "#.###.#.#.#.#.#",
               "#S..#.....#...#",
               "###############",

           }),
           45);
}
