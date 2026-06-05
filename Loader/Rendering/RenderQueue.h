#pragma once
#include <vector>
#include <cstdint>
#include <mutex>
#include <deque>

class RenderQueue
{
public:
    struct Packet
    {
        uint32_t type =0 ;
        std::vector<uint8_t> payload;
    };

    bool Enqueue(uint32_t type, const void* data, uint32_t size);
    std::vector<Packet> Drain();
private:
    bool IsValid(uint32_t type, uint32_t size);

    std::mutex mutex_;
    std::deque<Packet> queue_;
};