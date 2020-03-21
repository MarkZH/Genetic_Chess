#include "Utility/Scoped_Stopwatch.h"

#include <fstream>
#include <mutex>
#include <string>
#include <chrono>

#include "Utility/Random.h"

Scoped_Stopwatch Scoped_Stopwatch::start_stopwatch(const std::string& file_name) noexcept
{
    return Scoped_Stopwatch{file_name};
}

Scoped_Stopwatch::Scoped_Stopwatch(const std::string& name) noexcept :
    place_name(name),
    start_time(std::chrono::steady_clock::now()),
    stopped(place_name.empty())
{
}

Scoped_Stopwatch::~Scoped_Stopwatch()
{
    stop();
}

void Scoped_Stopwatch::stop() noexcept
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
             << Scoped_Stopwatch::seconds(end_time - start_time).count()
             << '\n';

    stopped = true;
}

void Scoped_Stopwatch::add_info(const std::string& info) noexcept
{
    place_name += info;
}

void Scoped_Stopwatch::reject() noexcept
{
    stopped = true;
}

Scoped_Stopwatch::seconds Scoped_Stopwatch::time_so_far() const noexcept
{
    auto end_time = std::chrono::steady_clock::now();
    return end_time - start_time;
}
