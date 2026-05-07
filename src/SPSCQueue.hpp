#include<utility>
#include<cstddef>
#pragma once
template<typename T,size_t N>
class SPSCQueue
{
    public:
        static_assert((N & (N-1)) == 0);
        template<typename... Args>
        SPSCQueue(Args...args):head(0), tail(0)
        {
            ptr_raw = new std::byte[N * sizeof(T)];
            ptr = reinterpret_cast<T*>(ptr_raw);
            for(size_t i=0;i<N;i++)
            {
                new(&ptr[i]) T(std::forward<Args>(args)...); 
            }

        }
        ~SPSCQueue()
        {
            delete [] ptr_raw;
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
        std::byte* ptr_raw;
        T* ptr;
        size_t tail, head;
};