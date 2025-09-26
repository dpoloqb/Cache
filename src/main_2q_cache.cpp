#include <iostream>
#include <cassert>
#include <vector>
#include "caches.hpp"

int slow_get_page_int(int key) { return key; }

int main ()
{
    int hits = 0;
    int nItems;
    size_t cacheSize;

    std::cin >> cacheSize >> nItems;
    assert(std::cin.good());
    
    caches::TwoQueuesCache<int> c{cacheSize};

    for (int i = 0; i < nItems; ++i)
    {
        int q;
        std::cin >> q;
        assert(std::cin.good());
        if(c.lookup_update(q, slow_get_page_int))
            hits += 1;
    }
    std::cout << hits << std::endl;

}