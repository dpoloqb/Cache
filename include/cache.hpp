#pragma once

#include <list>
#include <unordered_map>

namespace caches
{

    template <typename T, typename KeyT = int> struct cache_t
    {
        size_t Am_size_ = Am_size_ / 2;
        size_t A1in_size_ = Am_size_;
        size_t A1out_size_ = 2 * Am_size_; //NOTE - A1out содержит лишь идентификаторы, можно выделить под них побольше места
        
        std::list<std::pair<KeyT, T>> cache_Am;
        std::list<std::pair<KeyT, T>> cache_A1in;
        std::list<KeyT> cache_A1out;

        using ListIt = typename std::list<std::pair<KeyT, T>>::iterator;
        using A1outIt = typename std::list<KeyT>::iterator;

        std::unordered_map<KeyT, ListIt> hash_Am;
        std::unordered_map<KeyT, ListIt> hash_A1in;
        std::unordered_map<KeyT, A1outIt> hash_A1out;

        cache_t (size_t Am_size) : Am_size_(Am_size) {}

        bool full_Am() const { return (cache_Am.size() == Am_size_); } //TODO - обобщение
        bool full_A1in() const { return (cache_A1in.size() == A1in_size_); }
        bool full_A1out() const { return (cache_A1out.size() == A1out_size_); } 
        

        template <typename F> bool lookup_update(KeyT key, F slow_get_page)
        {
            auto hit_Am = hash_Am.find(key);
            
            if (hit_Am != hash_Am.end())
            {

            auto eltit_Am = hit_Am->second;
            cache_Am.splice(cache_Am.begin(), cache_Am, eltit_Am);

            return true;
            }

            auto hit_A1in = hash_A1in.find(key);

            if (hit_A1in != hash_A1in.end())
            {

                if (full_Am()) //TODO - обобщить удаление из списка и хэш таблицы
                {
                    hash_Am.erase(cache_Am.back().first);
                    cache_Am.pop_back();
                }
                cache_A1in.splice(cache_Am.begin(), cache_A1in, hit_A1in->second);

                return true;
            }


            if (full_A1in())
            {
                if (full_A1out())
                {
                    hash_A1out.erase(cache_A1out.back());
                    cache_A1out.pop_back();
                }
                cache_A1out.emplace_front(cache_A1in.back().first);
                cache_A1in.pop_back();
            }
            cache_A1in.emplace_front(key, slow_get_page(key));
            hash_A1in.emplace(key, cache_A1in.begin());

            return false;

        }

    };

}