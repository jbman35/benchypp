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
}
