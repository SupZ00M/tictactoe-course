#pragma once

#include "core/game.hpp"
#include "genetic.hpp"
#include <algorithm>  
#include <vector>

namespace ttt::my_player {

using game::Event;
using game::IPlayer;
using game::Point;
using game::Sign;
using game::State;
using game::MoveResult;

static long long run_weight(int n);

struct Cell {
  Sign sign = Sign::NONE;
  int dir[4] = {0, 0, 0, 0};
  int bonus = 0;
  int opp_bonus = 0;
  int fork = 0;
  int pattern = 0;
  int score = 0;
  bool filled() const { return sign != Sign::NONE; }
};

struct MoveStats {
  int wins = 0;
  int open_fours = 0;
  int fours = 0;
  int open_threes = 0;
  int threes = 0;
  int twos = 0;
};

struct Field {
  std::vector<Cell> cells;
  int rows = 0, cols = 0;

  void resize(int r, int c) {
    rows = r;
    cols = c;
    cells.assign(r * c, {});
  }
  Cell &at(int x, int y) { return cells[y * cols + x]; }
  const Cell &at(int x, int y) const { return cells[y * cols + x]; }
  bool valid(int x, int y) const {
    return x >= 0 && y >= 0 && x < cols && y < rows;
  }
};

class MyPlayer : public IPlayer {
  Sign m_sign = Sign::NONE;
  const char *m_name;

  static constexpr int MAX_BALL = 1000000000;
  static constexpr int K_OWN = 2;
  static constexpr int K_OPP = 1;
  static constexpr int MAX_LINE = 1000000000;

public:
  ttt::ga::Genome m_genome;

  MyPlayer(const char *name) : m_sign(Sign::NONE), m_name(name) {}
  MyPlayer(const char *name, const ttt::ga::Genome &g)
      : m_sign(Sign::NONE), m_name(name), m_genome(g) {}

  MoveStats evaluate_move(const State &state, int cx, int cy, Sign sign) const;
  void patterneval(const State &state, Field &f) const;

  void set_sign(Sign sign) override;
  Point make_move(const State &game) override;
  const char *get_name() const override;

  Field build_field(const State &state) const;

  void main_eval(const State &state, Field &f) const;
  void topeval(const State &state, Field &f) const;
 
  int fork(const State &state, Sign sign, int x, int y, int L) const;
  void forkeval(const State &state, Field &f) const;

  static Sign opp_sign(Sign s);
  int max_run(const State &state, Sign sign, int x, int y,
              int dx, int dy, int L) const;
};

} // namespace ttt::my_player
//  cmake -DBUILD_TTTCORE=PREBUILT .. && make && tests/test_stats_vs_baseline
//  cmake -DBUILD_TTTCORE=PREBUILT .. && make && tests/test_stats_all
//cmake -DBUILD_TTTCORE=PREBUILT .. && make && tests/test_genetic 42