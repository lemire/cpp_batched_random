#include <array>
#include <bitset>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <limits>
#include <random>

#include "batched_random.h"

/***
 * How do we test a shuffle function?
 * There are many tests that one could apply to a shuffle function.
 */

template <class URBG>
bool everyone_can_move_everywhere(URBG &gen) {
  constexpr size_t size = 512;
  uint64_t input[size];
  std::bitset<size> bits[size];
  for (size_t trial = 0; trial < size * size; trial++) {
    // We always start from the same input.
    std::iota(input, input + size, 0);
    // We shuffle:
    batched_random::shuffle(input, input + size, gen);
    // Mark that at position i we found value input[i].
    for (size_t i = 0; i < size; i++) {
      bits[i][input[i]] = 1;
    }
  }
  for (const std::bitset<size> &b : bits) {
    if (!b.all()) {
      return false;
    }
  }
  return true;
}

template <class URBG>
bool uniformity_test(URBG &gen) {
  constexpr size_t size = 512;
  uint64_t input[size];
  std::array<size_t, size> bits[size]{};
  size_t volume = size * size;
  for (size_t trial = 0; trial < volume; trial++) {
    // We always start from the same input.
    std::iota(input, input + size, 0);
    // We shuffle:
    batched_random::shuffle(input, input + size, gen);
    // Mark that at position i we found value input[i].
    for (size_t i = 0; i < size; i++) {
      bits[i][input[i]] += 1;
    }
  }
  size_t overall_min {std::numeric_limits<size_t>::max() };
  size_t overall_max = 0;
  size_t average = 0;

  for (const std::array<size_t, size> &b : bits) {
    average += std::accumulate(b.begin(), b.end(), 0);
    size_t max_value = *std::max_element(b.begin(), b.end());
    size_t min_value = *std::min_element(b.begin(), b.end());
    if (max_value > overall_max) {
      overall_max = max_value;
    }
    if (min_value < overall_min) {
      overall_min = min_value;
    }
  }
  size_t gap = overall_max - overall_min;
  double mean = (double)average / volume;
  double relative_gap = (double)gap / mean;

  printf("relative gap: %f, ", relative_gap);

  return relative_gap < 0.6;
}

template <class URBG>
bool any_possible_pair_at_the_start(URBG &gen) {
  constexpr size_t size = 64;
  uint64_t input[size];
  std::bitset<size * size> bits;
  for (size_t trial = 0; trial < size * size * size; trial++) {
    // We always start from the same input.
    std::iota(input, input + size, 0);
    // We shuffle:
    batched_random::shuffle(input, input + size, gen);
    bits[input[0] * size + input[1]] = 1;
  }
  for (size_t i = 0; i < size; i++) {
    for (size_t j = 0; j < size; j++) {
      if (i == j) {
        if (bits[i * size + j]) {
          return false;
        }
      } else {
        if (!bits[i * size + j]) {
          return false;
        }
      }
    }
  }
  return true;
}

template <class URBG>
bool any_possible_pair_at_the_end(URBG &gen) {
  constexpr size_t size = 64;
  uint64_t input[size];
  std::bitset<size * size> bits;
  for (size_t trial = 0; trial < size * size * size; trial++) {
    // We always start from the same input.
    std::iota(input, input + size, 0);
    // We shuffle:
    batched_random::shuffle(input, input + size, gen);
    bits[input[0] * size + input[1]] = 1;
  }
  for (size_t i = 0; i < size; i++) {
    for (size_t j = 0; j < size; j++) {
      if (i == j) {
        if (bits[i * size + j]) {
          return false;
        }
      } else {
        if (!bits[i * size + j]) {
          return false;
        }
      }
    }
  }
  return true;
}



template <class URBG>
bool test_everyone_can_move_everywhere(const std::string &gen_name, URBG &gen) {
  std::cout << __FUNCTION__ << " for " << gen_name << std::endl;
  std::cout << std::setw(40) << gen_name << ": ";
  std::cout.flush();
  if (!everyone_can_move_everywhere(gen)) {
    std::cerr << "!!!Test failed for " << gen_name << std::endl;
    return false;
  } else {
    std::cout << "passed" << std::endl;
  }
  return true;
}

template <class URBG>
bool test_uniformity_test(const std::string &gen_name, URBG &gen) {
  std::cout << __FUNCTION__ << " for " << gen_name << std::endl;
  std::cout << std::setw(40) << gen_name << ": ";
  std::cout.flush();
  if (!uniformity_test(gen)) {
    std::cerr << "!!!Test failed for " << gen_name << std::endl;
    return false;
  } else {
    std::cout << "passed" << std::endl;
  }
  return true;
}

template <class URBG>
bool test_any_possible_pair_at_the_start(const std::string &gen_name, URBG &gen) {
  std::cout << __FUNCTION__ << " for " << gen_name << std::endl;
  std::cout << std::setw(40) << gen_name << ": ";
  std::cout.flush();
  if (!any_possible_pair_at_the_start(gen)) {
    std::cerr << "!!!Test failed for " << gen_name << std::endl;
    return false;
  } else {
    std::cout << "passed" << std::endl;
  }
  return true;
}

template <class URBG>
bool test_any_possible_pair_at_the_end(const std::string &gen_name, URBG &gen) {
  std::cout << __FUNCTION__ << " for " << gen_name << std::endl;
  std::cout << std::setw(40) << gen_name << ": ";
  std::cout.flush();
  if (!any_possible_pair_at_the_end(gen)) {
    std::cerr << "!!!Test failed for " << gen_name << std::endl;
    return false;
  } else {
    std::cout << "passed" << std::endl;
  }
  return true;
}

int main() {
  std::random_device rd;
  std::mt19937_64 mtGenerator{rd()};
  std::ranlux48_base ranluxGenerator{rd()};

  bool success = true;
  success &= test_uniformity_test("mt19937_64", mtGenerator);
  success &= test_any_possible_pair_at_the_end("mt19937_64", mtGenerator);
  success &= test_any_possible_pair_at_the_start("mt19937_64", mtGenerator);
  success &= test_everyone_can_move_everywhere("mt19937_64", mtGenerator);

  if (success) {
    std::cout << "All tests passed" << std::endl;
  } else {
    std::cerr << "Some tests failed" << std::endl;
  }
  return success ? EXIT_SUCCESS : EXIT_FAILURE;
}
