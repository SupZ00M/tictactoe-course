#include "player/my_player.hpp"
#include "core/baseline.hpp"
#include "test_stats.hpp"
#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[]) {
  std::cout << "Testing MyPlayer vs BaselineEasy\n";
  if (argc >= 2) std::srand(atoi(argv[1]));

  const int RUNS = 5;
  const int GAMES_PER_RUN = 200;

  int total_x = 0, total_o = 0, total_draw = 0, total_err = 0;

  for (int r = 0; r < RUNS; ++r) {
    ttt::my_player::MyPlayer p1("MyPlayer");
    auto *p2 = ttt::baseline::get_easy_player("BaselineEasy");
    auto res = ttt::test::run_game_tests(p1, *p2, GAMES_PER_RUN);
    total_x += res.x_wins;
    total_o += res.o_wins;
    total_draw += res.draws;
    total_err += res.errors;
    std::printf("  run %d: MyPlayer=%d Baseline=%d draw=%d err=%d\n",
                r, res.x_wins, res.o_wins, res.draws, res.errors);
    delete p2;
  }

  const int total = total_x + total_o + total_draw;
  std::printf("\n=== vs BaselineEasy over %d games ===\n", total);
  std::printf("MyPlayer wins:  %d (%.1f%%)\n", total_x, 100.0 * total_x / total);
  std::printf("Baseline wins:  %d (%.1f%%)\n", total_o, 100.0 * total_o / total);
  std::printf("draws:          %d (%.1f%%)\n", total_draw, 100.0 * total_draw / total);
  std::printf("errors:         %d\n", total_err);
  return 0;
}