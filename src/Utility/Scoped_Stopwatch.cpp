#include "Utility/Scoped_Stopwatch.h"

#include <fstream>
#include <mutex>
#include <string>
#include <chrono>

#include "Utility/Random.h"

std::ofstream Scoped_Stopwatch::out_file;
std::mutex Scoped_Stopwatch::write_lock;

Scoped_Stopwatch::Scoped_Stopwatch(const std::string& name) :
    place_name(name),
    start_time(std::chrono::steady_clock::now()),
    stopped(place_name.empty())
{
}

Scoped_Stopwatch::~Scoped_Stopwatch()
{
    stop();
}

void Scoped_Stopwatch::stop()
{
    auto end_time = std::chrono::steady_clock::now();

    if(stopped)
    {
        return;
    }

    std::lock_guard<std::mutex> write_lock_guard(write_lock);

    if( ! out_file.is_open())
    {
        out_file.open("timings-" + std::to_string(Random::random_unsigned_int64()) + ".txt");
    }

    out_file << place_name << "|"
             << std::chrono::duration_cast<std::chrono::duration<double>>
                (end_time - start_time).count()
             << '\n';

    stopped = true;
}

void Scoped_Stopwatch::add_info(const std::string& info)
{
    place_name += info;
}

void Scoped_Stopwatch::reject()
{
    stopped = true;
}

double Scoped_Stopwatch::time_so_far() const
{
    auto end_time = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::duration<double>>
                (end_time - start_time).count();
}
