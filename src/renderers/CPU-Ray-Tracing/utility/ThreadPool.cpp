#include "ThreadPool.h"

namespace Rutile {
    ThreadPool::ThreadPool(size_t threadCount) {
        for (size_t i = 0; i < threadCount; ++i) {
            m_Threads.emplace_back(&ThreadPool::ThreadLoop, this);
        }
    }

    ThreadPool::~ThreadPool() {
        {
            std::unique_lock lock(m_QueueMutex);
            m_ShouldTerminate = true;
            m_MutexCondition.notify_all();
        }
        for (auto& thread : m_Threads) {
            thread.join();
        }
    }

    void ThreadPool::ThreadLoop() {
        while (true) {
            std::function<void()> job;

            {
                std::unique_lock lock{ m_QueueMutex }; // Take hold of the queue
                // The lambda below             VVV         is the condition, when the lambda returns true than this thread wakes up
                m_MutexCondition.wait(lock, [this] { return !m_Jobs.empty() || m_ShouldTerminate; }); // Wait until m_Jobs is not empty (there is a job for us)

                if (m_ShouldTerminate) {
                    return;
                }

                if (m_Jobs.empty()) {
                    continue; // Spurious Wakeup
                }

                job = m_Jobs.front();
                m_Jobs.pop();
            }

            job();
        }
    }

    void ThreadPool::QueueJob(const std::function<void()>& job) {
        {
            std::unique_lock lock{ m_QueueMutex };
            m_Jobs.push(job);
        }
        m_MutexCondition.notify_one();
    }
}