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

  if (cacheSize < 1) {
    std::cerr << "Not valid cache" << std::endl;
    return 1;
  }

  if (nItems < 1) {
    std::cerr << "Incorrect number of items";
    return 1;
  }

  std::vector<int> requests;

  for (size_t i = 0; i != nItems; ++i) {
    int num = 0;
    std::cin >> num;
    if (!std::cin.good()) {
      std::cerr << "Input error: expected integer at element " << std::endl;
      return 1;
    }
    requests.push_back(num);
  }

  caches::IdealCache<int> c{cacheSize, requests};

  for (int i = 0; i < nItems; ++i) {
    if (c.lookup_update(requests[i], slow_get_page_int))
      hits += 1;
  }
  std::cout << hits << std::endl;
}