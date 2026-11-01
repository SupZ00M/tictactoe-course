#include "genetic.hpp"
#include <algorithm>
#include <cstdio>

namespace ttt::ga {

using Rng = std::mt19937;

// Случайный геном
Genome random_genome(Rng &rng) {
  Genome g;
  for (int i = 0; i < 20; ++i) {
    const int lo = GENE_LIMITS[i][0];
    const int hi = GENE_LIMITS[i][1];
    const int step = GENE_LIMITS[i][2];
    const int n_steps = (hi - lo) / step;
    std::uniform_int_distribution<int> u(0, n_steps);
    g.g[i] = lo + u(rng) * step;
  }
  return g;
}

Genome mutate(const Genome &src, Rng &rng, double rate) {
  Genome out = src;
  std::uniform_real_distribution<double> coin(0.0, 1.0);
  for (int i = 0; i < 20; ++i) {
    if (coin(rng) < rate) {
      const int lo = GENE_LIMITS[i][0];
      const int hi = GENE_LIMITS[i][1];
      const int step = GENE_LIMITS[i][2];
      std::uniform_int_distribution<int> delta(-2, 2);
      int val = src.g[i] + delta(rng) * step;
      out.g[i] = std::clamp(val, lo, hi);
    }
  }
  return out;
}

Genome crossover(const Genome &a, const Genome &b, Rng &rng) {
  Genome out;
  std::uniform_int_distribution<int> point(0, 20);
  const int cut = point(rng);
  for (int i = 0; i < 20; ++i) {
    out.g[i] = (i < cut) ? a.g[i] : b.g[i];
  }
  return out;
}
} // namespace ttt::ga