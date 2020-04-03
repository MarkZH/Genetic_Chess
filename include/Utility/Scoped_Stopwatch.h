#ifndef SCOPED_STOPWATCH_H
#define SCOPED_STOPWATCH_H

#include <chrono>
#include <string>
#include <vector>
#include <tuple>
#include <iostream>
#include <fstream>

//! \brief A tool for timing the execution of user-chosen sections of code.
//!
//! The Scoped_Stopwatch is designed so that the constructor starts the
//! timer and the destructor stops it. So, if an instance is created at
//! the beginning of a function, the timer will stop when the function
//! exits, however that happens, whether a normal return or a thrown
//! exception. The stopwatch can also be manually stopped anywhere as well.
//!
//! The accumulated timings can be printed to a file or the console using
//! print_all_timings() method.
class [[nodiscard]] Scoped_Stopwatch
{
    public:
        //! \brief The unit of time for this class is seconds represented by a double numeric type.
        using seconds = std::chrono::duration<double>;

        //! \brief Create a new stopwatch that records the time to the given file.
        //!
        //! \param name The name of the section of code to be timed. This name will
        //!        be used to label the timing information in the file. If the name is
        //!        empty, then no information will be written to file. Other methods
        //!        of the resulting Scoped_Stopwatch object can be used to access timings.
        static Scoped_Stopwatch start_stopwatch(const std::string& name) noexcept;

        template<typename Duration = Scoped_Stopwatch::seconds>
        [[nodiscard]] static auto print_guard(const std::string& file_name = "") noexcept
        {
            struct Print_Guard
            {
                Print_Guard(const std::string& file_name_in) noexcept : file_name(file_name_in) {}
                ~Print_Guard() noexcept { Scoped_Stopwatch::print_all_timings<Duration>(file_name); }
                void set_file_name(const std::string& new_file_name) noexcept { file_name = new_file_name;  }
                std::string file_name;
            };
            return Print_Guard(file_name);
        }

        Scoped_Stopwatch(const Scoped_Stopwatch& other) = delete;
        Scoped_Stopwatch& operator=(const Scoped_Stopwatch& other) = delete;
        Scoped_Stopwatch(Scoped_Stopwatch&&) = delete;
        Scoped_Stopwatch& operator=(Scoped_Stopwatch&&) = delete;

        //! \brief If the method stop() has not been called already, the destructor stops the stopwatch and records the time.
        ~Scoped_Stopwatch();

        //! \brief Manually stop the timer.
        void stop() noexcept;

        //! \brief Add on to the name provided in the constructor.
        //!
        //! This can be useful for code with branches to label which
        //! branch a given execution took.
        void add_info(const std::string& info) noexcept;

        //! \brief Prevent the stopwatch from recording a run.
        void reject() noexcept; // do not record time

        //! \brief Print the timings from all stopped timer instances.
        //!
        //! \param The name of the file where the data will be printed.
        //!
        //! If the file_name parameter is empty, the data will be printed to stdout.
        //! If any Scoped_Stopwatch instances are still running, their data will not
        //! be included in the output.
        template<typename Duration = Scoped_Stopwatch::seconds>
        static void print_all_timings(const std::string& file_name = "") noexcept
        {
            if(data.empty())
            {
                return;
            }

            auto file_output = std::ofstream(file_name);
            std::ostream& output = file_name.empty() ? std::cout : file_output;
            for(const auto& [name, time] : data)
            {
                output << name << " | " << std::chrono::duration_cast<Duration>(time).count() << '\n';
            }
        }

    private:
        std::string place_name;
        std::chrono::steady_clock::time_point start_time;
        bool stopped;
        static std::vector<std::tuple<std::string, std::chrono::steady_clock::duration>> data;

        //! \brief The constructor starts the stopwatch and takes a name to record.
        //!
        //! \param name The name of the section of code to be timed. This name
        //!        will be written to the output file. If empty, nothing will
        //!        be written to file. Other methods can be used for displaying
        //!        time elapsed in that case.
        explicit Scoped_Stopwatch(const std::string& name) noexcept;
};

#endif // SCOPED_STOPWATCH_H
