#include "player/genetic.hpp"
#include "player/my_player.hpp"
#include "core/baseline.hpp"
#include "test_stats.hpp"

#include <cstdio>
#include <vector>
#include <algorithm>
#include <random>

using namespace ttt;
using namespace ttt::ga;

// Фитнес — винрейт против easy и hard (усреднённый)
double fitness(const Genome &g, int games_per_opp, unsigned seed) {
  // easy
  my_player::MyPlayer p1("p1", g);
  auto *easy = baseline::get_easy_player("easy");
  std::srand(seed);
  auto r_easy = test::run_game_tests(p1, *easy, games_per_opp);
  delete easy;

  // hard
  my_player::MyPlayer p2("p2", g);
  auto *hard = baseline::get_harder_player("hard");
  std::srand(seed + 1);
  auto r_hard = test::run_game_tests(p2, *hard, games_per_opp);
  delete hard;

  const int total_easy = r_easy.x_wins + r_easy.o_wins + r_easy.draws;
  const int total_hard = r_hard.x_wins + r_hard.o_wins + r_hard.draws;
  if (total_easy == 0 || total_hard == 0) return 0.0;

  double wr_easy = 1.0 * r_easy.x_wins / total_easy;
  double wr_hard = 1.0 * r_hard.x_wins / total_hard;

  // усредняем: hard-соперник даёт более значимый сигнал
  return 0.1 * wr_easy + 0.9 * wr_hard;
}

int main(int argc, char *argv[]) {
  // параметры GA
  const int POP_SIZE      = 10;    // особей в поколении
  const int GENERATIONS   = 10;    // поколений
  const int GAMES_PER_OPP = 50;    // партий на соперника (быстро для GA)
  const int TOURNAMENT_K  = 5;     // размер турнира при отборе
  const double MUT_RATE   = 0.7;
  const unsigned SEED     = (argc >= 2) ? std::atoi(argv[1]) : 42;

  std::mt19937 rng(SEED);

  // 1. Инициализация




  std::vector<Genome> pop(POP_SIZE);


  std::vector<double> fit(POP_SIZE);
 Genome best_genome;
best_genome.g ={  19, 8, 5, 13, 4, 2, 1, 100, 16100, 12000, 5400, 7300, 12600, 500000,50000, 20000,2000,200,30,2};
for (int i = 0; i < POP_SIZE; ++i) pop[i] = best_genome;
double best_fit = -1.0;
  for (int gen = 0; gen < GENERATIONS; ++gen) {
    std::printf("=== generation %d ===\n", gen);

    // 2. Оценка фитнеса

for (int i = 0; i < POP_SIZE; ++i) {
    fit[i] = fitness(pop[i], GAMES_PER_OPP, SEED + gen * 1000 + i);
}


 for (int i = 0; i < POP_SIZE; ++i) {
    if (fit[i] > best_fit) {
      best_fit = fit[i];
      best_genome = pop[i];
    }
  }
  double sum = 0.0, mx = -1.0;
int mx_idx = 0;
for (int i = 0; i < POP_SIZE; ++i) {
    sum += fit[i];
    if (fit[i] > mx) { mx = fit[i]; mx_idx = i; }
}
std::printf("  mean=%.4f max=%.4f (#%d)\n",
            sum / POP_SIZE, mx, mx_idx);

    // 3. Турнирный отбор + элитизм
    std::vector<Genome> next;
    // элитизм — топ-2
    {
      std::vector<int> idx(POP_SIZE);
      for (int i = 0; i < POP_SIZE; ++i) idx[i] = i;
      std::sort(idx.begin(), idx.end(),
                [&](int a, int b){ return fit[a] > fit[b]; });
      next.push_back(pop[idx[0]]);
      next.push_back(pop[idx[1]]);
    }

    // остальные — турнир + кроссовер + мутация
    std::uniform_int_distribution<int> pick(0, POP_SIZE - 1);
    auto tournament = [&]() {
      int best = pick(rng);
      for (int k = 1; k < TOURNAMENT_K; ++k) {
        int cand = pick(rng);
        if (fit[cand] > fit[best]) best = cand;
      }
      return pop[best];
    };

    while ((int)next.size() < POP_SIZE) {
      Genome a = tournament();
      Genome b = tournament();
      Genome child = crossover(a, b, rng);
      child = mutate(child, rng, MUT_RATE);
      next.push_back(child);
    }

    pop = next;
  }

  // 4. Финальная оценка лучшего


std::printf("\n=== BEST GENOME (all generations) ===\n");
best_genome.print();
std::printf("fitness=%.4f\n", best_fit);

  return 0;
}