# TMATH
## Brief
A C++ library for mathematical operations.

header only library.


## Features

- Header-only library for mathematical operations
- Supports C++23 standard
- Lazy evaluation for efficient computations
- Works with various container types (e.g., std::vector, std::array)
- Supports element-wise operations and mathematical functions

### Lazy Evaluation

All computations in TMATH are lazily evaluated, leveraging C++23 features. This means:

- Operations are not immediately performed when called
- Actual computation occurs only when the result is needed
- Enables efficient chaining of multiple operations
- Reduces unnecessary intermediate allocations and computations



## Build and Test

```bash
cmake -B build 
cmake --build build
ctest --test-dir build
```
## Usage

```cpp
#include "tmath.hpp"

std::vector<double> vec = {1, 2, 3, 4, 5};
auto sin_vec = tmath::sin(vec); 
// sin_vec is now {sin(1), sin(2), sin(3), sin(4), sin(5)}

std::array<double, 5> arr = {1, 2, 3, 4, 5};
auto multiplied_arr = arr * vec;
// multiplied_arr is now {1*1, 2*2, 3*3, 4*4, 5*5}
```
