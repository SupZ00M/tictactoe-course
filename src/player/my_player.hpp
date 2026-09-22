#pragma once

#include "core/game.hpp"
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
    Sign sign = Sign::NONE;     // NONE / X / O / WALL
    int  dir[4] = {0, 0, 0, 0}; // Слой 1: очки по 4 направлениям
     int  bonus  = 0;     
    int  score  = 0;            // Слой 2+: агрегат
    bool filled() const { return sign != Sign::NONE; }
  };

  // Игровое поле: заполненность + очки
  struct Field {
    std::vector<Cell> cells;
    int rows = 0, cols = 0;

    void resize(int r, int c) {
      rows = r; cols = c;
      cells.assign(r * c, {});
    }
    Cell       &at(int x, int y)       { return cells[y * cols + x]; }
    const Cell &at(int x, int y) const { return cells[y * cols + x]; }
    bool valid(int x, int y) const {
      return x >= 0 && y >= 0 && x < cols && y < rows;
    }
  };



class MyPlayer : public IPlayer {
  Sign m_sign = Sign::NONE;
  const char *m_name;


  static constexpr int MAX_BALL = 500;
  static constexpr int K_OWN    = 1;
  static constexpr int K_OPP    = 1;
  static constexpr int MAX_LINE = 1000000000;
  
 





public:
  MyPlayer(const char *name) : m_sign(Sign::NONE), m_name(name) {}
  void set_sign(Sign sign) override;
  Point make_move(const State &game) override;
  const char *get_name() const override;

  

  Field build_field(const State &state) const;


  void main_eval(const State &state, Field &f) const;
   void topeval(const State &state, Field &f) const;

  static Sign opp_sign(Sign s);
  int max_run(const State &state, Sign sign, int x, int y,
              int dx, int dy, int L) const;


};

}; // namespace ttt::my_player
//  cmake -DBUILD_TTTCORE=PREBUILT .. && make && tests/test_stats_vs_baseline