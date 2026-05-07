#include<utility>
#include<cstddef>
#pragma once
template<typename T,size_t N>
class SPSCQueue
{
    public:
        static_assert((N & (N-1)) == 0);
        SPSCQueue():head(0), tail(0)
        {
            ptr = new T[N];

        }
        ~SPSCQueue()
        {
            delete [] ptr;
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

            ptr[tail] = obj;
            
            tail = (tail + 1)%N;

            return true;

        }

        bool pop(T&target)
        {
            if(empty())
            {
                return false;
            }

            target = ptr[head];

            head = (head + 1)%N;
            return true;
        }

    private:
        T* ptr;
        size_t tail, head;
};