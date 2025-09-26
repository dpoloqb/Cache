# Cache Algorithms Implementation

This project provides C++ implementations of two cache replacement algorithms: the 2Q Cache and the Ideal Cache. The 2Q Cache is an efficient approximation of the LRU-2 algorithm, designed to handle both recency and frequency in page accesses. The Ideal Cache is an optimal (but impractical for real-time use) algorithm that assumes knowledge of future requests to make eviction decisions.

## Features

- #### 2Q Cache:
    - Uses three queues: A1_in (recently accessed), Am (frequently accessed), and A1_out (ghost queue for evicted from A1_in).
    - Supports generic key and value types (default key is int).
- #### Ideal Cache:
    - Evicts the page that will be requested furthest in the future (Belady's optimal algorithm).
    - Requires the full sequence of requests in advance.
    - Supports generic key and value types (default key is int).
- #### Testing:
    - Tests for both caches with expected hit/miss sequences.

## Algorithm Details

### 2Q Cache
- **A1_in**: FIFO queue for new pages.
- **Am**: LRU queue for frequently accessed pages (promoted from A1_in on hit).
- **A1_out**: Ghost queue to track evicted pages from A1_in; promotes to Am if hit.
- Eviction: From A1_in to A1_out, or from Am if full.

### Ideal Cache
- Assumes perfect knowledge of future requests.
- On eviction, removes the page with the furthest next access (or never accessed).
- Used as a benchmark for cache efficiency.

## Requirements
- C++20 or later.
- Compiler: GCC or Clang.
- No external dependencies.

## Building and Running
1. Clone the repository:
```bash
git clone https://github.com/dpoloqb/Cache
cd Cache

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

