#include "my_player.hpp"
#include <cmath>
#include <cstdio>
#include <cstdlib>

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

int MyPlayer::max_run(const State &state, Sign sign, int x, int y,
                      int dx, int dy, int L) const {
  const auto &o = state.get_opts();
  int left = 0;
  for (int i = 1; i < L; ++i) {
    int nx = x - dx * i, ny = y - dy * i;
    if (nx < 0 || ny < 0 || nx >= o.cols || ny >= o.rows) break;
    if (state.get_value(nx, ny) != sign) break;
    ++left;
  }
  int right = 0;
  for (int i = 1; i < L; ++i) {
    int nx = x + dx * i, ny = y + dy * i;
    if (nx < 0 || ny < 0 || nx >= o.cols || ny >= o.rows) break;
    if (state.get_value(nx, ny) != sign) break;
    ++right;
  }
  return 1 + left + right;
}

static long long run_weight(int n) {
  if (n <= 0) return 0;
  if (n == 1) return 1;
  return 1LL << (2 * n);
}

void MyPlayer::main_eval(const State &state, Field &f) const {
  const int L = state.get_opts().win_len;
  const Sign me = m_sign;
  const Sign opp = opp_sign(me);
  static const int dirs[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};

  for (int y = 0; y < f.rows; ++y) {
    for (int x = 0; x < f.cols; ++x) {
      Cell &c = f.at(x, y);

      if (c.sign == Sign::WALL || c.sign == opp) {
        for (int d = 0; d < 4; ++d) c.dir[d] = 0;
        c.bonus = 0;
        continue;
      }

      const auto &G = m_genome;

      for (int d = 0; d < 4; ++d) {
        const int dx = dirs[d][0], dy = dirs[d][1];
        const int my_run = max_run(state, me, x, y, dx, dy, L);
        const int opp_run = max_run(state, opp, x, y, dx, dy, L);
        long long v = (G.w_own() * run_weight(my_run) + G.w_opp() * run_weight(opp_run));
        c.dir[d] = (v > 1e9) ? (int)1e9 : (int)v;
      }

      if (c.sign == me) {
        int b = 0;
        for (int d = 0; d < 4; ++d) {
          const int dx = dirs[d][0], dy = dirs[d][1];
          const int my_run = max_run(state, me, x, y, dx, dy, L);
          const int opp_run = max_run(state, opp, x, y, dx, dy, L);
          long long v = G.w_own_calc() * run_weight(my_run) + G.w_opp_calc() * run_weight(opp_run);
          int val = (v > 1e9) ? (int)1e9 : (int)v;
          if (val > b) b = val;
        }
        c.bonus = b;
        c.opp_bonus = 0;
      } else if (c.sign == opp) {
        int b = 0;
        for (int d = 0; d < 4; ++d) {
          const int dx = dirs[d][0], dy = dirs[d][1];
          const int opp_run = max_run(state, opp, x, y, dx, dy, L);
          const int my_run = max_run(state, me, x, y, dx, dy, L);
          long long v = G.w_own() * run_weight(my_run) + G.w_opp() * run_weight(opp_run);
          int val = (v > 1e9) ? (int)1e9 : (int)v;
          if (val > b) b = val;
        }
        c.opp_bonus = b;
        c.bonus = 0;
      } else {
        c.bonus = 0;
        c.opp_bonus = 0;
      }
    }
  }
}

void MyPlayer::topeval(const State &state, Field &f) const {
  const int L = state.get_opts().win_len;
  const Sign opp = opp_sign(m_sign);
  static const int dirs[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
  const auto &G = m_genome;

  for (int y = 0; y < f.rows; ++y) {
    for (int x = 0; x < f.cols; ++x) {
      Cell &c = f.at(x, y);
      if (c.filled()) {
        c.score = -1;
        continue;
      }

      long long total = 0;

      for (int d = 0; d < 4; ++d) {
        const int dx = dirs[d][0], dy = dirs[d][1];

        total += G.w_dir() * c.dir[d];

        for (int i = 1; i <= L - 1; ++i) {
          const int nx = x + dx * i, ny = y + dy * i;
          if (!f.valid(nx, ny)) break;
          const Cell &nb = f.at(nx, ny);
          if (nb.sign == Sign::WALL) break;
          if (nb.filled()) {
            total += G.w_bonus() * nb.bonus + G.w_opp_bonus() * nb.opp_bonus;
          } else {
            total += nb.dir[d];
          }
        }

        for (int i = 1; i <= L - 1; ++i) {
          const int nx = x - dx * i, ny = y - dy * i;
          if (!f.valid(nx, ny)) break;
          const Cell &nb = f.at(nx, ny);
          if (nb.sign == Sign::WALL) break;
          if (nb.filled()) {
            total += G.w_bonus() * nb.bonus + G.w_opp_bonus() * nb.opp_bonus;
          } else {
            total += nb.dir[d];
          }
        }
      }

      c.score = (total > 1e9) ? (int)1e9 : (int)total;
    }
  }
}

int MyPlayer::fork(const State &state, Sign sign, int x, int y, int L) const {
  const auto &o = state.get_opts();
  static const int dirs[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
  int count = 0;

  for (auto &d : dirs) {
    const int dx = d[0], dy = d[1];

    int left = 0;
    for (int i = 1; i < L; ++i) {
      const int nx = x - dx * i, ny = y - dy * i;
      if (nx < 0 || ny < 0 || nx >= o.cols || ny >= o.rows) break;
      if (state.get_value(nx, ny) != sign) break;
      ++left;
    }
    int right = 0;
    for (int i = 1; i < L; ++i) {
      const int nx = x + dx * i, ny = y + dy * i;
      if (nx < 0 || ny < 0 || nx >= o.cols || ny >= o.rows) break;
      if (state.get_value(nx, ny) != sign) break;
      ++right;
    }

    const int run = 1 + left + right;

    bool left_open = false, right_open = false;

    {
      const int nx = x - dx * (left + 1);
      const int ny = y - dy * (left + 1);
      if (nx >= 0 && ny >= 0 && nx < o.cols && ny < o.rows) {
        if (state.get_value(nx, ny) == Sign::NONE) left_open = true;
      }
    }
    {
      const int nx = x + dx * (right + 1);
      const int ny = y + dy * (right + 1);
      if (nx >= 0 && ny >= 0 && nx < o.cols && ny < o.rows) {
        if (state.get_value(nx, ny) == Sign::NONE) right_open = true;
      }
    }

    if (run >= L) {
    } else if (run == L - 1 && left_open && right_open) {
      ++count;
    } else if (run == L - 2 && left_open && right_open) {
      ++count;
    }
  }
  return count;
}

void MyPlayer::forkeval(const State &state, Field &f) const {
  const int L = state.get_opts().win_len;
  const Sign me = m_sign;
  const Sign opp = opp_sign(me);
  const auto &G = m_genome;

  for (int y = 0; y < f.rows; ++y) {
    for (int x = 0; x < f.cols; ++x) {
      Cell &c = f.at(x, y);
      if (c.filled()) {
        c.fork = 0;
        continue;
      }

      const int my_forks = fork(state, me, x, y, L);
      const int opp_forks = fork(state, opp, x, y, L);

      int v = 0;
      if (my_forks >= 3) v += G.f3_my();
      else if (my_forks == 2) v += G.f2_my();
      else if (my_forks == 1) v += G.f1_my();

      if (opp_forks >= 3) v += G.f3_opp();
      else if (opp_forks == 2) v += G.f2_opp();
      else if (opp_forks == 1) v += G.f1_opp();
      
      c.fork = v;
    }
  }
}

MoveStats MyPlayer::evaluate_move(const State &state, int cx, int cy, Sign sign) const {
  MoveStats s;
  const auto &o = state.get_opts();
  const int L = o.win_len;
  const int w = o.cols, h = o.rows;
  const int dx[4] = {1, 0, 1, 1};
  const int dy[4] = {0, 1, 1, -1};

  for (int d = 0; d < 4; ++d) {
    int d_fours = 0, d_threes = 0, d_twos = 0;

    for (int offset = -L + 1; offset <= 0; ++offset) {
      int start_x = cx + offset * dx[d];
      int start_y = cy + offset * dy[d];

      bool ok = true;
      int mine = 0;
      for (int i = 0; i < L; ++i) {
        int px = start_x + i * dx[d];
        int py = start_y + i * dy[d];
        if (px < 0 || px >= w || py < 0 || py >= h) {
          ok = false;
          break;
        }
        Sign cell = state.get_value(px, py);
        if (px == cx && py == cy) ++mine;
        else if (cell == sign) ++mine;
        else if (cell != Sign::NONE) {
          ok = false;
          break;
        }
      }

      if (ok) {
        if (mine == L) ++s.wins;
        else if (mine == L - 1) ++d_fours;
        else if (mine == L - 2) ++d_threes;
        else if (mine == L - 3) ++d_twos;
      }
    }

    if (d_fours >= 2) ++s.open_fours;
    else if (d_fours == 1) ++s.fours;

    if (d_threes >= 2) ++s.open_threes;
    else if (d_threes == 1) ++s.threes;

    s.twos += d_twos;
  }
  return s;
}

void MyPlayer::patterneval(const State &state, Field &f) const {
  const Sign me = m_sign;
  const Sign opp = opp_sign(me);
  const auto &G = m_genome;

  const long long W_WIN = 1000000000LL;
  const long long W_OPEN4 = G.w_open4();
  const long long W_4 = G.w_4();
  const long long W_OPEN3 = G.w_open3();
  const long long W_3 = G.w_3();
  const long long W_2 = G.w_2();
  const long long ATT_MUL = G.att_mul();
  const long long DEF_MUL = G.def_mul();
  const long long MAX_VAL = 1000000000LL;

  for (int y = 0; y < f.rows; ++y) {
    for (int x = 0; x < f.cols; ++x) {
      Cell &c = f.at(x, y);
      if (c.filled()) {
        c.pattern = 0;
        continue;
      }

      MoveStats att = evaluate_move(state, x, y, me);
      MoveStats def = evaluate_move(state, x, y, opp);

      long long att_val = att.wins * W_WIN
                        + att.open_fours * W_OPEN4
                        + att.fours * W_4
                        + att.open_threes * W_OPEN3
                        + att.threes * W_3
                        + att.twos * W_2;

      long long def_val = def.wins * W_WIN
                        + def.open_fours * W_OPEN4
                        + def.fours * W_4
                        + def.open_threes * W_OPEN3
                        + def.threes * W_3
                        + def.twos * W_2;

      long long v = (att_val * ATT_MUL) + def_val * DEF_MUL;
      const int cx = f.cols / 2, cy = f.rows / 2;
      const int dist = std::abs(x - cx) + std::abs(y - cy);
      const long long CENTER_PENALTY = 0;
      v -= dist * CENTER_PENALTY;

      c.pattern = (v > MAX_VAL) ? (int)MAX_VAL : (int)v;
    }
  }
}

Point MyPlayer::make_move(const State &state) {
  const auto &o = state.get_opts();
  const Sign me = m_sign;
  const Sign opp = opp_sign(me);
  const int L = o.win_len;

  std::vector<Point> candidates;
  candidates.reserve(o.rows * o.cols);
  for (int y = 0; y < o.rows; ++y)
    for (int x = 0; x < o.cols; ++x)
      if (state.get_value(x, y) == Sign::NONE)
        candidates.push_back({x, y});

  if (candidates.empty()) return {0, 0};

  Field f = build_field(state);
  main_eval(state, f);
  topeval(state, f);
  forkeval(state, f);
  patterneval(state, f);

  Point best{-1, -1};
  long long best_score = -1;

  for (int y = 0; y < o.rows; ++y) {
    for (int x = 0; x < o.cols; ++x) {
      const Cell &c = f.at(x, y);
      if (c.filled()) continue;

      const long long s = (long long)c.score + (long long)c.fork + (long long)c.pattern;

      if (s > best_score) {
        best_score = s;
        best = {x, y};
      }
    }
  }

  if (best.x < 0) return candidates[0];
  return best;
}

} // namespace ttt::my_player