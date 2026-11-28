#include <cisstMultiTask/mtsInterfaceRequired.h>
#include <cisstMultiTask/mtsQueue.h>

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace {

const size_t configuredSize = mtsInterfaceRequired::DEFAULT_MAIL_BOX_AND_ARGUMENT_QUEUES_SIZE;

class FlagQueue
{
    mtsQueue<mtsBlockingType> queue;
public:
    FlagQueue(): queue(configuredSize, MTS_NOT_BLOCKING) {}
    bool Put(size_t index) {
        return queue.Put(index % 2 ? MTS_BLOCKING : MTS_NOT_BLOCKING) != 0;
    }
    bool Consume(size_t index) {
        const mtsBlockingType * value = queue.Peek();
        if (!value || *value != (index % 2 ? MTS_BLOCKING : MTS_NOT_BLOCKING)) { return false; }
        return queue.Pop();
    }
};

class ReturnQueue
{
    mtsInt first;
    mtsInt second;
    mtsQueue<mtsGenericObject *> queue;
public:
    ReturnQueue(): first(0), second(0), queue(configuredSize, 0) {}
    bool Put(size_t index) { return queue.Put(index % 2 ? &first : &second) != 0; }
    bool Consume(size_t index) {
        mtsGenericObject ** value = queue.Peek();
        if (!value || *value != (index % 2 ? &first : &second)) { return false; }
        return queue.Pop();
    }
};

class TypedArgumentQueue
{
    mtsQueue<mtsDouble> queue;
public:
    TypedArgumentQueue(): queue(configuredSize, mtsDouble(0.0)) {}
    bool Put(size_t index) { return queue.Put(mtsDouble(static_cast<double>(index))) != 0; }
    bool Consume(size_t index) {
        const mtsDouble * value = queue.Peek();
        if (!value || value->Data != static_cast<double>(index)) { return false; }
        return queue.Pop();
    }
};

class GenericArgumentQueue
{
    mtsQueueGeneric queue;
public:
    GenericArgumentQueue(): queue(configuredSize, mtsDouble(0.0)) {}
    bool Put(size_t index) { return queue.Put(mtsDouble(static_cast<double>(index))) != 0; }
    bool Consume(size_t index) {
        const mtsDouble * value = static_cast<const mtsDouble *>(queue.Peek());
        if (!value || value->Data != static_cast<double>(index)) { return false; }
        return queue.Pop();
    }
};

template <class Queue>
bool RunBatches(Queue & queue, size_t capacity, size_t rounds)
{
    for (size_t round = 0; round < rounds; ++round) {
        for (size_t index = 0; index < capacity; ++index) {
            if (!queue.Put(index + round)) { return false; }
        }
        for (size_t index = 0; index < capacity; ++index) {
            if (!queue.Consume(index + round)) { return false; }
        }
    }
    return true;
}

template <class Queue>
bool Benchmark(const char * name, size_t capacity, size_t rounds, size_t repetitions)
{
    Queue queue;
    
    // Warmup
    if (!RunBatches(queue, capacity, 100)) { return false; }

    std::vector<double> samples;
    for (size_t repetition = 0; repetition < repetitions; ++repetition) {
        const auto start = std::chrono::steady_clock::now();
        if (!RunBatches(queue, capacity, rounds)) { return false; }
        const auto end = std::chrono::steady_clock::now();
        const double elapsed = std::chrono::duration<double, std::nano>(end - start).count();
        samples.push_back(elapsed / (static_cast<double>(rounds) * capacity));
    }

    std::sort(samples.begin(), samples.end());
    const double median = samples.size() % 2 ? samples[samples.size() / 2]
        : (samples[samples.size() / 2 - 1] + samples[samples.size() / 2]) / 2.0;
    std::cout << std::left << std::setw(20) << name << " usable=" << std::setw(3) << capacity
              << " ns/transfer median=" << std::fixed << std::setprecision(2) << median
              << " min=" << samples.front() << " max=" << samples.back()
              << " Mtransfers/s=" << (1000.0 / median) << '\n';
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
    size_t rounds = 10000;
    size_t repetitions = 7;
    if (argc > 3 || (argc > 1 && !ParsePositive(argv[1], rounds))
        || (argc > 2 && !ParsePositive(argv[2], repetitions)) || repetitions > 1000) {
        std::cerr << "Usage: " << argv[0] << " [rounds > 0] [repetitions 1..1000]" << std::endl;
        return 2;
    }
    std::cout << "Single-thread fill/drain; configured size=" << configuredSize
              << ", rounds=" << rounds << ", repetitions=" << repetitions
              << ", warmup rounds=100\n"
              << "Each transfer includes Put, Peek, value verification and Pop.\n";
    if (!Benchmark<FlagQueue>("blocking flag", configuredSize, rounds, repetitions)
        || !Benchmark<ReturnQueue>("return pointer", configuredSize, rounds, repetitions)
        || !Benchmark<TypedArgumentQueue>("typed argument", configuredSize, rounds, repetitions)
        || !Benchmark<GenericArgumentQueue>("generic argument", configuredSize, rounds, repetitions)) {
        std::cerr << "Queue operation or FIFO verification failed" << std::endl;
        return 1;
    }
    return 0;
}