/**
 * \file include/ThreadPool.hpp
 */

#pragma once

#ifndef ThreadPool_HPP_
#define ThreadPool_HPP_

#include <mutex>
#include <queue>
#include <thread>
#include <functional>

namespace http {

/**
 * \brief A wrapper for managing vector of threads
 *
 * Ensures that all threads in the provided vector are joined upon destruction.
 *
 * \ref c++ concurrency in action by Anthony Williams
 */
class JoinThreads {
public:
    /**
     * \brief constructor.
     */
    explicit JoinThreads(std::vector<std::thread>& threads) 
        : m_threads(threads) {}

    /**
     */
    ~JoinThreads() {
        for (std::size_t i = 0; i < m_threads.size(); ++i) {
            if (m_threads[i].joinable()) {
                m_threads[i].join();
            }
        }
    }

private:
    std::vector<std::thread>& m_threads;
};


/**
 * \brief A thread-safe queue implementation.
 *
 * The ThreadsafeQueue class provides a thread-safe wrapper around a standard queue.
 * It supports multiple threads concurrently pushing and popping elements.
 *
 * \ref c++ concurrency in action by Anthony Williams
 */
template <typename T>
class ThreadsafeQueue {
public:
    /**
     * \brief Default constructor.
     */
    ThreadsafeQueue() = default;

    /**
     * \brief Copy constructor.
     *
     * \param other: The other queue to copy from.
     */
    ThreadsafeQueue(const ThreadsafeQueue& other) {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_data_queue = other.m_data_queue;
    }

    /**
     * \brief Copy assignment operator.
     *
     * \param other: The other queue to copy from.
     */
    ThreadsafeQueue& operator=(const ThreadsafeQueue& other) {
        std::lock_guard<std::mutex> lock(other.m_mutex);
        m_data_queue = other.m_data_queue;
        return *this;
    }

    /**
     * \brief Pushes a new element into the queue.
     *
     * \param new_val: The value to be pushed into the queue.
     */
    void push(T new_val) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_data_queue.push(new_val);
        m_data_cond.notify_one();
    }

    /**
     * \brief Waits and pops an element from the queue.
     *
     * \param value: The popped value is stored in this parameter.
     */
    void wait_and_pop(T& value) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_data_cond.wait(lock, [this]{ return !m_data_queue.empty(); });
        value = m_data_queue.front();
        m_data_queue.pop();
    }

    /**
     * \brief Waits and pops an element from the queue.
     *
     * \return A shared pointer to the popped value.
     */
    std::shared_ptr<T> wait_and_pop() {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_data_cond.wait(lock, [this]{ return !m_data_queue.empty(); });
        std::shared_ptr<T> res(std::make_shared<T>(m_data_queue.front()));
        m_data_queue.pop();
        return res;
    }

    /**
     * \brief Tries to pop an element from the queue without blocking.
     *
     * \param value: The popped value is stored in this parameter if successful.
     * \return True if the pop operation was successful, false otherwise.
     */
    bool try_pop(T& value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_data_queue.empty())
            return false;
        value = m_data_queue.front();
        m_data_queue.pop();
        return true;
    }

    /**
     * \brief Tries to pop an element from the queue without blocking.
     *
     * \return A shared pointer to the popped value if successful, or an empty shared pointer if the queue is empty.
     */
    std::shared_ptr<T> try_pop() {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_data_queue.empty())
            return std::shared_ptr<T>();
        std::shared_ptr<T> res(std::make_shared<T>(m_data_queue.front()));
        m_data_queue.pop();
        return res;
    }

    /**
     * \brief Checks if the queue is empty.
     *
     * \return True if the queue is empty, false otherwise.
     */
    bool empty() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_data_queue.empty();
    }

private:
    mutable std::mutex m_mutex; ///< Mutex to protect the queue.
    std::queue<T> m_data_queue; ///< Underlying queue to store data.
    std::condition_variable m_data_cond; ///< Condition variable for blocking operations.
};


/**
 * \brief A thread pool class that manages a pool of worker thread
 *
 * \ref c++ concurrency in action by Anthony Williams
 */
class ThreadPool {
public:
    /**
     * \brief Constructs a thread pool and starts the worker threads.
     */
    ThreadPool() 
        : m_done(false), m_joiner(m_threads)
    {
        // 
        std::size_t threadCount = std::thread::hardware_concurrency();

        // 
        try {
            for (std::size_t i = 0; i < threadCount; ++i) {
                m_threads.push_back(std::thread(&ThreadPool::worker_thread, this));
            }
        }
        catch (...) {
            m_done = true;
            throw;
        }
    }

    /**
     * \brief Destroys the thread pool and signals all worker threads to stop.
     */
    ~ThreadPool() {
        m_done = true;
    }

public:
    /**
     * \brief Submits a task to the thread pool.
     * \param f: The task to be executed by the thread pool.
     */
    template <typename FunctionType>
    void submit(FunctionType f) {
        m_workQueue.push(std::function<void()>(f));
    }

public:
    /**
     * \brief The function run by each worker thread.
     * 
     * Continuously fetches tasks from the work queue and executes them.
     * If no tasks are available, the thread yields.
     */
    void worker_thread() {
        while (m_done == false) {
            std::function<void()> task;
            if (m_workQueue.try_pop(task)) {
                task();
            }
            else {
                std::this_thread::yield();
            }
        }
    }

private:
    std::atomic_bool m_done;
    ThreadsafeQueue<std::function<void()>> m_workQueue;
    std::vector<std::thread> m_threads;
    JoinThreads m_joiner;
};


} // namespace http::


#endif // ThreadPool_HPP_
