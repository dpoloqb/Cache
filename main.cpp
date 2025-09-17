#include <iostream>
#include <cassert>
#include <vector>
#include "include/colors.hpp"
#include "include/cache.hpp"

int slow_get_page_int(int key) { return key; }

int main ()
{
    int hits = 0;
    int nItems;
    size_t cacheSize;

    std::cin >> cacheSize >> nItems;
    assert(std::cin.good());
    caches::cache_t<int> c{cacheSize};

    for (int i = 0; i < nItems; ++i)
    {
        int q;
        std::cin >> q;
        assert(std::cin.good());
        if(c.lookup_update(q, slow_get_page_int))
            hits += 1;
    }
    std::cout << hits << std::endl;
    #if 0
    std::cout << GREEN << "Please enter the cache size:" << RESET << std::endl;

    size_t cacheSize = 0;
    std::cin >> cacheSize;

    std::cout << GREEN << "Please enter the number of items:" << RESET << std::endl;

    size_t nItems = 0;
    std::cin >> nItems;

    std::vector<int> vec = {};

    std::cout << GREEN << "Please enter " << nItems << " items:" << RESET << std::endl;
    int value = 0;
    for (size_t i = 0; i < nItems; i++)
    {
        std::cin >> value;
        vec.push_back (value);
    }
    #endif
}