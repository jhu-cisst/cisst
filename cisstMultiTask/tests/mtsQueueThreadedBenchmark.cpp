#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsQueue.h>

#include <algorithm>
#include <atomic>
#include <cerrno>
#include <chrono>
#include <cstdlib>
#include <deque>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <thread>
#include <vector>

namespace {

const size_t configuredSize = mtsInterfaceRequired::DEFAULT_MAIL_BOX_AND_ARGUMENT_QUEUES_SIZE;
using Clock = std::chrono::steady_clock;

enum class ReadResult { Empty, Success, Mismatch };

class TypedQueue
{
    mtsQueue<int> queue;
public:
    explicit TypedQueue(size_t): queue(configuredSize, 0) {}
    bool Put(int value) { return queue.Put(value); }
    ReadResult Consume(int expected) {
        const int * value = queue.Peek();
        if (!value) { return ReadResult::Empty; }
        if (*value != expected) { return ReadResult::Mismatch; }
        return queue.Pop() ? ReadResult::Success : ReadResult::Mismatch;
    }
};

class GenericQueue
{
    mtsQueueGeneric queue;
public:
    explicit GenericQueue(size_t): queue(configuredSize, mtsInt(0)) {}
    bool Put(int value) { return queue.Put(mtsInt(value)); }
    ReadResult Consume(int expected) {
        const mtsInt * value = static_cast<const mtsInt *>(queue.Peek());
        if (!value) { return ReadResult::Empty; }
        if (value->Data != expected) { return ReadResult::Mismatch; }
        return queue.Pop() ? ReadResult::Success : ReadResult::Mismatch;
    }
};

class StandardQueue
{
    std::deque<int> queue;
    const size_t capacity;
    std::mutex mutex;

public:
    explicit StandardQueue(size_t capacity): capacity(capacity) {}
    bool Put(int value) {
        std::scoped_lock lock(mutex);
        if (queue.size() == capacity) { return false; }
        queue.push_back(value);
        return true;
    }

    ReadResult Consume(int expected) {
        std::scoped_lock lock(mutex);
        if (queue.empty()) { return ReadResult::Empty; }
        if (queue.front() != expected) { return ReadResult::Mismatch; }
        queue.pop_front();
        return ReadResult::Success;
    }
};

struct Sample
{
    double nanosecondsPerTransfer;
    size_t fullRetries;
    size_t emptyRetries;
};

template <class Queue>
bool RunSample(size_t capacity, int count, Sample & sample)
{
    Queue queue(capacity);
    std::atomic<int> ready(0);
    std::atomic<bool> start(false);
    std::atomic<bool> failed(false);
    size_t fullRetries = 0;
    size_t emptyRetries = 0;
    Clock::time_point deadline;
    Clock::time_point finished;

    std::thread producer([&]() {
        ready.fetch_add(1);
        while (!start.load(std::memory_order_acquire)) { std::this_thread::yield(); }
        size_t attempts = 0;
        for (int sequence = 0; sequence < count;) {
            if (failed.load(std::memory_order_relaxed)) { return; }
            if (++attempts % 1024 == 0 && Clock::now() >= deadline) {
                failed.store(true);
                return;
            }
            bool inserted = queue.Put(sequence);
            if (inserted) { ++sequence; }
            else { ++fullRetries; std::this_thread::yield(); }
        }
    });
    std::thread consumer([&]() {
        ready.fetch_add(1);
        while (!start.load(std::memory_order_acquire)) { std::this_thread::yield(); }
        size_t attempts = 0;
        for (int expected = 0; expected < count;) {
            if (failed.load(std::memory_order_relaxed)) { return; }
            if (++attempts % 1024 == 0 && Clock::now() >= deadline) {
                failed.store(true);
                return;
            }
            ReadResult result = queue.Consume(expected);
            if (result == ReadResult::Success) { ++expected; }
            else if (result == ReadResult::Empty) { ++emptyRetries; std::this_thread::yield(); }
            else { failed.store(true); return; }
        }
        finished = Clock::now();
    });
    while (ready.load() != 2) { std::this_thread::yield(); }
    const Clock::time_point started = Clock::now();
    deadline = started + std::chrono::seconds(30);
    start.store(true, std::memory_order_release);
    producer.join();
    consumer.join();
    if (failed.load()) { return false; }
    sample.nanosecondsPerTransfer = std::chrono::duration<double, std::nano>(finished - started).count() / count;
    sample.fullRetries = fullRetries;
    sample.emptyRetries = emptyRetries;
    return true;
}

template <class Queue>
bool Benchmark(const char * name, size_t capacity, int count, size_t repetitions)
{
    Sample warmup;
    if (!RunSample<Queue>(capacity, 10000, warmup)) { return false; }
    std::vector<Sample> samples;
    for (size_t repetition = 0; repetition < repetitions; ++repetition) {
        Sample sample;
        if (!RunSample<Queue>(capacity, count, sample)) { return false; }
        samples.push_back(sample);
    }
    std::sort(samples.begin(), samples.end(), [](const Sample & left, const Sample & right) {
        return left.nanosecondsPerTransfer < right.nanosecondsPerTransfer;
    });
    const Sample & median = samples[samples.size() / 2];
    std::cout << std::left << std::setw(20) << name << " usable=" << std::setw(3) << capacity
              << " ns/transfer median=" << std::fixed << std::setprecision(2) << median.nanosecondsPerTransfer
              << " min=" << samples.front().nanosecondsPerTransfer << " max=" << samples.back().nanosecondsPerTransfer
              << " Mtransfers/s=" << (1000.0 / median.nanosecondsPerTransfer)
              << " full-retries=" << median.fullRetries << " empty-retries=" << median.emptyRetries << '\n';
    return true;
}

bool ParsePositive(const char * text, size_t & value)
{
    const std::string input(text);
    if (input.empty() || input.find_first_not_of("0123456789") != std::string::npos) { return false; }
    char * end = 0;
    errno = 0;
    const unsigned long long parsed = std::strtoull(text, &end, 10);
    if (errno == ERANGE || *end || parsed == 0 || parsed > std::numeric_limits<size_t>::max()) { return false; }
    value = static_cast<size_t>(parsed);
    return true;
}

}

int main(int argc, char ** argv)
{
    size_t count = 200000;
    size_t repetitions = 5;
    if (argc > 3 || (argc > 1 && !ParsePositive(argv[1], count))
        || (argc > 2 && !ParsePositive(argv[2], repetitions))
        || count > static_cast<size_t>(std::numeric_limits<int>::max()) || repetitions > 1000) {
        std::cerr << "Usage: " << argv[0] << " [transfers 1..INT_MAX] [repetitions 1..1000]" << std::endl;
        return 2;
    }
    std::cout << "Two-thread SPSC; configured size=" << configuredSize
              << ", transfers=" << count << ", repetitions=" << repetitions
              << ", warmup transfers=10000, timeout=30s/sample\n";
    const int transfers = static_cast<int>(count);
    if (!Benchmark<TypedQueue>("typed integer", configuredSize, transfers, repetitions)
        || !Benchmark<GenericQueue>("generic integer", configuredSize, transfers, repetitions)
        || !Benchmark<StandardQueue>("std::deque", configuredSize, transfers, repetitions)) {
        std::cerr << "FIFO verification failed or sample timed out" << std::endl;
        return 1;
    }
    return 0;
}
