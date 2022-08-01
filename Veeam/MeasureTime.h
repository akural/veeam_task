#pragma once
#include <chrono>
#include <iostream>

class TimeDuration
{
public:
    TimeDuration();
    virtual ~TimeDuration();
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
};
