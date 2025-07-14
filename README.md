# benchypp
benchy++ is a header-only C++20 micro-library for making benchmarks that are simple, pretty and quick to integrate.
It supports both of synchronous and asynchronous runs.


| Platform                 | CPU `%` support | API used      |
| ------------------------ | ---------------- | ----------------- |
| Linux/macOS              | ✅ Yes            | `getrusage`       |
| Windows                  | ✅ Yes            | `GetProcessTimes` |
| Others (e.g.: WebAssembly) | 🚫 No           | `cpu_load = -1.0` |

Example
```cpp
#include "benchy.hpp"
#include <vector>
#include <numeric>

int main() {
    benchy::run("std::vector<int> push_back 1M", [] {
        std::vector<int> v;
        v.reserve(1'000'000);
        for (int i = 0; i < 1'000'000; ++i)
            v.push_back(i);
    });

    benchy::run("accumulate 1M", [] {
        std::vector<int> v(1'000'000, 1);
        volatile int sum = std::accumulate(v.begin(), v.end(), 0);
        (void)sum;
    }, 5);

    benchy::run_async("std::vector<int> push_back 1M", [] {
        std::vector<int> v;
        v.reserve(1'000'000);
        for (int i = 0; i < 1'000'000; ++i)
            v.push_back(i);
    });

    benchy::run_async("accumulate 1M", [] {
        std::vector<int> v(1'000'000, 1);
        volatile int sum = std::accumulate(v.begin(), v.end(), 0);
        (void)sum;
    }, 5);
}
```
