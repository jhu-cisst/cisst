# Queue tests

Build and run the regular queue fixture from the repository root:

```sh
cmake --build build --target cisstMultiTaskTests
ctest --test-dir build -R mtsQueueTest --output-on-failure
```

## Micro-benchmarks

With cisstMultiTask tests enabled, build and run the opt-in benchmarks:

```sh
cmake --build build --target cisstQueueBenchmark cisstQueueThreadedBenchmark
build/bin/cisstQueueBenchmark 10000 7
build/bin/cisstQueueThreadedBenchmark 200000 5
```

### Current results

Measured 2026-11-23 on Linux x86_64, Intel Core i9-14900K, GCC 11.5.0,
Release (`-O3 -DNDEBUG`), without CPU affinity or controlled system load.
The commands above were used. Values are median ns/transfer (min-max) and million
transfers per second; timings vary considerably with scheduling and CPU frequency.

| Single-thread workload | Usable | Median (min-max) | M transfers/s |
| --- | ---: | ---: | ---: |
| Blocking flag | 63 | 1.03 (1.01-1.89) | 968.35 |
| Return pointer | 63 | 1.16 (1.15-1.17) | 862.37 |
| Typed double argument | 63 | 2.19 (2.15-2.40) | 457.45 |
| Generic double argument | 64 | 5.07 (5.06-5.21) | 197.25 |

| Two-thread workload | Usable | Median (min-max) | M transfers/s | Full / empty retries |
| --- | ---: | ---: | ---: | ---: |
| Typed integer | 63 | 118.82 (93.48-125.60) | 8.42 | 7191 / 1694 |
| `std::deque`, typed capacity | 63 | 311.04 (302.87-375.88) | 3.22 | 14993 / 1016 |
| Generic integer | 64 | 201.75 (156.07-246.58) | 4.96 | 1523 / 11637 |
| `std::deque`, generic capacity | 64 | 337.85 (281.34-368.00) | 2.96 | 12014 / 584 |

### Sanitizer

Tests should be run under the Address, Undefined Behavior, Memory, and Thread sanitizers. It's recommended to use `clang` to compile the tests as it has the best support for these sanitizers.

Build with asan/ubsan:
```
cmake -B build_asan \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCISST_BUILD_TESTS=ON \
  -DCISST_USE_EXTERNAL=ON \
  -DCMAKE_C_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" \
  -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined" \
  -DCMAKE_SHARED_LINKER_FLAGS="-fsanitize=address,undefined"
```
```
ASAN_OPTIONS=detect_leaks=0 cmake --build build_asan -j 24
```

Build with msan:
```
cmake -B build_msan \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCISST_BUILD_TESTS=ON \
  -DCISST_USE_EXTERNAL=ON \
  -DCMAKE_C_FLAGS="-fsanitize=memory -fsanitize-memory-track-origins=2 -fno-omit-frame-pointer" \
  -DCMAKE_CXX_FLAGS="-fsanitize=memory -fsanitize-memory-track-origins=2 -fno-omit-frame-pointer"
```
```
MSAN_OPTIONS="exitcode=0:halt_on_error=0" cmake --build build_msan -j 24
```

Build with tsan:
```
cmake -B build_tsan \
  -DCMAKE_C_COMPILER=clang \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCISST_BUILD_TESTS=ON \
  -DCISST_USE_EXTERNAL=ON \
  -DCMAKE_C_FLAGS="-fsanitize=thread -fno-omit-frame-pointer" \
  -DCMAKE_CXX_FLAGS="-fsanitize=thread -fno-omit-frame-pointer" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=thread" \
  -DCMAKE_SHARED_LINKER_FLAGS="-fsanitize=thread"
```
```
setarch $(uname -m) -R cmake --build build_tsan -j 24
```