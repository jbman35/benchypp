#pragma once

#include <chrono>
#include <functional>
#include <iostream>
#include <string>
#include <iomanip>
#include <future>

#if defined(__unix__) || defined(__APPLE__)
#include <sys/resource.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

namespace benchy {

using Clock = std::chrono::steady_clock;
using Duration = std::chrono::duration<double, std::milli>;

struct Result {
    std::string name;
    double avg_duration_ms = 0.0;
    double total_duration_ms = 0.0;
    double cpu_load_percent = -1.0; // -1 if unavailable
};

namespace internal {

#if defined(__unix__) || defined(__APPLE__)
inline double get_cpu_time_ms() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    double user = usage.ru_utime.tv_sec * 1000.0 + usage.ru_utime.tv_usec / 1000.0;
    double sys  = usage.ru_stime.tv_sec * 1000.0 + usage.ru_stime.tv_usec / 1000.0;
    return user + sys;
}
#elif defined(_WIN32)
inline double get_cpu_time_ms() {
    FILETIME create, exit, kernel, user;
    if (!GetProcessTimes(GetCurrentProcess(), &create, &exit, &kernel, &user)) {
        return -1.0;
    }

    auto filetime_to_ms = [](const FILETIME& ft) -> double {
        ULARGE_INTEGER li;
        li.LowPart = ft.dwLowDateTime;
        li.HighPart = ft.dwHighDateTime;
        return static_cast<double>(li.QuadPart) * 0.0001; // 100ns to ms
    };

    double user_ms = filetime_to_ms(user);
    double kernel_ms = filetime_to_ms(kernel);
    return user_ms + kernel_ms;
}
#endif

inline Result run(bool async, const std::string& name, const std::function<void()>& func, int iterations = 1) {
    double cpu_before = internal::get_cpu_time_ms();
    auto wall_start = Clock::now();
    if (async) {
        std::future<void> f = std::async(std::launch::async, func);
        f.wait();
    }
    {
        for (int i = 0; i < iterations; ++i) {
            func();
        }
    }

    auto wall_end = Clock::now();
    double cpu_after = internal::get_cpu_time_ms();

    double cpu_time_ms = (cpu_before >= 0 && cpu_after >= 0) ? (cpu_after - cpu_before) : -1.0;

    Duration wall_duration = wall_end - wall_start;
    double total_ms = wall_duration.count();
    double avg_ms = total_ms / iterations;

    double cpu_load_pct = (cpu_time_ms >= 0.0 && total_ms > 0.0)
        ? (cpu_time_ms / total_ms) * 100.0
        : -1.0;

    Result result {
        .name = name,
        .avg_duration_ms = avg_ms,
        .total_duration_ms = total_ms,
        .cpu_load_percent = cpu_load_pct
    };

    std::cout << std::fixed << std::setprecision(3);
    std::cout << "[benchy] " << name
              << ": " << avg_ms << " ms (avg over " << iterations << " run"
              << (iterations > 1 ? "s" : "") << ")";

    if (cpu_load_pct >= 0.0)
        std::cout << " | CPU load: " << std::setprecision(1) << cpu_load_pct << "%";
    else
        std::cout << " | CPU load: n/a";

    std::cout << '\n';
    return result;
}

} // namespace internal

inline Result run_async(const std::string& name, const std::function<void()>& func, int iterations = 1) {
    return internal::run(true, name, func, iterations);
}

inline Result run(const std::string& name, const std::function<void()>& func, int iterations = 1) { 
    return internal::run(false, name, func, iterations);
}

} // namespace benchy
