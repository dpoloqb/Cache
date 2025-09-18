#pragma once

#include <list>
#include <unordered_map>
#include <iostream>

namespace caches
{

    template <typename T, typename KeyT = int> struct cache_t
    {
    
    private:

        size_t cacheSize_;
        size_t Am_size_;
        size_t A1in_size_;
        size_t A1out_size_;
        
        std::list<std::pair<KeyT, T>> cache_Am;
        std::list<std::pair<KeyT, T>> cache_A1in;
        std::list<KeyT> cache_A1out;

        using ListIt = typename std::list<std::pair<KeyT, T>>::iterator;
        using A1outIt = typename std::list<KeyT>::iterator;

        std::unordered_map<KeyT, ListIt> hash_Am;
        std::unordered_map<KeyT, ListIt> hash_A1in;
        std::unordered_map<KeyT, A1outIt> hash_A1out;

        bool full_Am() const { return (cache_Am.size() == Am_size_); }
        bool full_A1in() const { return (cache_A1in.size() == A1in_size_); }
        bool full_A1out() const { return (cache_A1out.size() == A1out_size_); } 

    public:

        cache_t(size_t cacheSize)
        {
            cacheSize_ = cacheSize;
            Am_size_ = cacheSize / 2;
            A1in_size_ = cacheSize / 2;
            A1out_size_ = 2 * cacheSize;
        }
        

        template <typename F> bool lookup_update(KeyT key, F slow_get_page)
        {
            //std::cout << cacheSize_;
            auto hit_Am = hash_Am.find(key);
            
            if (hit_Am != hash_Am.end())
            {

                auto eltit_Am = hit_Am->second;
                cache_Am.splice(cache_Am.begin(), cache_Am, eltit_Am);
                //FIXME - может нужно что-то менять в хэш-таблице

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
                cache_Am.emplace_front(*(hit_A1in->second));
                hash_Am.emplace(key, cache_Am.begin()); 
                hash_A1in.erase(hit_A1in->first);
                cache_A1in.erase(hit_A1in->second);

                return true;
            }

            auto hit_A1out = hash_A1out.find(key);

            if (hit_A1out != hash_A1out.end()) 
            {
                if (full_Am()) //TODO - обобщить удаление из списка и хэш таблицы
                {
                    hash_Am.erase(cache_Am.back().first); 
                    cache_Am.pop_back();
                }
                cache_Am.emplace_front(key, slow_get_page(key)); 
                hash_Am.emplace(key, cache_Am.begin());
                hash_A1out.erase(hit_A1out->first);
                cache_A1out.erase(hit_A1out->second);

                return false;
            }

            if (full_A1in())
            {
                if (full_A1out())
                {
                    hash_A1out.erase(cache_A1out.back());
                    cache_A1out.pop_back();
                }
                cache_A1out.emplace_front(cache_A1in.back().first); 
                hash_A1out.emplace(cache_A1in.back().first, cache_A1out.begin()); 
                hash_A1in.erase(cache_A1in.back().first);
                cache_A1in.pop_back();
            }
            cache_A1in.emplace_front(key, slow_get_page(key)); 
            hash_A1in.emplace(key, cache_A1in.begin());

            return false;
        }

    };

}