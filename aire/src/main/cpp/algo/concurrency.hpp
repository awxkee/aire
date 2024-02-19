/// \file parallel-util.hpp

#pragma once

// Uncommenting the following line enables the "verbose" mode, which may be
// useful for debugging. However, notice that it affects the overall performance
// badly as it inserts mutex-locked standard output commands.

// #define PARALLELUTIL_VERBOSE

#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

#ifdef PARALLELUTIL_VERBOSE
#include <iostream>
#endif

namespace concurrency {
    template <typename Function, typename... Args>
    void parallel_for(int num_threads, int num_iterations, Function&& func, Args&&... args) {
        std::vector<std::thread> threads;
        std::mutex mtx;  // Mutex for synchronization

        auto parallel_worker = [&](int thread_id) {
            for (int i = 0; i < num_iterations; ++i) {
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    func(thread_id, std::forward<Args>(args)...);  // Execute the function
                }
            }
        };

        // Launch N-1 worker threads
        for (int i = 0; i < num_threads - 1; ++i) {
            threads.emplace_back(parallel_worker, i);
        }

        // Main thread executes the function
        parallel_worker(num_threads - 1);

        // Join all threads
        for (auto& thread : threads) {
            thread.join();
        }
    }
}
