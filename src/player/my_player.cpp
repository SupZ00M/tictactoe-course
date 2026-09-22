#include "my_player.hpp"
#include <cstdlib>
#include <cmath>

namespace ttt::my_player {

void MyPlayer::set_sign(Sign sign) { m_sign = sign; }
const char *MyPlayer::get_name() const { return m_name; }

int EXP = 2;


Sign MyPlayer::opp_sign(Sign s) {
  switch (s) {
  case Sign::X: return Sign::O;
  case Sign::O: return Sign::X;
  default:      return Sign::NONE;
  }
}

Field MyPlayer::build_field(const State &state) const {
  const auto &o = state.get_opts();
  Field f;
  f.resize(o.rows, o.cols);
  for (int y = 0; y < o.rows; ++y)
    for (int x = 0; x < o.cols; ++x)
      f.at(x, y).sign = state.get_value(x, y);
  return f;
}





//layers




void MyPlayer::main_eval(const State &state, Field &f) const {
  const int L = state.get_opts().win_len;
  const Sign opp = opp_sign(m_sign);
  static const int dirs[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};

  for (int y = 0; y < f.rows; ++y) {
    for (int x = 0; x < f.cols; ++x) {
      Cell &c = f.at(x, y);

      // занятые клетки не оцениваем
      if (c.filled()) {
        for (int d = 0; d < 4; ++d) c.dir[d] = 0;
        continue;
      }

      for (int d = 0; d < 4; ++d) {
        const int dx = dirs[d][0];
        const int dy = dirs[d][1];

        int r1 = 0;
        for (int i = 1; i <= L - 1; ++i) {
          if (!f.valid(x + dx * i, y + dy * i)) break;
          Sign v = f.at(x + dx * i, y + dy * i).sign;
          if (v == Sign::WALL || v == opp) break;
          ++r1;
        }
        int r2 = 0;
        for (int i = 1; i <= L - 1; ++i) {
          if (!f.valid(x - dx * i, y - dy * i)) break;
          Sign v = f.at(x - dx * i, y - dy * i).sign;
          if (v == Sign::WALL || v == opp) break;
          ++r2;
        }

        const int w = r1 + r2 - L;
        c.dir[d] = (w > 0) ? (w * 2) : 0;
      }
    }
  }
}

































/*Point MyPlayer::make_move(const State &state) {
  Point result;
  for (int n_attempt = 0; n_attempt < 50; ++n_attempt) {
    result.x = std::rand() % state.get_opts().cols;
    result.y = std::rand() % state.get_opts().rows;
    if (state.get_value(result.x, result.y) != Sign::NONE) {
      --n_attempt;
      continue;
    }
    bool has_neighbors = false;
    for (int dx = -1; dx <= 1; ++dx) {
      for (int dy = -1; dy <= 1; ++dy) {
        if (dx == 0 && dy == 0)
          continue;
        const Sign val = state.get_value(result.x + dx, result.y + dy);
        if (val == Sign::X || val == Sign::O) {
          has_neighbors = true;
          break;
        }
      }
      if (has_neighbors)
        break;
    }
    if (has_neighbors)
      break;
  }
  return result;
}*/






Point MyPlayer::make_move(const State &state) {
  const auto &o = state.get_opts();

  // Слой 0: быстрая победа / блок
  Point fast{-1, -1};


  // Построить поле и прогнать через слои
  Field f = build_field(state);

  main_eval(state, f);   // sign → dir[]

  // Выбор максимума по score
  Point best{-1, -1};
  int best_score = -1;
  const int cx = o.cols / 2;
  const int cy = o.rows / 2;

  for (int y = 0; y < o.rows; ++y) {
    for (int x = 0; x < o.cols; ++x) {
      const Cell &c = f.at(x, y);
      if (c.filled()) continue;

      int s = c.score;
      s += (o.rows + o.cols) - (std::abs(x - cx) + std::abs(y - cy));

      if (s > best_score) {
        best_score = s;
        best = {x, y};
      }
    }
  }
    if (best.x < 0) {
    for (int y = 0; y < o.rows; ++y)
      for (int x = 0; x < o.cols; ++x)
        if (state.get_value(x, y) == Sign::NONE)
          return {x, y};
  }
  return best;
}












}; // namespace ttt::my_player
