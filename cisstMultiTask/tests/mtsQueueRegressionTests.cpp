#include <cisstMultiTask/mtsQueue.h>

#include <atomic>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

namespace {

// Tracks number of constructions and destructions
class LifetimeValue
{
public:
    static std::atomic<int> constructions;
    static std::atomic<int> destructions;

    LifetimeValue() { ++constructions; }
    LifetimeValue(const LifetimeValue &) { ++constructions; }
    ~LifetimeValue() { ++destructions; }
};

std::atomic<int> LifetimeValue::constructions = 0;
std::atomic<int> LifetimeValue::destructions = 0;

bool TestTypedLifetime()
{
    LifetimeValue::constructions = 0;
    LifetimeValue::destructions = 0;
    {
        LifetimeValue prototype;
        mtsQueue<LifetimeValue> queue(4, prototype);
        queue.SetSize(2, prototype);
    }
    if (LifetimeValue::constructions != LifetimeValue::destructions) {
        std::cerr << "Unbalanced constructions and destructions: "
                  << LifetimeValue::constructions << " versus "
                  << LifetimeValue::destructions << std::endl;
        return false;
    }
    return true;
}

bool TestGenericDefault()
{
    mtsQueueGeneric queue;
    return queue.GetSize() == 0 && queue.IsEmpty();
}

class TypedQueue
{
    mtsQueue<int> queue;
public:
    explicit TypedQueue(size_t capacity): queue(capacity, 0) {}
    bool Put(int value) { return queue.Put(value) != 0; }
    bool Peek(int & value) {
        const int * element = queue.Peek();
        if (!element) { return false; }
        value = *element;
        return true;
    }
    void Pop() { queue.Pop(); }
};

class GenericQueue
{
    mtsQueueGeneric queue;
public:
    explicit GenericQueue(size_t capacity): queue(capacity, mtsInt(0)) {}
    bool Put(int value) { return queue.Put(mtsInt(value)) != 0; }
    bool Peek(int & value) {
        const mtsInt * element = dynamic_cast<const mtsInt *>(queue.Peek());
        if (!element) { return false; }
        value = element->Data;
        return true;
    }
    void Pop() { queue.Pop(); }
};

template <class Queue>
bool TestConcurrent()
{
    const int count = 100000;
    const size_t capacities[] = {1, 2, 7};
    for (size_t capacity : capacities) {
        Queue queue(capacity);
        std::atomic<bool> failed(false);
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(10);
        std::thread producer([&]() {
            for (int sequence = 0; sequence < count; ++sequence) {
                while (!queue.Put(sequence)) {
                    if (failed.load() || std::chrono::steady_clock::now() >= deadline) {
                        failed.store(true);
                        return;
                    }
                    std::this_thread::yield();
                }
            }
        });
        std::thread consumer([&]() {
            for (int expected = 0; expected < count; ++expected) {
                int actual = -1;
                while (!queue.Peek(actual)) {
                    if (failed.load() || std::chrono::steady_clock::now() >= deadline) {
                        failed.store(true);
                        return;
                    }
                    std::this_thread::yield();
                }
                if (actual != expected) {
                    std::cerr << "Capacity " << capacity << ": expected " << expected
                              << ", got " << actual << std::endl;
                    failed.store(true);
                    return;
                }
                queue.Pop();
            }
        });
        producer.join();
        consumer.join();
        if (failed.load()) {
            std::cerr << "Concurrent queue test failed or timed out" << std::endl;
            return false;
        }
    }
    return true;
}

}

int main(int argc, char ** argv)
{
    if (argc != 2) {
        std::cerr << "Expected a queue regression test name" << std::endl;
        return 2;
    }
    const std::string test(argv[1]);
    if (test == "typed-lifetime") { return TestTypedLifetime() ? 0 : 1; }
    if (test == "generic-default") { return TestGenericDefault() ? 0 : 1; }
    if (test == "typed-concurrent") { return TestConcurrent<TypedQueue>() ? 0 : 1; }
    if (test == "generic-concurrent") { return TestConcurrent<GenericQueue>() ? 0 : 1; }
    std::cerr << "Unknown queue regression test: " << test << std::endl;
    return 2;
}