#pragma once
#include <chrono>

class Timer
{
    public:
        Timer();
        ~Timer();

        void tick();
        float getDeltaTime();

    private:
        std::chrono::high_resolution_clock::time_point t1, t2;
};