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
        //! The constructor starts the stopwatch and takes a name to record.
        //
        //! \param name The name of the section of code to be timed. This name
        //!        will be written to the output file. If empty, nothing will
        //!        be written to file. Other methods can be used for displaying
        //!        time elapsed in that case.
        //! The timing result will be written to a file named "timing-N.txt",
        //! where N is a random number from 0 to 2^64 - 1.
        explicit Scoped_Stopwatch(const std::string& name);
        
        Scoped_Stopwatch(const Scoped_Stopwatch& other) = delete;
        Scoped_Stopwatch& operator=(const Scoped_Stopwatch& other) = delete;
        
        //! If the method stop() has not been called already, the destructor stops the stopwatch and records the time to a file.
        ~Scoped_Stopwatch();

        //! Manually stop the timer and record the result to a file.
        void stop();
        
        //! Add on to the name provided in the constructor.
        //
        //! This can be useful for code with branches to label which
        //! branch a given execution took.
        void add_info(const std::string& info);
        
        //! Prevent the stopwatch from recording a run.
        void reject(); // do not record time
        
        //! Find out how much time as elapsed since the stopwatch started.
        //
        //! \returns Time in seconds.
        double time_so_far() const;

    private:
        std::string place_name;
        std::chrono::steady_clock::time_point start_time;
        bool stopped;
};

#endif // SCOPED_STOPWATCH_H
