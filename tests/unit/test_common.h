#pragma once
#include <chrono>
#include <thread>

namespace {

uint64_t rand64(uint64_t& x)
{
    x ^= x >> 12;
    x ^= x << 25;
    x ^= x >> 27;
    return x * UINT64_C(2685821657736338717);
}

uint64_t seed()
{
    auto i = std::chrono::system_clock::now().time_since_epoch();
    return i.count();
}

void sleep(uint32_t ns)
{
    std::this_thread::sleep_for(std::chrono::microseconds(ns));
}

} // namespace {}
