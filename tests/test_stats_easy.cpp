#include "player/my_player.hpp"
#include "core/baseline.hpp"
#include "test_stats.hpp"
#include <iostream>
#include <cstdlib>

int main(int argc, char *argv[]) {
  std::cout << "Testing MyPlayer vs BaselineEasy\n";
  if (argc >= 2) std::srand(atoi(argv[1]));

  int total_x = 0, total_o = 0, total_draw = 0, total_err = 0;
  double total_x_time = 0, total_o_time = 0, total_game_time = 0;
  int time_samples = 0;

  // Три серии: 100, 500, 1000
  int game_counts[] = {100, 500, 1000};

  for (int series = 0; series < 3; ++series) {
    int games = game_counts[series];
    std::cout << "\n--- Series " << series + 1 << " (" << games << " games) ---\n";

    int series_x = 0, series_o = 0, series_draw = 0, series_err = 0;
    double series_x_time = 0, series_o_time = 0, series_game_time = 0;

    ttt::my_player::MyPlayer p1("MyPlayer");
    auto *p2 = ttt::baseline::get_easy_player("BaselineEasy");
    auto res = ttt::test::run_game_tests(p1, *p2, games);

    series_x = res.x_wins;
    series_o = res.o_wins;
    series_draw = res.draws;
    series_err = res.errors;
    series_x_time = res.x_move_time;
    series_o_time = res.o_move_time;
    series_game_time = res.game_time;

    std::printf("  MyPlayer=%d Baseline=%d draw=%d err=%d\n",
                series_x, series_o, series_draw, series_err);

    total_x += series_x;
    total_o += series_o;
    total_draw += series_draw;
    total_err += series_err;
    total_x_time += series_x_time;
    total_o_time += series_o_time;
    total_game_time += series_game_time;
    ++time_samples;

    delete p2;
  }

  const int total = total_x + total_o + total_draw;
  std::printf("\n=== vs BaselineEasy over %d games ===\n", total);
  std::printf("MyPlayer wins:  %d (%.1f%%)\n", total_x, 100.0 * total_x / total);
  std::printf("Baseline wins:  %d (%.1f%%)\n", total_o, 100.0 * total_o / total);
  std::printf("draws:          %d (%.1f%%)\n", total_draw, 100.0 * total_draw / total);
  std::printf("errors:         %d\n", total_err);
  std::printf("avg game time:  %.2f ms\n", total_game_time / time_samples);
  return 0;
}