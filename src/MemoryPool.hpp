#pragma once
#include<cstddef>
#include<memory>
#include <new>
#include <bitset>
template <typename T,size_t Capacity>
class MemoryPool
{
    public:
        MemoryPool():avail(Capacity),live_cnt(0),free_top(Capacity),storage(new std::byte[Capacity * sizeof(T)]),free_list(new size_t[Capacity])
        {

            for(size_t i= 0; i<Capacity;i++)
            {
                free_list[i] = Capacity - i - 1;
            }
        }
        
        ~MemoryPool()
        {}

        template <typename... Args>
        T*allocate(Args...args)
        {   
            if(free_top == 0)
            {
                return nullptr;
            }

            size_t slot_indx = free_list[--free_top];
            T* addr =reinterpret_cast<T*>(storage.get() + slot_indx * sizeof(T));
            new(addr) T(std::forward<Args>(args)...); 
            avail--;
            live_cnt++;
            occupied.set(slot_indx);
            return addr;
        }

        bool dealocate(T*ptr)
        {
            size_t offset = (ptr - reinterpret_cast<T*>(storage.get()))/sizeof(T);
            if(offset<0 || offset>=Capacity)
            {
                return false;
            }
            std::destroy_at(ptr);
            free_list[free_top++]=offset;
            live_cnt--;
            avail++;
            occupied.reset(offset);
            return true;
        }

        size_t get_avail()
        {
            return avail;
        }

        size_t get_live_cnt()
        {
            return live_cnt;
        }
        std::bitset<Capacity>occupied;
    private:
        size_t free_top;
        std::unique_ptr<std::byte[]>storage;
        std::unique_ptr<size_t[]>free_list;
        size_t avail, live_cnt;

};