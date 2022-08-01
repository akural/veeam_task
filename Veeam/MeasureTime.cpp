#include "MeasureTime.h"

TimeDuration::TimeDuration()
{
    start = std::chrono::high_resolution_clock::now();
}

TimeDuration::~TimeDuration()
{
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Generation takes "<< duration_cast<std::chrono::milliseconds>(end - start).count() << " mSec" << std::endl;
}