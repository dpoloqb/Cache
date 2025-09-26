# Cache Algorithms Implementation

This project includes C++ implementations of two cache replacement algorithms: 2Q Cache and Ideal Cache. The 2Q Cache algorithm is an efficient approximation of LRU-2, designed to account for both recency and frequency of page accesses. The Ideal Cache represents an optimal (though impractical for real-world use) algorithm that makes eviction decisions based on perfect knowledge of future requests.

# Features

- **2Q Cache:**
  - Uses three internal queues:
    - `A1_in` — holds recently accessed pages.
    - `Am` — stores frequently accessed pages (promoted from `A1_in` on a cache hit).
    - `A1_out` — a “ghost” queue that tracks pages evicted from `A1_in`.
  - Supports generic key and value types (defaults to `int` for keys).

- **Ideal Cache:**
  - Evicts the page that will be used farthest in the future (or not used at all), following Belady’s optimal algorithm.
  - Requires full knowledge of the entire future request sequence.
  - Works with generic key and value types (default key type is `int`).

- **Testing:**
  - Includes end-to-end tests for both cache implementations, verifying expected hit quantaties.

# Algorithm Details

## 2Q Cache

- **`A1_in`**: A FIFO queue for newly inserted pages.
- **`Am`**: An LRU-style queue for pages accessed more than once; pages move here from `A1_in` upon a second access.
- **`A1_out`**: A ghost queue that retains identifiers of pages recently evicted from `A1_in`. If a page in `A1_out` is accessed again, it is promoted to `Am`.
- **Eviction Policy**:  
  When the cache is full:
  - If `A1_in` exceeds its size limit, its oldest page is moved to `A1_out`.
  - If total capacity is still exceeded, pages are evicted from `Am` using an LRU strategy.

## Ideal Cache

- Assumes perfect foresight — the complete sequence of future accesses is known in advance.
- On eviction, removes the page whose next use occurs farthest in the future (or never occurs).
- Not implementable in real systems but serves as an optimal theoretical benchmark for evaluating practical caching algorithms.

## Requirements
- C++20 or later.
- Compiler: GCC or Clang.
- No external dependencies.

## Building and Running

1. Clone the repository:
```bash
git clone https://github.com/dpoloqb/Cache
cd Cache
```

2. Configure and Build:
```bash
cmake -DCMAKE_BUILD_TYPE=Realease -S . -B build
cmake --build build
```
3. Run Main Application:
```bash
./build/2q_cache           # to use the 2Q Cache

./build/ideal_cache        # to use the Ideal Cache
```
4. Run Tests 
```bash
cd build

ctest             # to test 2Q and ideal caches

ctest -R "2q_"    # to test 2Q cache directly

ctest -R "ideal_"    # to test 2Q cache directly
```

## Usage
Include ```caches.hpp``` in your project and use the cache classes as follows:

### 2Q Cache Example
```cpp
#include "caches.hpp"

int SlowGetPage(int key) {
    // simulate slow page fetch
    return key;
}

int main() {
    caches::TwoQCache<int> cache{10};               // capacity 10
    bool hit = cache.LookupUpdate(1, SlowGetPage);  // returns false (miss), inserts
    hit = cache.LookupUpdate(1, SlowGetPage);       // returns true (hit)
    return 0;
}
```

### Ideal Cache Example
```cpp
#include <vector>

#include "caches.hpp"

int SlowGetPage(int key) {
    // simulate slow page fetch
    return key;
}

int main() {
    std::vector<int> requests = {1, 2, 3, 1, 4};  // future request sequence
    caches::IdealCache<int> cache{3, requests};   // capacity 3, provide full requests
    bool hit = cache.LookupUpdate(1, SlowGetPage);
    return 0;
}
```

## Project Structure
```
Cache/
├── include/
│ └── caches.hpp
├── src/
| ├── main_2q_cache.cc
│ └── main_ideal_cache.cc
├── tests/
| ├── test1.dat
| ├── test1.2q.ans
| ├── test1.ideal.ans
│ └── ...
├── .gitignore
├── CMakeLists.txt
├── runtest.sh
└── README.md
```