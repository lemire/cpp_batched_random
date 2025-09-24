
/**
 * This header contains C++ templates that shuffle the elements in the range [first,
 * last) using the random number generator g. They are meant to emulate
 * the standard std::shuffle function and can often act as drop-in replacement.
 */
#ifndef TEMPLATE_SHUFFLE_H
#define TEMPLATE_SHUFFLE_H
 
#include <iterator>
#include <concepts>
#include <type_traits>
#include <limits>

#if defined(MSVC) && !defined(__clang__)
#include <__msvc_int128.hpp>
#endif

// This code is meant to look like the C++ standard library.
namespace batched_random {


template <class random_it, class URBG>
void shuffle(random_it first, random_it last, URBG &&g) {
    // Ensure the random number generator produces 64-bit unsigned integers
    static_assert(std::is_same<typename std::remove_reference<URBG>::type::result_type, uint64_t>::value, "result_type must be uint64_t");
    static_assert(std::remove_reference<URBG>::type::min() == 0, "min() must be 0");
    static_assert(std::remove_reference<URBG>::type::max() == std::numeric_limits<uint64_t>::max(), "max() must be the maximum uint64_t value");
    
    // Calculate the number of elements to shuffle
    uint64_t i = std::distance(first, last);
#if defined(MSVC) && !defined(__clang__)
    using our_uint128 = _Unsigned128;
#else
    using our_uint128 = __uint128_t;
#endif
    // Local struct to hide the partial shuffle function
    struct partial_shuffle {
        __attribute__((always_inline)) static uint64_t shuffle(random_it storage, uint64_t n, uint64_t k, uint64_t bound, URBG& gen) {
            // Use 128-bit arithmetic to avoid overflow in random number scaling
            our_uint128 x;
            // Get a random 64-bit value from the generator
            uint64_t r = gen();
            // Store indices for swapping (k <= 7, so fixed-size array is safe)
            uint64_t indexes[7];
            // Generate k random indices using the division method
            for (uint64_t j = 0; j < k; j++) {
                // Scale random number to select an index in [0, n-j)
                x = (our_uint128)(n - j) * (our_uint128)r;
                r = (uint64_t)x; // Lower 64 bits for next iteration
                indexes[j] = (uint64_t)(x >> 64); // Upper 64 bits give the index
            }
            // Check for bias in random number generation
            [[unlikely]] if (r < bound) {
                // Recalculate bound as n * (n-1) * ... * (n-(k-1))
                bound = n;
                for (uint64_t j = 1; j < k; j++) {
                    bound *= n - j;
                }
                // Compute threshold to reject biased random numbers
                uint64_t t = -bound % bound;
                // Regenerate random numbers until unbiased
                while (r < t) {
                    r = gen();
                    for (uint64_t j = 0; j < k; j++) {
                        x = (our_uint128)(n - j) * (our_uint128)r;
                        r = (uint64_t)x;
                        indexes[j] = (uint64_t)(x >> 64);
                    }
                }
            }
            // Perform swaps to shuffle k elements
            for (uint64_t j = 0; j < k; j++) {
                std::iter_swap(storage + n - j - 1, storage + indexes[j]);
            }
            return bound;
        }
    };

    // Process large arrays (above 2^30 elements) one element at a time
    for (; i > 1 << 30; i--) {
        partial_shuffle::shuffle(first, i, 1, i, g);
    }
    // Batches of 2 for sizes up to 2^30 elements
    uint64_t bound = (uint64_t)1 << 60;
    for (; i > 1 << 19; i -= 2) {
        bound = partial_shuffle::shuffle(first, i, 2, bound, g);
    }
    // Batches of 3 for sizes up to 2^19 elements
    bound = (uint64_t)1 << 57;
    for (; i > 1 << 14; i -= 3) {
        bound = partial_shuffle::shuffle(first, i, 3, bound, g);
    }
    // Batches of 4 for sizes up to 2^14 elements
    bound = (uint64_t)1 << 56;
    for (; i > 1 << 11; i -= 4) {
        bound = partial_shuffle::shuffle(first, i, 4, bound, g);
    }
    // Batches of 5 for sizes up to 2^11 elements
    bound = (uint64_t)1 << 55;
    for (; i > 1 << 9; i -= 5) {
        bound = partial_shuffle::shuffle(first, i, 5, bound, g);
    }
    // Batches of 6 for sizes up to 2^9 elements
    bound = (uint64_t)1 << 54;
    for (; i > 6; i -= 6) {
        bound = partial_shuffle::shuffle(first, i, 6, bound, g);
    }
    // Handle remaining elements (2 to 6) in a single batch
    if (i > 1) {
        partial_shuffle::shuffle(first, i, i - 1, 720, g);
    }
}

} // namespace batched_random

#endif // TEMPLATE_SHUFFLE_H
