#include "../src/MemoryPool.hpp"
#include <gtest/gtest.h>
#include <random>
#include <thread>

TEST(MemoryPoolTest,StartEmpty)
{

    struct T
    {
        int i_;
        double d_;
        char c_;
        T(int i,double d, char c)
        {
            i_=i;
            d_=d;
            c_=c;
        }
    };

    MemoryPool<T,1024> mp;

    EXPECT_EQ(mp.get_avail(), 1024);
    EXPECT_EQ(mp.get_live_cnt(), 0);

    T*ptr=mp.allocate(10,100.1,'a');
    EXPECT_EQ(mp.get_avail(), 1023);
    EXPECT_EQ(mp.get_live_cnt(), 1);
    EXPECT_EQ(mp.occupied[0],1);

    EXPECT_EQ(ptr->c_,'a');
    EXPECT_EQ(ptr->d_,100.1);
    EXPECT_EQ(ptr->i_,10);
    mp.dealocate(ptr);
    EXPECT_EQ(mp.get_avail(),1024);
    EXPECT_EQ(mp.get_live_cnt(),0);
    EXPECT_EQ(mp.occupied[0],0);
    
}


TEST(MemoryPoolTest, AllocateUntilFullThenFail) {
    struct T {
        int value;
        explicit T(int v) : value(v) {}
    };

    MemoryPool<T, 4> mp;
    T* p0 = mp.allocate(0);
    T* p1 = mp.allocate(1);
    T* p2 = mp.allocate(2);
    T* p3 = mp.allocate(3);
    T* p4 = mp.allocate(4);

    EXPECT_NE(p0, nullptr);
    EXPECT_NE(p1, nullptr);
    EXPECT_NE(p2, nullptr);
    EXPECT_NE(p3, nullptr);
    EXPECT_EQ(p4, nullptr);

    EXPECT_EQ(mp.get_avail(), 0);
    EXPECT_EQ(mp.get_live_cnt(), 4);
}

TEST(MemoryPoolTest, ReuseFreedSlot) {
    struct T {
        int value;
        explicit T(int v) : value(v) {}
    };

    MemoryPool<T, 4> mp;
    T* first = mp.allocate(1);
    T* second = mp.allocate(2);

    ASSERT_NE(first, nullptr);
    ASSERT_NE(second, nullptr);

    EXPECT_TRUE(mp.dealocate(first));
    EXPECT_EQ(mp.get_avail(), 3);
    EXPECT_EQ(mp.get_live_cnt(), 1);

    T* third = mp.allocate(3);
    ASSERT_NE(third, nullptr);
    EXPECT_EQ(third, first);
    EXPECT_EQ(third->value, 3);
}

TEST(MemoryPoolTest, DeallocateSecondAllocation) {
    struct T {
        int value;
        explicit T(int v) : value(v) {}
    };

    MemoryPool<T, 4> mp;
    T* first = mp.allocate(1);
    T* second = mp.allocate(2);

    ASSERT_NE(first, nullptr);
    ASSERT_NE(second, nullptr);

    EXPECT_TRUE(mp.dealocate(second));
    EXPECT_EQ(mp.get_avail(), 3);
    EXPECT_EQ(mp.get_live_cnt(), 1);
}

TEST(MemoryPoolTest, StoresDistinctValuesPerAllocation) {
    struct T {
        int i_;
        double d_;
        char c_;
        T(int i, double d, char c) : i_(i), d_(d), c_(c) {}
    };

    MemoryPool<T, 4> mp;
    T* a = mp.allocate(1, 1.5, 'x');
    T* b = mp.allocate(2, 2.5, 'y');

    ASSERT_NE(a, nullptr);
    ASSERT_NE(b, nullptr);

    EXPECT_EQ(a->i_, 1);
    EXPECT_DOUBLE_EQ(a->d_, 1.5);
    EXPECT_EQ(a->c_, 'x');

    EXPECT_EQ(b->i_, 2);
    EXPECT_DOUBLE_EQ(b->d_, 2.5);
    EXPECT_EQ(b->c_, 'y');
}

TEST(MemoryPoolTest, RejectForeignPointer) {
    struct T {
        int value;
        explicit T(int v) : value(v) {}
    };

    MemoryPool<T, 4> mp;
    T external(99);

    EXPECT_FALSE(mp.dealocate(&external));
}
