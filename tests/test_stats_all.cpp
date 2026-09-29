#include <cstdio>
#include <cstdlib>

int main(int argc, char *argv[]) {
  // пробрасываем seed, если задан
  const char *seed = (argc >= 2) ? argv[1] : "0";

  char cmd[256];

  std::printf("========================================\n");
  std::printf(" Running self-play test\n");
  std::printf("========================================\n");
  std::snprintf(cmd, sizeof(cmd), "./tests/test_stats_self %s", seed);
  int r1 = std::system(cmd);

  std::printf("\n========================================\n");
  std::printf(" Running test vs BaselineEasy\n");
  std::printf("========================================\n");
  std::snprintf(cmd, sizeof(cmd), "./tests/test_stats_easy %s", seed);
  int r2 = std::system(cmd);

  std::printf("\n========================================\n");
  std::printf(" Running test vs BaselineHard\n");
  std::printf("========================================\n");
  std::snprintf(cmd, sizeof(cmd), "./tests/test_stats_hard %s", seed);
  int r3 = std::system(cmd);

  std::printf("\n=== DONE ===\n");
  std::printf("self-play rc=%d, vs_easy rc=%d, vs_hard rc=%d\n", r1, r2, r3);
  return (r1 == 0 && r2 == 0 && r3 == 0) ? 0 : 1;
}