#ifndef SCOPED_STOPWATCH_H
#define SCOPED_STOPWATCH_H

#include <chrono>
#include <string>

//! A tool for timing the execution time of user-chosen sections of code.
//
//! The Scoped_Stopwatch is designed so that the constructor starts the
//! timer and the destructor stops it. So, if an instance is created at
//! the beginning of a function, the timer will stop when the function
//! exits, however that happens, whether a normal return or a thrown
//! exception. The stopwatch can also be manually stopped anywhere as well.
//!
//! Some care is needed when using this class with very fast sections of
//! code. The machinery of setting up and writing to a file adds about
//! 0.1 microseconds to the measured time. Execution times smaller than this
//! will not get accurate results.
class Scoped_Stopwatch
{
    public:
        explicit Scoped_Stopwatch(const std::string& name);
        Scoped_Stopwatch(const Scoped_Stopwatch& other) = delete;
        Scoped_Stopwatch& operator=(const Scoped_Stopwatch& other) = delete;
        ~Scoped_Stopwatch();

        void stop();
        void add_info(const std::string& info);
        void reject(); // do not record time
        double time_so_far() const;

    private:
        std::string place_name;
        std::chrono::steady_clock::time_point start_time;
        bool stopped;
};

#endif // SCOPED_STOPWATCH_H
