#pragma once
#include <functional>
#include <mutex>
#include <thread>
#include <vector>
#include <queue>

namespace Rutile {
    class ThreadPool {
    public:
        ThreadPool(size_t threadCount);
        ~ThreadPool();

        ThreadPool(const ThreadPool& other) = delete;
        ThreadPool(ThreadPool&& other) noexcept = default;
        ThreadPool& operator=(const ThreadPool& other) = delete;
        ThreadPool& operator=(ThreadPool&& other) noexcept = default;

        void ThreadLoop();

        void QueueJob(const std::function<void()>& job);

    private:
        std::vector<std::thread> m_Threads;
        std::queue<std::function<void()>> m_Jobs;

        std::mutex m_QueueMutex;
        std::condition_variable m_MutexCondition;

        bool m_ShouldTerminate{ false };
    };
}