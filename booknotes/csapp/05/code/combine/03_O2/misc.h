#pragma once

// Require by __TIMER_LOG
#include <chrono>
#include <cstdio>

#ifndef __TIMER_LOG

#define __TIMER_LOG(str) \
for (struct { bool done; std::chrono::high_resolution_clock::time_point start; } timer = { false, std::chrono::high_resolution_clock::now() }; \
timer.done == false ? (timer.done = true) : false; \
printf(str "%.6f seconds\n", std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - timer.start).count() / double(1000000)))

#endif

