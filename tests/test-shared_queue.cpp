struct IUnknown; // Workaround for "combaseapi.h(229): error C2187: syntax error: 'identifier' was unexpected here" when using /permissive-

#include "../include/tuc/shared_queue.hpp"
#include "picotest/picotest.h"
#include <thread>
#include <atomic>
#include <map>

namespace {

    class SharedQueueTest : public ::testing::Test {
    protected:
        SharedQueueTest() {
            // You can do set-up work for each test here.
        }

        virtual ~SharedQueueTest() {
            // You can do clean-up work that doesn't throw exceptions here.
        }

        virtual void SetUp() {
            // Code here will be called immediately after the constructor (right before each test).
        }

        virtual void TearDown() {
            // Code here will be called immediately after each test (right before the destructor).
        }

        tuc::shared_queue<std::string> buffer;
    };

    TEST_F(SharedQueueTest, DoesNotPopIfNothingPushed) {
        std::string retrievedValue;
        EXPECT_FALSE(buffer.pop_front(retrievedValue));
        EXPECT_EQ(retrievedValue, "");
    }

    TEST_F(SharedQueueTest, PopsPushedValuesSingleThread) {
        buffer.push_back("test1");
        buffer.push_back("test2");

        std::string retrievedValue;
        EXPECT_TRUE(buffer.pop_front(retrievedValue));
        EXPECT_EQ(retrievedValue, "test1");
        EXPECT_TRUE(buffer.pop_front(retrievedValue));
        EXPECT_EQ(retrievedValue, "test2");
    }

    TEST_F(SharedQueueTest, PopsPushedValueImmediately) {

        std::thread consumer{ [&] {
            auto const t0 = std::chrono::steady_clock::now();
            std::string value;
            EXPECT_TRUE(buffer.pop_front(value, std::chrono::seconds(1)));
            EXPECT_EQ(value, "test");
            EXPECT_LE(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0).count(), 10);
        } };

        std::thread producer{ [&] {
            buffer.push_back("test");
        } };

        producer.join();
        consumer.join();
    }

    TEST_F(SharedQueueTest, Halts) {
        auto const t0 = std::chrono::steady_clock::now();

        std::thread consumer{ [&] {
            auto const t1 = std::chrono::steady_clock::now();
            std::string value;
            EXPECT_FALSE(buffer.pop_front(value, std::chrono::seconds(1)));
            EXPECT_LE(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t1).count(), 10);
        } };

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        buffer.halt();
        consumer.join();

        EXPECT_LE(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0).count(), 20);
    }

    TEST_F(SharedQueueTest, ReturnsIfHaltedBeforehand) {
        buffer.halt();

        auto const t0 = std::chrono::steady_clock::now();
        std::string value;
        EXPECT_FALSE(buffer.pop_front(value, std::chrono::seconds(1)));
        EXPECT_LE(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - t0).count(), 10);
    }

    TEST_F(SharedQueueTest, PopsPushedValuesDifferentThreads) {

        int const valuesToPush = 1000;

        // Deliberately relay strings rather than ints, so we easily get some additional debug checks.

        std::thread consumer{ [&] {
            std::string value;
            int expectedNumber = 0;
            while (expectedNumber < valuesToPush && buffer.pop_front(value, std::chrono::seconds(1))) {
                int const number = std::stoi(value);
                EXPECT_EQ(number, expectedNumber);
                expectedNumber = number + 1;
            }
            EXPECT_EQ(expectedNumber, valuesToPush);
        } };

        std::thread producer{ [&] {
            for (int i = 0; i < valuesToPush; ++i) {
                std::ostringstream number;
                number << i;
                buffer.push_back(number.str());
            }
        } };

        producer.join();
        consumer.join();

    }

    TEST_F(SharedQueueTest, HandlesMultipleProducersAndConsumers) {

        size_t const valuesToPush = 100;
        size_t const consumerCount = 20;
        size_t const producerCount = 10;

        std::vector<std::thread> consumers, producers;

        std::mutex mutex;
        std::map<std::string, size_t> consumedValueCounts;

        for (size_t i = 0; i < consumerCount; ++i) {
            consumers.push_back(std::thread([&] {
                std::string value;
                while (buffer.pop_front(value, std::chrono::seconds(1))) {
                    std::lock_guard<std::mutex> lock(mutex);
                    ++consumedValueCounts[value];
                }
            }));
        }

        for (size_t i = 0; i < producerCount; ++i) {
            producers.push_back(std::thread([&] {
                for (size_t i = 0; i < valuesToPush; ++i) {
                    std::ostringstream number;
                    number << i;
                    buffer.push_back(number.str());
                }
            }));
        }

        for (auto& producer : producers) {
            producer.join();
        }

        buffer.halt();

        for (auto& consumer : consumers) {
            consumer.join();
        }

        EXPECT_EQ(consumedValueCounts.size(), valuesToPush);
        for (auto const& i : consumedValueCounts) {
            EXPECT_EQ(i.second, producerCount);
        }
    }

}  // namespace
