#include "../src/SPSCQueue.hpp"
#include <gtest/gtest.h>
#include <random>
#include <thread>

TEST(SPSCQueueTest, StartsEmpty) {
    SPSCQueue<int, 1024> q;
    EXPECT_TRUE(q.empty());
    EXPECT_FALSE(q.full());
}

TEST(SPSCQueueTest, PushThenPopRoundTrip) {
    SPSCQueue<int, 1024> q;
    int in = 42;

    EXPECT_TRUE(q.push(in));
    EXPECT_FALSE(q.empty());

    int out = 0;
    EXPECT_TRUE(q.pop(out));
    EXPECT_EQ(out, 42);
    EXPECT_TRUE(q.empty());
}

TEST(SPSCQueueTest, PopOnEmptyQueueFails) {
    SPSCQueue<int, 1024> q;
    int out = 0;

    EXPECT_FALSE(q.pop(out));
}


TEST(SPSCQueueTest,ConsumerProducerTest)
{

    SPSCQueue<int,1024> Q;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<>dist(1,100000);
    int n = 1000;
    std::vector<int> vec(n);
    std::vector<int> res(n);

    for(int i=0;i<n;i++)
    {
        vec[i] = dist(gen);
        res[i] = 0;
    }

    std::thread producer([&](){
        for(int i=0;i<n;i++)
        {
            Q.push(vec[i]);
        }
    });

    std::thread consumer([&](){
        int i=0;
        while ( i != n)
        {
            if(Q.pop(res[i]))
            {
                i++;
            }
        }
    });

    producer.join();
    consumer.join();

    EXPECT_EQ(vec, res);
}

// Queue of size N can hold N-1 elements (one slot reserved to distinguish full from empty)
TEST(SPSCQueueTest, FillToCapacity) {
    SPSCQueue<int, 8> q;  // holds 7 elements max
    int val = 1;
    for (int i = 0; i < 7; i++) {
        EXPECT_TRUE(q.push(val));
    }
    EXPECT_TRUE(q.full());
    EXPECT_FALSE(q.push(val));  // 8th push must fail
}

// After filling, popping one slot allows exactly one more push
TEST(SPSCQueueTest, PushAfterPopWhenFull) {
    SPSCQueue<int, 8> q;
    int val = 99;
    for (int i = 0; i < 7; i++) q.push(val);
    EXPECT_FALSE(q.push(val));  // full

    int out = 0;
    q.pop(out);
    EXPECT_TRUE(q.push(val));   // one slot freed
    EXPECT_TRUE(q.full());
}

// Elements must come out in the same order they were pushed (FIFO)
TEST(SPSCQueueTest, FIFOOrdering) {
    SPSCQueue<int, 16> q;
    for (int i = 0; i < 10; i++) q.push(i);

    for (int i = 0; i < 10; i++) {
        int out = -1;
        EXPECT_TRUE(q.pop(out));
        EXPECT_EQ(out, i);
    }
}

// Push N-1 items, pop all, then push N-1 more — verifies the ring wraps correctly
TEST(SPSCQueueTest, WrapAround) {
    SPSCQueue<int, 8> q;  // capacity 7
    int val = 0;

    // Fill and drain once
    for (int i = 0; i < 7; i++) q.push(i);
    for (int i = 0; i < 7; i++) { int out; q.pop(out); }

    // Fill again across the wrap boundary
    for (int i = 0; i < 7; i++) {
        EXPECT_TRUE(q.push(i * 2));
    }
    for (int i = 0; i < 7; i++) {
        int out = -1;
        EXPECT_TRUE(q.pop(out));
        EXPECT_EQ(out, i * 2);
    }
    EXPECT_TRUE(q.empty());
}