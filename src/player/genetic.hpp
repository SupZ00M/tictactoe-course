#pragma once

#include <array>
#include <vector>
#include <random>
#include <cstdint>

namespace ttt::ga {

// 12 параметров, которые подбираем
struct Genome {
//1
 //std::array<int, 13> g = {1, 1, 1, 1, 1, 1, 1, 3000, 1500, 750, 2000, 750, 500};
//2
 //std::array<int, 13> g = {14, 6, 1, 12, 5, 8, 0, 10000, 8700, 11500, 500, 11500, 103000};
//3
 //std::array<int, 13> g = {19, 8, 5, 13, 4, 2, 0, 100, 16100, 12000, 5400, 7300, 12600};

//std::array<int, 19> g = {  13, 12, 2, 5, 2, 4, 1,   9000, 11350, 2700,  5000, 9000, 7600, 500000,50000, 20000,2000,200,30};
//std::array<int, 19> g = {  8, 4, 9, 11, 10, 7, 1, 7700, 1600, 3300,15000, 14900, 11200,  100000, 15000, 11000, 2500, 350, 10};
//std::array<int, 20> g = {  19, 8, 5, 13, 4, 2, 1, 100, 16100, 12000, 5400, 7300, 12600, 500000,50000, 20000,2000,200,30,2};

//std::array<int, 20> g = {10, 5, 1, 8, 4, 6, 2, 0, 16100, 12100, 5200, 7300, 12800, 200000, 50500, 20800, 1900, 210, 32, 1};


//std::array<int, 20> g = { 20, 10, 7, 12, 2, 4, 0,300, 16100, 11800, 5400, 7200, 12600,200000, 50000, 19800, 1950, 200,28, 1    };
std::array<int, 20> g = {
    20, 8, 7, 11, 3, 2, 0,          // Слой 1 и 2
    0, 16200, 12200,                // Свои вилки (f3m, f2m, f1m)
    5400, 7300, 13000,              // Защита вилок (f3o, f2o, f1o)
    200000, 48500, 20200, 1950, 200,// Паттерны (W_OPEN4...W_2)
    30, 1                           // ATT_MUL, DEF_MUL
};


  int   w_own()           const { return g[0]; }
  int   w_opp()           const { return g[1]; }
  int   w_bonus()         const { return g[2]; }
  int   w_opp_bonus()     const { return g[3]; }
  int   w_dir()           const { return g[4]; }
  int   w_own_calc()      const { return g[5]; }
  int   w_opp_calc()      const { return g[6]; }
  int   f3_my()           const { return g[7]; }
  int   f2_my()           const { return g[8]; }
  int   f1_my()           const { return g[9]; }
  int   f3_opp()           const { return g[10]; }
  int   f2_opp()          const { return g[11]; }
  int   f1_opp()          const { return g[12]; }

  int w_open4()      const { return g[13]; }
  int w_4()          const { return g[14]; }
  int w_open3()      const { return g[15]; }
  int w_3()          const { return g[16]; }
  int w_2()          const { return g[17]; }
  int att_mul()      const { return g[18]; }
  int def_mul()      const { return g[19]; }

  void print() const {
    std::printf("w_own=%d w_opp=%d w_bonus=%d w_opp_bonus=%d "
                "w_dir=%d w_own_calc=%d w_opp_calc=%d | "
                "f3m=%d f2m=%d f1m=%d f3o=%d f2o=%d f1o=%d | "
                "W_OPEN4=%d W_4=%d W_OPEN3=%d W_3=%d W_2=%d ATT_MUL=%d DEF_MUL=%d\n",
                g[0],g[1],g[2],g[3],g[4],g[5],g[6],
                g[7],g[8],g[9],g[10],g[11],g[12],
                g[13],g[14],g[15],g[16],g[17],g[18],g[19]);
  }
};

// Пределы для каждого гена: {min, max, step}
static const int GENE_LIMITS[20][3] = {
  {0,  20, 1},   // w_own
  {0,  20, 1},   // w_opp
  {0,  20, 1},   // w_bonus
  {0,  20, 1},   // w_opp_bonus
  {0,  20, 1},   // w_dir
  {0,  20, 1},   // w_own_calc
  {0,  20, 1},   // w_opp_calc
  {0, 17000, 100},   // f3_my
  {0, 17000, 100},   // f2_my
  {0, 17000, 100},    // f1_my
  {0, 17000, 100},   // f3_opp
  {0, 17000, 100},   // f2_opp
  {0, 17000, 100},   // f2_opp
  {0, 200000, 1000}, // W_OPEN4 (до 200k)
  {0,  200000,  500}, // W_4
  {0,  200000,  200}, // W_OPEN3
  {0,  200000,   50}, // W_3
  {0,   200000,    5}, // W_2
  {1,     100,    1}, // ATT_MUL (от 1 до 10)
   {1,     100,    1}, 
};
Genome random_genome(std::mt19937 &rng);
Genome mutate(const Genome &src, std::mt19937 &rng, double rate);
Genome crossover(const Genome &a, const Genome &b, std::mt19937 &rng);


} // namespace ttt::ga