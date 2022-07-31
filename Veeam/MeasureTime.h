#pragma once
#include <chrono>
#include <iostream>

// вынести в отдельный 
class TimeDuration
{
public:
    TimeDuration()
    {
        start = std::chrono::high_resolution_clock::now();
    }
    virtual ~TimeDuration()
    {
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << duration_cast<std::chrono::milliseconds>(end - start).count() << " mSec" << std::endl;
    }
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start;
};
