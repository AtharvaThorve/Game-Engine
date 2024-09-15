#pragma once
#include <mutex>
#include <cstdint>

class Timeline {
private:
    std::mutex m;
    int64_t start_time;
    int64_t elapsed_paused_time;
    int64_t last_paused_time;
    int64_t tic;
    bool paused;
    Timeline* anchor;

    static uint64_t rdtsc();
    static int64_t getCurrentSystemTime();

public:
    Timeline(Timeline* anchor = nullptr, int64_t tic = 1);
    int64_t getTime();
    void pause();
    void unpause();
    void changeTic(int new_tic);
    bool isPaused();
    int64_t getTic();
};