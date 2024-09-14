#include "Timeline.hpp"

#if defined(_MSC_VER)
#include <intrin.h>
uint64_t Timeline::rdtsc() {
    return __rdtsc();
}
#elif defined(__GNUC__)
uint64_t Timeline::rdtsc() {
    unsigned int lo, hi;
    __asm__ volatile ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}
#else
#error "Compiler not supported!"
#endif

int64_t Timeline::getCurrentSystemTime() {
    return static_cast<int64_t>(rdtsc());
}

Timeline::Timeline(Timeline* anchor, int64_t tic) :
    start_time(0), elapsed_paused_time(0), last_paused_time(0),
    tic(tic), paused(false), anchor(anchor) {
    start_time = anchor ? anchor->getTime() : getCurrentSystemTime();
}

int64_t Timeline::getTime() {
    std::lock_guard<std::mutex> lock(m);
    if (paused) {
        return last_paused_time - start_time - elapsed_paused_time;
    }
    int64_t current_time = anchor ? anchor->getTime() : getCurrentSystemTime();
    int64_t elapsed_time = current_time - start_time - elapsed_paused_time;
    return elapsed_time / tic;
}

void Timeline::pause() {
    std::lock_guard<std::mutex> lock(m);
    if (!paused) {
        paused = true;
        last_paused_time = anchor ? anchor->getTime() : getCurrentSystemTime();
    }
}

void Timeline::unpause() {
    std::lock_guard<std::mutex> lock(m);
    if (paused) {
        paused = false;
        int64_t current_time = anchor ? anchor->getTime() : getCurrentSystemTime();
        elapsed_paused_time += current_time - last_paused_time;
    }
}

void Timeline::changeTic(int new_tic) {
    std::lock_guard<std::mutex> lock(m);
    tic = new_tic;
}

bool Timeline::isPaused() {
    std::lock_guard<std::mutex> lock(m);
    return paused;
}