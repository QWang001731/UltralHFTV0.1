#include<utility>
#include<cstddef>
#include <memory>
#pragma once
template<typename T,size_t N>
class SPSCQueue
{
    public:
        static_assert((N & (N-1)) == 0);
        SPSCQueue():head(0), tail(0),storage(new T[N])
        {
        }
        ~SPSCQueue()
        {
        }

        inline bool full()
        {
            return (tail + 1) % N == head; 
        }

        inline bool empty()
        {
            return head == tail;
        }

        bool push(T&obj)
        {
            if(full())
            {
                return false;
            }

            storage.get()[tail] = obj;
            
            tail = (tail + 1)%N;

            return true;

        }

        bool pop(T&target)
        {
            if(empty())
            {
                return false;
            }

            target = storage.get()[head];

            head = (head + 1)%N;
            return true;
        }

    private:
        std::unique_ptr<T[]> storage;
        size_t tail, head;
};