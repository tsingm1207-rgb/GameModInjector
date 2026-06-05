#pragma once
#include <stdint.h>
#include <mutex>
#include <vector>
class EventQueue
{
public:
    struct QueuedEvent
    {
        uint32_t event_type;
        std::vector<uint8_t> payload; // 事件数据的副本
    };

    void EnqueueEvent(uint32_t event_type, const void* event_data, uint32_t event_size);
    std::vector<QueuedEvent> Drain();
private:
    std::mutex mutex_;
    std::vector<QueuedEvent> events_;
};