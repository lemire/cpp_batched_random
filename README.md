## C++ Batched Random

Fast shuffling functions using batched random index generation.




This project contains a single header `batched_random.h`. It defines a single template function `batched_random::shuffle` which can serve as a drop-in replacement for `std::shuffle` when the random number generator produces 64-bit integers (e.g., `std::mt19937_64`). It can be significantly faster than the standard `std::shuffle` in many cases. We include tests and benchmarks.


### Reference

* Nevin Brackett-Rozinsky, Daniel Lemire, [Batched Ranged Random Integer Generation](https://arxiv.org/abs/2408.06213), Software: Practice and Experience 55 (1), 2024.

### Usage

For Linux and macOS: 

```
cmake -B build
cmake --build build
ctest --test-dir build
./build/benchmark
```

For Windows: 


```
cmake -B build
cmake --build build --config Release
ctest --test-dir build -C Release
./build/Release/benchmark
```
