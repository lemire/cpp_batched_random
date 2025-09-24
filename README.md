## C++ Batched Random

Fast shuffling functions using batched random index generation.


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