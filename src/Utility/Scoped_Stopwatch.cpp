#include "Utility/Scoped_Stopwatch.h"

#include <fstream>
#include <mutex>
#include <string>
#include <chrono>

#include "Utility/Random.h"

//! The constructor starts the stopwatch and takes a name to record.
//
//! \param name The name of the section of code to be timed. This name
//!        will be written to the output file. If empty, nothing will
//!        be written to file. Other methods can be used for displaying
//!        time elapsed in that case.
//! The timing result will be written to a file named "timing-N.txt",
//! where N is a random number from 0 to 2^64 - 1.
Scoped_Stopwatch::Scoped_Stopwatch(const std::string& name) :
    place_name(name),
    start_time(std::chrono::steady_clock::now()),
    stopped(place_name.empty())
{
}

//! If the method stop() has not been called already, the destructor
//! stops the stopwatch and records the time to a file.
Scoped_Stopwatch::~Scoped_Stopwatch()
{
    stop();
}

//! Manually stop the timer and record the result to a file.
void Scoped_Stopwatch::stop()
{
    auto end_time = std::chrono::steady_clock::now();

    if(stopped)
    {
        return;
    }

    static std::mutex write_lock;
    auto write_lock_guard = std::lock_guard(write_lock);

    static std::ofstream out_file("timings-" + std::to_string(Random::random_unsigned_int64()) + ".txt");

    out_file << place_name << "|"
             << std::chrono::duration_cast<std::chrono::duration<double>>
                (end_time - start_time).count()
             << '\n';

    stopped = true;
}

//! Add on to the name provided in the constructor.
//
//! This can be useful for code with branches to label which
//! branch a given execution took.
void Scoped_Stopwatch::add_info(const std::string& info)
{
    place_name += info;
}

//! Prevent the stopwatch from recording a run.
void Scoped_Stopwatch::reject()
{
    stopped = true;
}

//! Find out how much time as elapsed since the stopwatch started.
//
//! \returns Time in seconds.
double Scoped_Stopwatch::time_so_far() const
{
    auto end_time = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::duration<double>>
                (end_time - start_time).count();
}
