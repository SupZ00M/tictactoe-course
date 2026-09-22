#include "my_player.hpp"
#include <cstdlib>
#include <cstdio>
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

// max_run — сколько ПОДРЯД фишек sign через (x,y) в направлении (dx,dy),
// с учётом самой клетки (её как будто только что поставили)
int MyPlayer::max_run(const State &state, Sign sign, int x, int y,
                      int dx, int dy, int L) const {
  const auto &o = state.get_opts();
  int left = 0;
  for (int i = 1; i < L; ++i) {
    int nx = x - dx*i, ny = y - dy*i;
    if (nx < 0 || ny < 0 || nx >= o.cols || ny >= o.rows) break;
    if (state.get_value(nx, ny) != sign) break;
    ++left;
  }
  int right = 0;
  for (int i = 1; i < L; ++i) {
    int nx = x + dx*i, ny = y + dy*i;
    if (nx < 0 || ny < 0 || nx >= o.cols || ny >= o.rows) break;
    if (state.get_value(nx, ny) != sign) break;
    ++right;
  }
  return 1 + left + right;
}

// вес за линию длины n
static long long run_weight(int n) {
  if (n <= 0) return 0;
  if (n == 1) return 1;
  return 1LL << (2 * n);   // 4, 16, 64, 256, ...
}

//layers




void MyPlayer::main_eval(const State &state, Field &f) const {
  const int L = state.get_opts().win_len;
  const Sign me  = m_sign;
  const Sign opp = opp_sign(me);
  static const int dirs[4][2] = {{1,0},{0,1},{1,1},{1,-1}};

  for (int y = 0; y < f.rows; ++y) {
    for (int x = 0; x < f.cols; ++x) {
      Cell &c = f.at(x, y);

      // WALL и чужие — обнуляем всё
      if (c.sign == Sign::WALL || c.sign == opp) {
        for (int d = 0; d < 4; ++d) c.dir[d] = 0;
        c.bonus = 0;
        continue;
      }

      // свободные И свои — считаем dir[d] как если бы клетка была пустой
      // (для своих: max_run по-прежнему считает их как me, но сама клетка
      //  мысленно пустая — это уже заложено, потому что max_run считает от (x,y)
      //  как будто там стоит me, не читая текущее значение)
      for (int d = 0; d < 4; ++d) {
        const int dx = dirs[d][0], dy = dirs[d][1];
        const int my_run  = max_run(state, me,  x, y, dx, dy, L);
        const int opp_run = max_run(state, opp, x, y, dx, dy, L);
        long long v =(run_weight(my_run)+ run_weight(opp_run));
        c.dir[d] = (v > 1e9) ? (int)1e9 : (int)v;
      }

      // у свободных bonus = 0, у своих bonus = собственный dir (усреднённый)
      if (c.filled()) {
        // своя фишка: bonus = её собственный dir, как если бы была пустой
        // берём максимум по 4 направлениям — или сумму, на ваш выбор
        int b = 0;
        for (int d = 0; d < 4; ++d) if (c.dir[d] > b) b = c.dir[d];
        c.bonus = b;
      } else {
        c.bonus = 0;
      }
    }
  }
}

void MyPlayer::topeval(const State &state, Field &f) const {
  const int L = state.get_opts().win_len;
  const Sign opp = opp_sign(m_sign);
  static const int dirs[4][2] = {{1,0},{0,1},{1,1},{1,-1}};

  for (int y = 0; y < f.rows; ++y) {
    for (int x = 0; x < f.cols; ++x) {
      Cell &c = f.at(x, y);
      if (c.filled()) { c.score = -1; continue; }

      long long total = 0;

      for (int d = 0; d < 4; ++d) {
        const int dx = dirs[d][0], dy = dirs[d][1];

        total += c.dir[d];   // свой вклад (у свободной bonus = 0)

        // "+" до преграды/opp, не дальше L-1
        for (int i = 1; i <= L - 1; ++i) {
          const int nx = x + dx * i, ny = y + dy * i;
          if (!f.valid(nx, ny)) break;
          const Cell &nb = f.at(nx, ny);
          if (nb.sign == Sign::WALL) break;
          if (nb.sign == opp) break;
          // свободная → dir[d]; своя → bonus (её dir как если бы была пустой)
          total += (nb.filled() ? nb.bonus : nb.dir[d]);
        }

        // "−" аналогично
        for (int i = 1; i <= L - 1; ++i) {
          const int nx = x - dx * i, ny = y - dy * i;
          if (!f.valid(nx, ny)) break;
          const Cell &nb = f.at(nx, ny);
          if (nb.sign == Sign::WALL) break;
          if (nb.sign == opp) break;
          total += (nb.filled() ? nb.bonus : nb.dir[d]);
        }
      }

      c.score = (total > 1e9) ? (int)1e9 : (int)total;
    }
  }
}






int MyPlayer::fork(const State &state, Sign sign,
                                 int x, int y, int L) const {
  const auto &o = state.get_opts();
  static const int dirs[4][2] = {{1,0},{0,1},{1,1},{1,-1}};
  int count = 0;

  for (auto &d : dirs) {
    const int dx = d[0], dy = d[1];

    // считаем подряд sign в обе стороны от (x,y); сама (x,y) = sign
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

    // проверяем открытость концов (следующая клетка за концом линии)
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

    // классификация
    if (run >= L) {
      // победа — обрабатывается отдельно, не считаем как форк
    } else if (run == L - 1 && left_open && right_open) {
      ++count;   // открытая четвёрка (для L=5 это 4 подряд)
    } else if (run == L - 2 && left_open && right_open) {
      ++count;   // открытая тройка (для L=5 это 3 подряд)
    }
  }
  return count;
}

















void MyPlayer::forkeval(const State &state, Field &f) const {
  const int L = state.get_opts().win_len;
  const Sign me  = m_sign;
  const Sign opp = opp_sign(me);

  for (int y = 0; y < f.rows; ++y) {
    for (int x = 0; x < f.cols; ++x) {
      Cell &c = f.at(x, y);
      if (c.filled()) { c.fork = 0; continue; }

      const int my_forks  = fork(state, me,  x, y, L);
      const int opp_forks = fork(state, opp, x, y, L);

      int v = 0;
      if (my_forks >= 3)      v = 3000;   // тройная вилка — почти победа
else if (my_forks == 2) v = 2500;   // двойная вилка
else if (my_forks == 1) v =  500;   // одиночная открытая угроза

if (opp_forks >= 2)     v += 2000; 
      c.fork = v;
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

  Field f = build_field(state);
  main_eval(state, f);   // Слой 1: dir[] по направлениям
  topeval(state, f); 
  forkeval(state, f); 
       // Слой 2: score по клеткам

  Point best{-1, -1};
  int best_score = -1;
  const int cx = o.cols / 2;
  const int cy = o.rows / 2;

  for (int y = 0; y < o.rows; ++y) {
    for (int x = 0; x < o.cols; ++x) {
      const Cell &c = f.at(x, y);
      if (c.filled()) continue;

      long long s = (long long)c.score + (long long)c.fork;
s += (o.rows + o.cols) - (std::abs(x - cx) + std::abs(y - cy));

      if (s > best_score) { best_score = s; best = {x, y}; }
    }
  }

  std::printf("[mv] move=%d best=(%d,%d) score=%d\n",
            state.get_move_no(), best.x, best.y, best_score);
  if (best.x < 0) {
    for (int y = 0; y < o.rows; ++y)
      for (int x = 0; x < o.cols; ++x)
        if (state.get_value(x, y) == Sign::NONE)
          return {x, y};
  }
  return best;
}











}; // namespace ttt::my_player
