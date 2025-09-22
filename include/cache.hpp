#pragma once

#include <list>
#include <unordered_map>
#include <iostream>

namespace caches
{

    template <typename T, typename KeyT = int>
    struct TwoQueuesCache
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
        
        template <typename QTy>
        bool isFull(QTy& Q, size_t refSize) const
        { return (Q.size() == refSize); }

        bool check_Am(KeyT key)
        {
            auto hit_Am = hash_Am.find(key);
            
            if (hit_Am != hash_Am.end())
            {

                auto eltit_Am = hit_Am->second;
                cache_Am.splice(cache_Am.begin(), cache_Am, eltit_Am);

                return true;
            }

            return false;
        }

        bool check_A1in(KeyT key)
        {
            auto hit_A1in = hash_A1in.find(key);

            if (hit_A1in != hash_A1in.end())
            {

                if (isFull(cache_Am, Am_size_))
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

            return false;
        }

        template <typename F>
        bool check_A1out(KeyT key, F slow_get_page)
        {
            auto hit_A1out = hash_A1out.find(key);

            if (hit_A1out != hash_A1out.end()) 
            {
                if (isFull(cache_Am, Am_size_))
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
            return true;
        }

        template <typename F>
        void get_new_elem(KeyT key, F slow_get_page)
        {
            if (isFull(cache_A1in, A1in_size_))
            {
                if (isFull(cache_A1out, A1out_size_))
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
        }

    public:

        TwoQueuesCache(size_t cacheSize) : cacheSize_(cacheSize), Am_size_(cacheSize / 2),
                                    A1in_size_(cacheSize / 2), A1out_size_(cacheSize * 2) {}

        template <typename F>
        bool lookup_update(KeyT key, F slow_get_page)
        {
            if (check_Am(key))
                return true;
            if (check_A1in(key))
                return true;
            if (!check_A1out(key, slow_get_page))
                return false;

            get_new_elem(key, slow_get_page);
            return false;
        }
    };
}