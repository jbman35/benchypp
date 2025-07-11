# benchypp
benchy++ is a header-only C++20 micro-library for making benchmarks that are simple, pretty and quick to integrate.

| Platform                 | CPU `%` support | API used      |
| ------------------------ | ---------------- | ----------------- |
| Linux/macOS              | ✅ Oui            | `getrusage`       |
| Windows                  | ✅ Oui            | `GetProcessTimes` |
| Others (e.g.: WebAssembly) | 🚫 Non           | `cpu_load = -1.0` |

Example
```cpp
#include "benchy.hpp"
#include <vector>
#include <numeric>

int main() {
    auto result = benchy::run("accumulate 1M", [] {
        std::vector<int> v(1'000'000, 1);
        volatile int sum = std::accumulate(v.begin(), v.end(), 0);
        (void)sum;
    }, 3);

    // Use benchmark output data
    std::cout << "\n[INFO] Raw measurements:\n"
              << "  Average: " << result.avg_duration_ms << " ms\n"
              << "  Total : " << result.total_duration_ms << " ms\n"
              << "  CPU Load: " 
              << (result.cpu_load_percent >= 0.0 ? std::to_string(result.cpu_load_percent) + "%" : "N/A")
              << '\n';
}
```