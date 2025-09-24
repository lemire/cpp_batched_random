
#include "performancecounters/benchmarker.h"
#include <algorithm>
#include <charconv>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <stdlib.h>
#include <vector>
#include "batched_random.h"

void pretty_print(size_t volume, size_t bytes, std::string name,
                  event_aggregate agg) {
  printf("%-45s : ", name.c_str());
  printf(" %5.2f Gi/s ", volume / agg.fastest_elapsed_ns());
  double best_speed = volume / agg.fastest_elapsed_ns();
  double avg_speed = volume / agg.elapsed_ns();
  double range = (best_speed - avg_speed) / avg_speed * 100.0;
  printf(" %5.2f GB/s best, %5.2f GB/s average, (%2.0f %%) ",
         bytes / agg.fastest_elapsed_ns(), bytes / agg.elapsed_ns(), range);
  if (collector.has_events()) {
    printf(" %5.2f GHz ", agg.fastest_cycles() / agg.fastest_elapsed_ns());
    printf(" %5.2f c/b ", agg.fastest_cycles() / bytes);
    printf(" %5.2f i/b ", agg.fastest_instructions() / bytes);
    printf(" %5.2f i/e ", agg.fastest_instructions() / volume);
    printf(" %5.2f i/c ", agg.fastest_instructions() / agg.fastest_cycles());
  }
  printf("\n");
}

void bench(size_t size) {
  constexpr size_t min_volume = 4096;
  if (size == 0) {
    return;
  }
  size_t volume = size;
  if (size < min_volume) {
    volume *= min_volume / size;
  }
  std::vector<uint64_t> input(volume);
  std::random_device rd;

  if (size > 0xFFFFFFFF) {
    std::cerr << "WARNING: Volume too large for precomputed shuffle."
              << std::endl;
  }

  std::cout << "Size of precomputed values " << size * sizeof(uint32_t) / 1024
            << " kB" << std::endl;
  std::cout << "Size of shuffle      : " << size << " words" << std::endl;
  std::cout << "Size of shuffle      : "
            << size * sizeof(uint64_t) / 1024 / 1024. << " MB" << std::endl;

  size_t min_repeat = 10;
  size_t min_time_ns = 100000000;
  size_t max_repeat = 100000;

  if (size < volume) {
    printf("inner repeat: %zu\n", volume / size);
  }

    std::mt19937_64 mtGenerator{rd()};


    // C++ Mersenne twister
    std::cout << "=== C++ Mersenne Twister" << std::endl;

    pretty_print(
        volume, volume * sizeof(uint64_t), "C++ std::shuffle (mersenne)",
        bench(
            [&input, &mtGenerator, size]() {
              for (auto t = input.begin(); t < input.end(); t += size) {
                std::shuffle(t, t + size, mtGenerator);
              }
            },
            min_repeat, min_time_ns, max_repeat));
    pretty_print(
        volume, volume * sizeof(uint64_t), "C++ batched_random::shuffle (mersenne)",
        bench(
            [&input, &mtGenerator, size]() {
              for (auto t = input.begin(); t < input.end(); t += size) {
                batched_random::shuffle(t, t + size, mtGenerator);
              }
            },
            min_repeat, min_time_ns, max_repeat));

  

}

int main(int argc, char **argv) {
  // We want to make sure we extend the range far enough to see regressions
  // for large arrays, if any.
  for (size_t i = 1 << 6; i <= 1 << 20; i <<= 1) {
    bench(i);
    std::cout << std::endl;
  }

  return EXIT_SUCCESS;
}
