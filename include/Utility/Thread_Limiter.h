#ifndef THREAD_LIMITER_H
#define THREAD_LIMITER_H

#include <condition_variable>
#include <mutex>
#include <atomic>

//! \brief A class that helps to limit the number of simultaneous threads that are running simultaneously.
//! 
//! Note that this class does not create threads. It only helps to keep track of how many are running.
class Thread_Limiter
{
    public:
        //! \brief The constructor argument specifies the maximum number of threads that can run at the same time.
        //! \exception std::invalid_argument Thrown if the argument is zero.
        Thread_Limiter(size_t available_threads);

        //! \brief Ask for permission to create a thread. The call blocks if all available threads are taken.
        //! 
        //! This should be run either before the thread is created, or inside the thread before it starts its work.
        void ask() noexcept;

        //! \brief Signal that the thread has completed its work.
        //! \exception std::logic_error Thrown if called when no threads are running.
        //! 
        //! This should be called inside the created thread after it has completed its work.
        void done();

    private:
        size_t maximum_thread_count;
        std::condition_variable cv;
        std::mutex m;
        std::atomic_uint active_thread_count = 0;
};

#endif // THREAD_LIMITER_H
