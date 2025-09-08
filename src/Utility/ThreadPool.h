#pragma once
#include <functional>
#include <mutex>
#include <thread>
#include <vector>
#include <queue>
#include <iostream>

namespace Rutile {
    template<typename ArgumentType>
    class ThreadPool {
    public:
        using Job = std::function<void(ArgumentType)>;

        ThreadPool(size_t threadCount) {
            //std::cout << threadCount << std::endl;
            for (size_t i = 0; i < threadCount; ++i) {
                m_Threads.emplace_back(&ThreadPool::ThreadLoop, this);
            }
        }

        ~ThreadPool() {
            {
                std::unique_lock lock(m_QueueMutex);
                m_ShouldTerminate = true;
                m_MutexCondition.notify_all();
            }
            for (auto& thread : m_Threads) {
                thread.join();
            }
        }

        ThreadPool(const ThreadPool& other) = delete;
        ThreadPool(ThreadPool&& other) noexcept = default;
        ThreadPool& operator=(const ThreadPool& other) = delete;
        ThreadPool& operator=(ThreadPool&& other) noexcept = default;

        void ThreadLoop() {
            while (true) {
                Job job;
                ArgumentType arg;

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
                    arg = m_JobArguments.front();

                    m_Jobs.pop();
                    m_JobArguments.pop();
                    ++m_JobsInProgress;
                }

                job(arg);

                {
                    std::unique_lock lock{ m_QueueMutex };
                    --m_JobsInProgress;
                    if (m_Jobs.empty() && m_JobsInProgress == 0) {
                        m_AllJobsCompleted.notify_all();
                    }
                }
            }
        }

        void QueueJob(Job job, ArgumentType arg) {

            {
                std::unique_lock lock{ m_QueueMutex };

                m_Jobs.push(job);
                m_JobArguments.push(arg);
            }

            m_MutexCondition.notify_one();
        }

        void WaitForCompletion() {
            {
                std::unique_lock lock{ m_QueueMutex };
                m_AllJobsCompleted.wait(lock, [this] { return m_Jobs.empty() && m_JobsInProgress == 0; });
            }
        }

        bool WaitForCompletionOrTime(std::chrono::duration<double> time) {
            bool out;

            {
                std::unique_lock lock{ m_QueueMutex };
                out = m_AllJobsCompleted.wait_for(lock, time, [this] { return m_Jobs.empty() && m_JobsInProgress == 0; });
            }

            return out;
        }

    private:
        std::vector<std::thread> m_Threads;
        std::queue<Job> m_Jobs;
        std::queue<ArgumentType> m_JobArguments;

        std::mutex m_QueueMutex;
        std::condition_variable m_MutexCondition;

        bool m_ShouldTerminate{ false };

        std::condition_variable m_AllJobsCompleted;
        std::atomic<int> m_JobsInProgress{ 0 };
    };
}