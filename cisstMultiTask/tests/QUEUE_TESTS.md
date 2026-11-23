# Queue tests

Build and run the regular queue fixture from the repository root:

```sh
cmake --build build --target cisstMultiTaskTests
ctest --test-dir build -R mtsQueueTest --output-on-failure
```

The typed queue currently reserves one of its allocated slots; the generic
queue allocates an extra slot. Tests characterize those differing capacities
until a common capacity contract is chosen.

## Micro-benchmarks

With cisstMultiTask tests enabled, build and run the opt-in benchmarks:

```sh
cmake --build build --target cisstQueueBenchmark cisstQueueThreadedBenchmark
build/bin/cisstQueueBenchmark 10000 7
build/bin/cisstQueueThreadedBenchmark 200000 5
```

Arguments are optional: rounds or transfers per sample, followed by the number
of samples. Both use the interface's configured size of 64; the typed queues
currently hold 63 entries and the generic queue holds 64.

- **Single-thread fill/drain:** Measures blocking flags, return pointers,
	typed double arguments, and generic double arguments. Each transfer includes
	Put, Peek, verification and Get. Each sample fills and drains the queue in
	batches; allocation and 100 warmup rounds are outside the timed interval.
- **Two-thread SPSC:** Measures typed and generic integer queues against bounded
	`std::deque<int>` queues with matching effective capacities. One producer and
	one consumer transfer a checked sequence, with 10,000 warmup transfers and a
	cooperative 30-second timeout per sample. An external mutex protects every
	queue operation, including the read and removal of a slot. Full/empty retries
	yield the thread and are reported for the median-time sample.

Both report wall-clock nanoseconds per completed transfer and throughput, not
per-operation or end-to-end item latency. The threaded time includes mutex
contention, scheduling, retries and verification; thread construction, joins,
and queue construction are outside the measured interval. For an even sample
count the threaded benchmark reports the upper middle sample as its median.
No mailbox execution is measured. The mutex-protected results are not an
unlocked SPSC baseline: the current implementation has data races, and its
lifetime defects mean all measurements are exploratory. Use Release builds
and record compiler, hardware, affinity and load when comparing results.

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

These are a snapshot, not performance targets or evidence that one queue is
consistently faster. Re-run under comparable conditions before drawing conclusions.

Additional regression cases are excluded from the default build and disabled
in CTest because they exercise known defects, including undefined behavior.
Build them explicitly and run each case in a separate process:

```sh
cmake --build build --target cisstQueueRegressionTests
build/bin/cisstQueueRegressionTests typed-lifetime
build/bin/cisstQueueRegressionTests generic-default
build/bin/cisstQueueRegressionTests typed-concurrent
build/bin/cisstQueueRegressionTests generic-concurrent
```

The lifetime cases currently fail or crash. The concurrent cases race under
the C++ memory model, so a successful run does not establish correctness.
After the queue is fixed, enable these CTest cases and include the regression
target in the normal build. Run the concurrent cases with ThreadSanitizer in
a separately configured build that instruments both the test and libraries;
run the lifetime cases with AddressSanitizer and UndefinedBehaviorSanitizer.

Build with asan/ubsan:
cmake -B build_asan \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCISST_BUILD_TESTS=On \
  -DCMAKE_C_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" \
  -DCMAKE_CXX_FLAGS="-fsanitize=address,undefined -fno-omit-frame-pointer" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address,undefined" \
  -DCMAKE_SHARED_LINKER_FLAGS="-fsanitize=address,undefined"

ASAN_OPTIONS=detect_leaks=0 cmake --build build_asan --target cisstMultiTaskTests cisstQueueRegressionTests cisstQueueBenchmark cisstQueueThreadedBenchmark -j 24

Build with tsan:
cmake -B build_tsan \
  -DCMAKE_BUILD_TYPE=RelWithDebInfo \
  -DCISST_BUILD_TESTS=On \
  -DCMAKE_C_FLAGS="-fsanitize=thread -fno-omit-frame-pointer" \
  -DCMAKE_CXX_FLAGS="-fsanitize=thread -fno-omit-frame-pointer" \
  -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=thread" \
  -DCMAKE_SHARED_LINKER_FLAGS="-fsanitize=thread"
setarch $(uname -m) -R cmake --build build_tsan --target cisstMultiTaskTests cisstQueueRegressionTests cisstQueueBenchmark cisstQueueThreadedBenchmark -j 24
