#include "Utility/Thread_Limiter.h"

#include <condition_variable>
#include <mutex>
#include <atomic>
#include <stdexcept>

Thread_Limiter::Thread_Limiter(size_t available_threads) : maximum_thread_count{available_threads}
{
    if(maximum_thread_count == 0)
    {
        throw std::invalid_argument("The available threads must be greater than zero.");
    }
}

void Thread_Limiter::ask() noexcept
{
    auto lock = std::unique_lock(m);
    cv.wait(lock, [this]() { return active_thread_count < maximum_thread_count; });
    ++active_thread_count;
}

void Thread_Limiter::done()
{
    if(active_thread_count == 0)
    {
        throw std::logic_error("Thread_Limiter::done() called without calling Thread_Limiter::ask() first.");
    }
    --active_thread_count;
    cv.notify_one();
}
