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