#ifndef SCOPED_STOPWATCH_H
#define SCOPED_STOPWATCH_H

#include <chrono>
#include <mutex>
#include <string>
#include <fstream>

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
        static std::ofstream out_file;
        static std::mutex write_lock;
        std::string place_name;
        std::chrono::steady_clock::time_point start_time;
        bool stopped;
};

#endif // SCOPED_STOPWATCH_H
