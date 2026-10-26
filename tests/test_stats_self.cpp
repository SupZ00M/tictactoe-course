#include "player/my_player.hpp"
#include "test_stats.hpp"
#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[]) {
  std::cout << "Testing MyPlayer vs MyPlayer (self-play)\n";
  if (argc >= 2) std::srand(atoi(argv[1]));

  const int RUNS = 10;          // число серий
  const int GAMES_PER_RUN = 100;   // партий в серии

  int total_x = 0, total_o = 0, total_draw = 0, total_err = 0;

  for (int r = 0; r < RUNS; ++r) {
    ttt::my_player::MyPlayer p1("p1");
    ttt::my_player::MyPlayer p2("p2");
    auto res = ttt::test::run_game_tests(p1, p2, GAMES_PER_RUN);
    total_x += res.x_wins;
    total_o += res.o_wins;
    total_draw += res.draws;
    total_err += res.errors;
    std::printf("  run %d: X=%d O=%d draw=%d err=%d\n",
                r, res.x_wins, res.o_wins, res.draws, res.errors);
  }

  const int total = total_x + total_o + total_draw;
  std::printf("\n=== Self-play summary over %d games ===\n", total);
  std::printf("X (first) wins: %d (%.1f%%)\n", total_x, 100.0 * total_x / total);
  std::printf("O (second) wins: %d (%.1f%%)\n", total_o, 100.0 * total_o / total);
  std::printf("draws: %d (%.1f%%)\n", total_draw, 100.0 * total_draw / total);
  std::printf("errors: %d\n", total_err);
  return 0;
}