#include<cstddef>
#pragma once
template <typename T, size_t N>
class SPSCQueue
{   
    public:
    SPSCQueue():head_(0), tail_(0)
    {
        static_assert((N & (N - 1)) == 0, "N must be a power of two");
        ptr = new T[N];
        
    }
    inline bool empty()
    {
        return (head_ == tail_ );
    }
    inline bool full()
    {
        return  ((tail_ + 1) % N == head_ );
    }

    bool push(T&obj)
    {
        if(this->full())
        {
            return false;
        }

        ptr[tail_] = obj;
        tail_ = (tail_ + 1) %N;
        return true;

    }

    bool pop(T&target)
    {
        if(this->empty())
        {
            return false;
        }

        target = ptr[head_];
        head_ = (head_ + 1) % N;
        
        return true;

    }

    ~SPSCQueue()
    {
        delete[] ptr;
    }

    private:
        T*ptr;
        size_t head_;
        size_t tail_;

};