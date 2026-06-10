#include <Timer.h>

Timer::Timer()
{
    t1 = std::chrono::high_resolution_clock::now();
}

Timer::~Timer() {}

void Timer::tick()
{
    t2 = std::chrono::high_resolution_clock::now();
}

float Timer::getDeltaTime()
{
    std::chrono::duration<float> delta = t2 - t1;
    float deltaTime = delta.count();
    t1 = t2;

    return deltaTime;
}
