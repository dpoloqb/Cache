#pragma once

#include <list>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <cassert>
#include <ostream>

namespace caches
{

    template <typename T, typename KeyT = int>
    struct TwoQueuesCache
    {
    
    private:

        size_t cacheSize_;
        size_t A1in_size_;
        size_t Am_size_;
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

        TwoQueuesCache(size_t cacheSize) : cacheSize_(cacheSize), A1in_size_(cacheSize / 2), Am_size_(cacheSize - A1in_size_),
                                     A1out_size_(cacheSize * 2) {}

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

    template <typename T, typename KeyT = int>
    struct IdealCache
    {
    private:

        size_t cacheSize_;
        size_t currentIndex = 0;

        std::list<std::pair<KeyT, T>> cache;

        using ListIt = typename std::list<std::pair<KeyT, T>>::iterator;

        std::unordered_map<KeyT, ListIt> hash;

        std::vector<KeyT> requests_; //последовательность запросов const?
        std::unordered_map<KeyT, std::vector<size_t>> key_positions; //позиции ключей в requests

        void find_victim()
        {  
            ListIt victim = cache.end();
            size_t farthest_use = 0;

            for (auto it = cache.begin(); it != cache.end(); ++it) //проходимся по всему кэшу
            { //мб ещё искать в hash повторные элементы 
                KeyT cached_key = it->first;

                auto it_position = key_positions.find(cached_key);
                if (it_position == key_positions.end())
                    throw std::logic_error("Inconsistent state: cached key not found in key_positions");

                const auto& currentItPositions = it_position->second; //опять вектор с позициями кэшированного ключа в requests

                auto nextItUse = std::upper_bound(currentItPositions.begin(), currentItPositions.end(), currentIndex);

                size_t next_use = (nextItUse == currentItPositions.end())
                    ? std::numeric_limits<size_t>::max()
                    : *nextItUse;
                
                if (next_use > farthest_use)
                {
                    farthest_use = next_use;
                    victim = it;
                }

                if (victim == cache.end())
                    throw std::invalid_argument("Failed to select victim for eviction");          
            }

            hash.erase(victim->first);
            cache.erase(victim);
        }

    public:

    IdealCache(size_t cacheSize, const std::vector<KeyT>& requests) : cacheSize_(cacheSize), requests_(requests)
    {
        for (size_t i = 0; i < requests_.size(); ++i)
        {
            key_positions[requests_[i]].push_back(i);
        }

        #ifndef NDEBUG
        for (const auto& [key, vec] : key_positions)
        {
            assert(std::is_sorted(vec.begin(), vec.end()) && "key_positions must be sorted");
        }
        #endif
    }

    template <typename F>
    bool lookup_update(KeyT key, F slow_get_page)
    {
        if (currentIndex >= requests_.size())
            throw std::out_of_range("No more requests to process");
        

        auto hit_cache = hash.find(key);
        if (hit_cache != hash.end())
        {
            ++currentIndex;
            //cache.splice(cache.begin(), cache, hit->second);
            return true;
        }

        const auto& currentKeyPositions = (key_positions.find(key))->second; //отcортированная копия вектора (не будем менять)

        //std::cout << std::typeid(currentKeyPositions);

        auto nextKeyUse = std::upper_bound(currentKeyPositions.begin(), currentKeyPositions.end(), currentIndex); //ищем первую позицию, больше currentIndex
        //если upper_bound ничего не нашёл, то он возвращает 0. Надо обработать этот случай:

        size_t next_use = (nextKeyUse == currentKeyPositions.end())
                    ? std::numeric_limits<size_t>::max()
                    : *nextKeyUse;

        if (next_use <= currentIndex)
            throw std::logic_error("Next key usage can't be earlier than current request");

        if (next_use != std::numeric_limits<size_t>::max()) //если где-то будет повторное использование, то надо решить, кого вытеснять из текущего кэша
        {
            if (cache.size() == cacheSize_)
            {
               find_victim(); //находит и удаляет элемент
            }
        cache.emplace_front(key, slow_get_page(key)); //добавляем новый элемент
        hash.emplace(key, cache.begin()); //скорей всего можно юзать hash[key] = cache.begin(); 

        }

        //нет повторного использования входящего request'а => не берём
        ++currentIndex;
        return false;

        
    }

    };
}