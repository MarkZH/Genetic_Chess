#include "Utility/Scoped_Stopwatch.h"

#include <string>
#include <chrono>
#include <vector>
#include <tuple>

std::vector<std::tuple<std::string, std::chrono::steady_clock::duration>> Scoped_Stopwatch::data{};

Scoped_Stopwatch Scoped_Stopwatch::start_stopwatch(const std::string& file_name) noexcept
{
    return Scoped_Stopwatch{file_name};
}

Scoped_Stopwatch::Scoped_Stopwatch(const std::string& name) noexcept :
    place_name(name),
    start_time(std::chrono::steady_clock::now()),
    stopped(false)
{
}

Scoped_Stopwatch::~Scoped_Stopwatch()
{
    stop();
}

void Scoped_Stopwatch::stop() noexcept
{
    if(stopped)
    {
        return;
    }

    auto end_time = std::chrono::steady_clock::now();
    data.emplace_back(place_name, end_time - start_time);
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
