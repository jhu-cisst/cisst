/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */

/*

  Author(s):  Anton Deguet
  Created on: 2009-04-29

  (C) Copyright 2009-2026 Johns Hopkins University (JHU), All Rights Reserved.

--- begin cisst license - do not edit ---

This software is provided "as is" under an open source license, with
no warranty.  The complete license can be found in license.txt and
http://www.cisst.org/cisst/license.txt.

--- end cisst license ---
*/

#include "mtsQueueTest.h"
#include "mtsMacrosTestClasses.h"

#include <atomic>
#include <chrono>
#include <deque>
#include <iostream>
#include <limits>
#include <random>
#include <stdexcept>
#include <thread>


namespace {

// Copying throws once Budget copies/assignments have been spent; Alive tracks leaks.
struct ThrowingValue
{
    static int Budget;
    static int Alive;
    int Value;

    explicit ThrowingValue(int value = 0): Value(value) { ++Alive; }
    ThrowingValue(const ThrowingValue & other): Value(other.Value) { Spend(); ++Alive; }
    ThrowingValue & operator=(const ThrowingValue & other) { Spend(); Value = other.Value; return *this; }
    ~ThrowingValue() { --Alive; }

    static void Spend() {
        if (Budget-- <= 0) {
            throw std::runtime_error("ThrowingValue: budget exhausted");
        }
    }
};

int ThrowingValue::Budget = 0;
int ThrowingValue::Alive = 0;

// Tracks number of constructions and destructions
class LifetimeValue
{
public:
    static std::atomic<int> constructions;
    static std::atomic<int> destructions;

    LifetimeValue() { ++constructions; }
    LifetimeValue(const LifetimeValue &) { ++constructions; }
    LifetimeValue* operator=(const LifetimeValue &) { return this; }
    ~LifetimeValue() { ++destructions; }
};

std::atomic<int> LifetimeValue::constructions = 0;
std::atomic<int> LifetimeValue::destructions = 0;

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

}

void mtsQueueTest::TestReservationCommit(void)
{
    mtsQueue<int> queue(2, 0);
    CPPUNIT_ASSERT(queue.Put(10));
    auto reservation = queue.Reserve(20);
    CPPUNIT_ASSERT(static_cast<bool>(reservation));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), queue.GetAvailable());
    CPPUNIT_ASSERT(!queue.IsFull());
    CPPUNIT_ASSERT(queue.Peek());
    CPPUNIT_ASSERT_EQUAL(10, *queue.Peek());
    CPPUNIT_ASSERT(!static_cast<bool>(queue.Reserve(30)));
    CPPUNIT_ASSERT(!queue.Put(30));
    reservation.commit();
    CPPUNIT_ASSERT(!static_cast<bool>(reservation));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), queue.GetAvailable());
    CPPUNIT_ASSERT(queue.IsFull());
    int value = 0;
    CPPUNIT_ASSERT(queue.Get(value));
    CPPUNIT_ASSERT_EQUAL(10, value);
    CPPUNIT_ASSERT(queue.Get(value));
    CPPUNIT_ASSERT_EQUAL(20, value);
    CPPUNIT_ASSERT(queue.IsEmpty());
}

void mtsQueueTest::TestReservationRollback(void)
{
    mtsQueue<int> queue(2, 0);
    CPPUNIT_ASSERT(queue.Put(10));
    auto reservation = queue.Reserve(20);
    CPPUNIT_ASSERT(static_cast<bool>(reservation));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), queue.GetAvailable());
    CPPUNIT_ASSERT_EQUAL(10, *queue.Peek());
    CPPUNIT_ASSERT(!static_cast<bool>(queue.Reserve(30)));
    CPPUNIT_ASSERT(!queue.Put(30));
    reservation.rollback();
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), queue.GetAvailable());
    CPPUNIT_ASSERT(queue.Put(30));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), queue.GetAvailable());
    CPPUNIT_ASSERT(queue.IsFull());
    int value = 0;
    CPPUNIT_ASSERT(queue.Get(value));
    CPPUNIT_ASSERT_EQUAL(10, value);
    CPPUNIT_ASSERT(queue.Get(value));
    CPPUNIT_ASSERT_EQUAL(30, value);
    CPPUNIT_ASSERT(queue.IsEmpty());

    {
        auto reservation2 = queue.Reserve(40);
        CPPUNIT_ASSERT(queue.IsEmpty());
        CPPUNIT_ASSERT(!queue.Put(50));
    }
    CPPUNIT_ASSERT(queue.IsEmpty());
    CPPUNIT_ASSERT(queue.Put(50));
}

void mtsQueueTest::TestTypedQueue(void)
{
    mtsQueue<int> queue(4, 0);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), queue.GetSize());
    CPPUNIT_ASSERT(queue.IsEmpty());
    CPPUNIT_ASSERT(!queue.IsFull());
    CPPUNIT_ASSERT(!queue.Peek());
    CPPUNIT_ASSERT(!queue.Pop());
    int element = -1;
    CPPUNIT_ASSERT(!queue.Get(element));
    CPPUNIT_ASSERT_EQUAL(-1, element);

    for (int value = 1; value <= 4; ++value) {
        CPPUNIT_ASSERT(queue.Put(value));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(value), queue.GetAvailable());
    }

    CPPUNIT_ASSERT(queue.IsFull());
    CPPUNIT_ASSERT(!queue.Put(5));
    CPPUNIT_ASSERT(queue.Peek());
    CPPUNIT_ASSERT_EQUAL(1, *queue.Peek());
    CPPUNIT_ASSERT_EQUAL(1, *queue.Peek());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), queue.GetAvailable());
    CPPUNIT_ASSERT(queue.Get(element));
    CPPUNIT_ASSERT_EQUAL(1, element);
    CPPUNIT_ASSERT(queue.Put(5));
    CPPUNIT_ASSERT(queue.IsFull());

    for (int value = 2; value <= 5; ++value) {
        CPPUNIT_ASSERT(queue.Get(element));
        CPPUNIT_ASSERT_EQUAL(value, element);
    }

    CPPUNIT_ASSERT(queue.IsEmpty());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), queue.GetAvailable());
}

void mtsQueueTest::TestTypedBoolQueue(void)
{
    mtsQueue<bool> queue(4, true);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), queue.GetSize());
    CPPUNIT_ASSERT(queue.IsEmpty());
    CPPUNIT_ASSERT(!queue.IsFull());
    CPPUNIT_ASSERT(!queue.Peek());
    CPPUNIT_ASSERT(!queue.Pop());
    bool element = false;
    CPPUNIT_ASSERT(!queue.Get(element));
    CPPUNIT_ASSERT_EQUAL(false, element);

    for (size_t count = 1; count <= 4; ++count) {
        CPPUNIT_ASSERT(queue.Put(static_cast<bool>(count % 2 == 0)));
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(count), queue.GetAvailable());
    }

    CPPUNIT_ASSERT(queue.IsFull());
    CPPUNIT_ASSERT(!queue.Put(true));
    CPPUNIT_ASSERT(queue.Peek());
    CPPUNIT_ASSERT_EQUAL(false, *queue.Peek());
    CPPUNIT_ASSERT_EQUAL(false, *queue.Peek());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(4), queue.GetAvailable());
    CPPUNIT_ASSERT(queue.Get(element));
    CPPUNIT_ASSERT_EQUAL(false, element);
    CPPUNIT_ASSERT(queue.Put(false));
    CPPUNIT_ASSERT(queue.IsFull());

    for (size_t count = 2; count <= 5; ++count) {
        CPPUNIT_ASSERT(queue.Get(element));
        CPPUNIT_ASSERT_EQUAL(static_cast<bool>(count % 2 == 0), element);
    }

    CPPUNIT_ASSERT(queue.IsEmpty());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), queue.GetAvailable());
}

void mtsQueueTest::TestTypedBoundariesAndResize(void)
{
    mtsQueue<int> queue(1, 0);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), queue.GetSize());
    CPPUNIT_ASSERT(queue.IsEmpty());
    CPPUNIT_ASSERT(!queue.IsFull());
    CPPUNIT_ASSERT(queue.Put(1));
    CPPUNIT_ASSERT(queue.IsFull());
    CPPUNIT_ASSERT(!queue.Put(2));
    CPPUNIT_ASSERT(queue.Peek());
    CPPUNIT_ASSERT_EQUAL(1, *queue.Peek());
    CPPUNIT_ASSERT(queue.Pop());
    CPPUNIT_ASSERT(queue.IsEmpty());

    queue.SetSize(3, 0);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), queue.GetSize());
    CPPUNIT_ASSERT(queue.Put(10));
    CPPUNIT_ASSERT(queue.Put(20));
    CPPUNIT_ASSERT(!queue.IsFull());
    CPPUNIT_ASSERT(queue.Put(30));
    CPPUNIT_ASSERT(queue.IsFull());

    queue.SetSize(2, 0);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), queue.GetSize());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), queue.GetAvailable());
    CPPUNIT_ASSERT(queue.IsEmpty());
    CPPUNIT_ASSERT(queue.Put(40));
    CPPUNIT_ASSERT(queue.Put(50));
    CPPUNIT_ASSERT(queue.IsFull());
    int element = -1;
    CPPUNIT_ASSERT(queue.Get(element));
    CPPUNIT_ASSERT_EQUAL(40, element);
}

void mtsQueueTest::TestTypedProxyReference(void)
{
    mtsDouble prototype = 0.0;
    mtsQueue<mtsDouble> queue(3, prototype);
    double source = 4.5;
    mtsGenericObjectProxyRef<double> reference(source);
    CPPUNIT_ASSERT(queue.Put(reference));
    source = 9.0;
    CPPUNIT_ASSERT(queue.Peek());
    CPPUNIT_ASSERT_EQUAL(4.5, queue.Peek()->Data);
    CPPUNIT_ASSERT(queue.Pop());
    CPPUNIT_ASSERT(queue.IsEmpty());
}

void mtsQueueTest::TestGenericBoundariesAndRejection(void)
{
    mtsDouble prototype = 0.0;
    mtsQueueGeneric queue(2, prototype);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), queue.GetSize());
    CPPUNIT_ASSERT(queue.IsEmpty());
    CPPUNIT_ASSERT(!queue.Peek());
    CPPUNIT_ASSERT(!queue.Pop());

    mtsDouble first = 1.0;
    mtsDouble second = 2.0;
    mtsDouble third = 3.0;
    mtsInt wrongType = 7;
    CPPUNIT_ASSERT(queue.Put(first));
    // TODO BB: Fix?
    // CPPUNIT_ASSERT(!queue.Put(wrongType));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(1), queue.GetAvailable());
    CPPUNIT_ASSERT(queue.Put(second));
    CPPUNIT_ASSERT(queue.IsFull());
    CPPUNIT_ASSERT(!queue.Put(third));
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), queue.GetAvailable());
    mtsDouble * retrieved = dynamic_cast<mtsDouble *>(queue.Peek());
    CPPUNIT_ASSERT(retrieved);
    CPPUNIT_ASSERT_EQUAL(1.0, retrieved->Data);
    CPPUNIT_ASSERT(queue.Pop());
    CPPUNIT_ASSERT(queue.Put(third));
    retrieved = dynamic_cast<mtsDouble *>(queue.Peek());
    CPPUNIT_ASSERT(retrieved);
    CPPUNIT_ASSERT_EQUAL(2.0, retrieved->Data);
    CPPUNIT_ASSERT(queue.Pop());
    retrieved = dynamic_cast<mtsDouble *>(queue.Peek());
    CPPUNIT_ASSERT(retrieved);
    CPPUNIT_ASSERT_EQUAL(3.0, retrieved->Data);
    CPPUNIT_ASSERT(queue.Pop());
    CPPUNIT_ASSERT(queue.IsEmpty());

    CPPUNIT_ASSERT(queue.Put(first));
    queue.SetSize(0, prototype);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), queue.GetSize());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), queue.GetAvailable());
    CPPUNIT_ASSERT(queue.IsEmpty());
    CPPUNIT_ASSERT(queue.IsFull());
    CPPUNIT_ASSERT(!queue.Put(first));
    CPPUNIT_ASSERT(!queue.Pop());
    queue.SetSize(1, prototype);
    CPPUNIT_ASSERT(queue.Put(second));
    CPPUNIT_ASSERT(queue.IsFull());
    retrieved = dynamic_cast<mtsDouble *>(queue.Peek());
    CPPUNIT_ASSERT(retrieved);
    CPPUNIT_ASSERT_EQUAL(2.0, retrieved->Data);
    CPPUNIT_ASSERT(queue.Pop());
    CPPUNIT_ASSERT(queue.IsEmpty());
}

void mtsQueueTest::TestZeroCapacityAndDefaultConstruction(void)
{
    mtsQueue<int> typedDefault;
    mtsQueue<int> typedEmpty(0, 0);
    mtsQueueGeneric genericDefault;
    mtsDouble prototype = 0.0;
    mtsQueueGeneric genericEmpty(0, prototype);

    int element = -1;
    CPPUNIT_ASSERT(typedDefault.IsEmpty() && typedDefault.IsFull());
    CPPUNIT_ASSERT(typedEmpty.IsEmpty() && typedEmpty.IsFull());
    CPPUNIT_ASSERT(!typedDefault.Put(1) && !typedEmpty.Put(1));
    CPPUNIT_ASSERT(!typedDefault.Peek() && !typedEmpty.Peek());
    CPPUNIT_ASSERT(!typedDefault.Pop() && !typedEmpty.Pop());
    CPPUNIT_ASSERT(!typedDefault.Get(element) && !typedEmpty.Get(element));

    CPPUNIT_ASSERT(genericDefault.IsEmpty() && genericDefault.IsFull());
    CPPUNIT_ASSERT(genericEmpty.IsEmpty() && genericEmpty.IsFull());
    CPPUNIT_ASSERT(!genericDefault.Put(prototype) && !genericEmpty.Put(prototype));
    CPPUNIT_ASSERT(!genericDefault.Peek() && !genericEmpty.Peek());
    CPPUNIT_ASSERT(!genericDefault.Pop() && !genericEmpty.Pop());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), genericDefault.GetAvailable());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), genericEmpty.GetAvailable());
}

void mtsQueueTest::TestPeekedElementHeldUntilPop(void)
{
    mtsQueue<int> typedQueue(2, 0);
    mtsInt prototype = 0;
    mtsQueueGeneric genericQueue(2, prototype);
    mtsInt one = 1, two = 2, three = 3;

    CPPUNIT_ASSERT(typedQueue.Put(1) && typedQueue.Put(2));
    CPPUNIT_ASSERT(genericQueue.Put(one) && genericQueue.Put(two));
    int * typedOldest = typedQueue.Peek();
    mtsInt * genericOldest = dynamic_cast<mtsInt *>(genericQueue.Peek());
    CPPUNIT_ASSERT(typedOldest && genericOldest);

    // a full queue must not hand the peeked slot back to the producer
    CPPUNIT_ASSERT(!typedQueue.Put(3));
    CPPUNIT_ASSERT(!genericQueue.Put(three));
    CPPUNIT_ASSERT_EQUAL(1, *typedOldest);
    CPPUNIT_ASSERT_EQUAL(1, genericOldest->Data);

    CPPUNIT_ASSERT(typedQueue.Pop());
    CPPUNIT_ASSERT(genericQueue.Pop());
    CPPUNIT_ASSERT(typedQueue.Put(3));
    CPPUNIT_ASSERT(genericQueue.Put(three));
}

void mtsQueueTest::TestTypedExceptionSafety(void)
{
    ThrowingValue::Budget = 1000;
    {
        ThrowingValue prototype(7);
        mtsQueue<ThrowingValue> queue(3, prototype);
        CPPUNIT_ASSERT(queue.Put(ThrowingValue(1)));
        CPPUNIT_ASSERT(queue.Put(ThrowingValue(2)));

        // a failed resize leaves the queue as it was
        ThrowingValue::Budget = 2;
        CPPUNIT_ASSERT_THROW(queue.SetSize(10, prototype), std::runtime_error);
        ThrowingValue::Budget = 1000;
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), queue.GetSize());
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), queue.GetAvailable());
        CPPUNIT_ASSERT(queue.Peek());
        CPPUNIT_ASSERT_EQUAL(1, queue.Peek()->Value);

        // a failed Put publishes nothing
        ThrowingValue::Budget = 0;
        CPPUNIT_ASSERT_THROW(queue.Put(ThrowingValue(3)), std::runtime_error);
        ThrowingValue::Budget = 1000;
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), queue.GetAvailable());
        CPPUNIT_ASSERT(queue.Put(ThrowingValue(3)));
        CPPUNIT_ASSERT(queue.IsFull());

        // a failed Get leaves the element queued
        ThrowingValue out;
        ThrowingValue::Budget = 0;
        CPPUNIT_ASSERT_THROW(queue.Get(out), std::runtime_error);
        ThrowingValue::Budget = 1000;
        CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), queue.GetAvailable());
        CPPUNIT_ASSERT(queue.Get(out));
        CPPUNIT_ASSERT_EQUAL(1, out.Value);
    }
    CPPUNIT_ASSERT_EQUAL(0, ThrowingValue::Alive);
}

void mtsQueueTest::TestGenericExceptionSafety(void)
{
    mtsMacrosTestClass original;
    mtsQueueGeneric queue(3, original);
    CPPUNIT_ASSERT(queue.Put(original));
    CPPUNIT_ASSERT(queue.Put(original));

    // a failed resize leaves the queue as it was and releases the partial allocation
    mtsMacrosTestClass::ResetAllCounters();
    mtsMacrosTestClass::CopyConstructorLimit = 2;
    bool threw = false;
    try {
        queue.SetSize(10, original);
    } catch (const std::runtime_error &) {
        threw = true;
    }
    mtsMacrosTestClass::CopyConstructorLimit = std::numeric_limits<size_t>::max();
    CPPUNIT_ASSERT(threw);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), mtsMacrosTestClass::CopyConstructorCalls);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), mtsMacrosTestClass::DestructorCalls);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(3), queue.GetSize());
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), queue.GetAvailable());
    CPPUNIT_ASSERT(queue.Peek());
    CPPUNIT_ASSERT(queue.Pop());
    CPPUNIT_ASSERT(queue.Pop());
    CPPUNIT_ASSERT(queue.IsEmpty());
    CPPUNIT_ASSERT(queue.Put(original));

    // a failed first allocation leaves a usable empty queue
    mtsQueueGeneric fresh;
    mtsMacrosTestClass::ResetAllCounters();
    mtsMacrosTestClass::CopyConstructorLimit = 1;
    threw = false;
    try {
        fresh.SetSize(4, original);
    } catch (const std::runtime_error &) {
        threw = true;
    }
    mtsMacrosTestClass::CopyConstructorLimit = std::numeric_limits<size_t>::max();
    CPPUNIT_ASSERT(threw);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), fresh.GetSize());
    CPPUNIT_ASSERT(fresh.IsEmpty());
    CPPUNIT_ASSERT(!fresh.Peek());
    CPPUNIT_ASSERT(!fresh.Put(original));
}

void mtsQueueTest::TestAgainstReferenceModel(void)
{
    const size_t capacities[] = {0, 1, 2, 7, 64};
    for (size_t capacity : capacities) {
        mtsQueue<int> typedQueue(capacity, 0);
        mtsInt prototype = 0;
        mtsQueueGeneric genericQueue(capacity, prototype);
        std::deque<int> expected;
        std::mt19937 generator(static_cast<unsigned int>(capacity));

        for (int sequence = 0; sequence < 5000; ++sequence) {
            const unsigned int operation = generator() % 3;
            if (operation == 0) {
                mtsInt element = sequence;
                const bool shouldSucceed = expected.size() < capacity;
                CPPUNIT_ASSERT_EQUAL(shouldSucceed, typedQueue.Put(sequence) != 0);
                CPPUNIT_ASSERT_EQUAL(shouldSucceed, genericQueue.Put(element) != 0);
                if (shouldSucceed) {
                    expected.push_back(sequence);
                }
            } else {
                int * typed = typedQueue.Peek();
                mtsInt * generic = dynamic_cast<mtsInt *>(genericQueue.Peek());
                CPPUNIT_ASSERT_EQUAL(!expected.empty(), typed != 0);
                CPPUNIT_ASSERT_EQUAL(!expected.empty(), generic != 0);
                if (!expected.empty()) {
                    CPPUNIT_ASSERT_EQUAL(expected.front(), *typed);
                    CPPUNIT_ASSERT_EQUAL(expected.front(), generic->Data);
                }
                if (operation == 2) {
                    CPPUNIT_ASSERT_EQUAL(!expected.empty(), typedQueue.Pop());
                    CPPUNIT_ASSERT_EQUAL(!expected.empty(), genericQueue.Pop());
                    if (!expected.empty()) {
                        expected.pop_front();
                    }
                }
            }

            CPPUNIT_ASSERT_EQUAL(expected.size(), typedQueue.GetAvailable());
            CPPUNIT_ASSERT_EQUAL(expected.size(), genericQueue.GetAvailable());
            CPPUNIT_ASSERT_EQUAL(expected.empty(), typedQueue.IsEmpty());
            CPPUNIT_ASSERT_EQUAL(expected.empty(), genericQueue.IsEmpty());
            CPPUNIT_ASSERT_EQUAL(expected.size() == capacity, typedQueue.IsFull());
            CPPUNIT_ASSERT_EQUAL(expected.size() == capacity, genericQueue.IsFull());
        }
    }
}

void mtsQueueTest::TestQueue_mtsDouble(void)
{
    // test default constructor
    mtsQueueGeneric queue;
    CPPUNIT_ASSERT_EQUAL(queue.GetSize(), static_cast<size_t>(0));
    CPPUNIT_ASSERT_EQUAL(queue.GetAvailable(), static_cast<size_t>(0));
    CPPUNIT_ASSERT(queue.IsEmpty());
    CPPUNIT_ASSERT(queue.IsFull());

    // test resize
    const mtsDouble original = 1.2345;
    const size_t size = 100;
    queue.SetSize(size, original);
    CPPUNIT_ASSERT_EQUAL(queue.GetSize(), size);
    CPPUNIT_ASSERT_EQUAL(queue.GetAvailable(), static_cast<size_t>(0));
    CPPUNIT_ASSERT(queue.IsEmpty());
    CPPUNIT_ASSERT(!queue.IsFull());

    mtsDouble element;
    mtsGenericObject * basePointer;
    mtsDouble * retrieved;

    // test with one element, fill the queue at least twice to test circular buffer
    size_t index;
    for (index = 0; index < 3 * queue.GetSize(); index++) {
        element = static_cast<double>(index);
        CPPUNIT_ASSERT(queue.Put(element));
        CPPUNIT_ASSERT_EQUAL(queue.GetAvailable(), static_cast<size_t>(1));
        basePointer = queue.Peek();
        CPPUNIT_ASSERT(basePointer);
        CPPUNIT_ASSERT_EQUAL(queue.GetAvailable(), static_cast<size_t>(1));
        retrieved = dynamic_cast<mtsDouble *>(basePointer);
        CPPUNIT_ASSERT(retrieved);
        CPPUNIT_ASSERT_EQUAL(retrieved->Data, static_cast<double>(index));
        CPPUNIT_ASSERT(queue.Pop());
        CPPUNIT_ASSERT_EQUAL(queue.GetAvailable(), static_cast<size_t>(0));
    }

    CPPUNIT_ASSERT(queue.IsEmpty());
    CPPUNIT_ASSERT_EQUAL(queue.GetAvailable(), static_cast<size_t>(0));

    // test filling it up
    for (index = 0; index < queue.GetSize(); index++) {
        element = static_cast<double>(index);
        CPPUNIT_ASSERT(queue.Put(element));
        CPPUNIT_ASSERT_EQUAL(queue.GetAvailable(), index + 1);
        CPPUNIT_ASSERT(!queue.IsEmpty());
    }
    CPPUNIT_ASSERT(queue.IsFull());

    // test peek and empty
    for (index = 0; index < queue.GetSize(); index++) {
        basePointer = queue.Peek();
        CPPUNIT_ASSERT(basePointer);
        CPPUNIT_ASSERT_EQUAL(queue.GetAvailable(), queue.GetSize() - index);
        retrieved = dynamic_cast<mtsDouble *>(basePointer);
        CPPUNIT_ASSERT(retrieved);
        CPPUNIT_ASSERT_EQUAL(retrieved->Data, static_cast<double>(index));
        CPPUNIT_ASSERT(queue.Pop());
        CPPUNIT_ASSERT_EQUAL(queue.GetAvailable(), queue.GetSize() - index - 1);
    }
    CPPUNIT_ASSERT(queue.IsEmpty());
}


void mtsQueueTest::TestConstructorDestructorCalls(void)
{
    // reset all counters
    mtsMacrosTestClass::ResetAllCounters();
    mtsMacrosTestClass original;
    CPPUNIT_ASSERT_EQUAL(mtsMacrosTestClass::DefaultConstructorCalls, static_cast<size_t>(1));
    CPPUNIT_ASSERT_EQUAL(mtsMacrosTestClass::CopyConstructorCalls, static_cast<size_t>(0));
    CPPUNIT_ASSERT_EQUAL(mtsMacrosTestClass::DestructorCalls, static_cast<size_t>(0));

    // test default constructor
    mtsQueueGeneric * queue = new mtsQueueGeneric;

    // test constructor call when set size
    const size_t size = 100;
    size_t index;
    mtsMacrosTestClass::ResetAllCounters();
    queue->SetSize(size, original);
    CPPUNIT_ASSERT_EQUAL(mtsMacrosTestClass::DefaultConstructorCalls, static_cast<size_t>(0));
    CPPUNIT_ASSERT_EQUAL(mtsMacrosTestClass::CopyConstructorCalls, size + 1);
    CPPUNIT_ASSERT_EQUAL(mtsMacrosTestClass::DestructorCalls, static_cast<size_t>(0));

    // test a couple of Put and Pop
    mtsMacrosTestClass::ResetAllCounters();
    for (index = 0; index < size; index++) {
        CPPUNIT_ASSERT(queue->Put(original));
    }
    for (index = 0; index < size; index++) {
        CPPUNIT_ASSERT(queue->Peek());
        CPPUNIT_ASSERT(queue->Pop());
    }
    CPPUNIT_ASSERT_EQUAL(mtsMacrosTestClass::DefaultConstructorCalls, static_cast<size_t>(0));
    CPPUNIT_ASSERT_EQUAL(mtsMacrosTestClass::CopyConstructorCalls, size);
    CPPUNIT_ASSERT_EQUAL(mtsMacrosTestClass::DestructorCalls, static_cast<size_t>(100));

    // test resize
    mtsMacrosTestClass::ResetAllCounters();
    queue->SetSize(size * 2, original);
    CPPUNIT_ASSERT_EQUAL(mtsMacrosTestClass::DefaultConstructorCalls, static_cast<size_t>(0));
    CPPUNIT_ASSERT_EQUAL(mtsMacrosTestClass::CopyConstructorCalls, 2 * size + 1);
    CPPUNIT_ASSERT_EQUAL(mtsMacrosTestClass::DestructorCalls, size + 1);


    // test final delete
    mtsMacrosTestClass::ResetAllCounters();
    delete queue;
    CPPUNIT_ASSERT_EQUAL(mtsMacrosTestClass::DefaultConstructorCalls, static_cast<size_t>(0));
    CPPUNIT_ASSERT_EQUAL(mtsMacrosTestClass::CopyConstructorCalls, static_cast<size_t>(0));
    CPPUNIT_ASSERT_EQUAL(mtsMacrosTestClass::DestructorCalls, 2 * size + 1);
}


void mtsQueueTest::TestTypedLifetimes()
{
    LifetimeValue::constructions = 0;
    LifetimeValue::destructions = 0;
    {

        LifetimeValue prototype;
        mtsQueue<LifetimeValue> queue(4, prototype);
        queue.SetSize(2, prototype);
    }

    CPPUNIT_ASSERT_EQUAL(LifetimeValue::constructions, LifetimeValue::destructions);
}


void mtsQueueTest::TestGenericDefaultConstruction()
{
    mtsQueueGeneric queue;
    CPPUNIT_ASSERT_EQUAL((size_t)0, queue.GetSize());
    CPPUNIT_ASSERT(queue.IsEmpty());
}


namespace {

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


void mtsQueueTest::TestTypedConcurrentUsage()
{
    CPPUNIT_ASSERT(TestConcurrent<TypedQueue>());
}


void mtsQueueTest::TestGenericConcurrentUsage()
{
    CPPUNIT_ASSERT(TestConcurrent<GenericQueue>());
}


