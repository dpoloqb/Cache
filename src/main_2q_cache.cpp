#include "caches.hpp"
#include <iostream>
#include <vector>

int slow_get_page_int(int key) { return key; }

int main() {
  size_t hits = 0;
  size_t nItems;
  size_t cacheSize;

  std::cin >> cacheSize >> nItems;
  if (!std::cin.good()) {
    std::cerr << "Input error: expected integer at element " << std::endl;
    return 1;
  }

  if (nItems < 1) {
    std::cerr << "Incorrect number of items";
    return 1;
  }

  caches::TwoQueuesCache<int> c{cacheSize};

  for (size_t i = 0; i < nItems; ++i) {
    int q;
    std::cin >> q;
    if (!std::cin.good()) {
      std::cerr << "Input error: expected integer at element " << std::endl;
      return 1;
    }
    if (c.lookup_update(q, slow_get_page_int))
      hits += 1;
  }
  std::cout << hits << std::endl;
}