#include "EventQueue.h"

void EventQueue::EnqueueEvent(uint32_t event_type, const void *event_data, uint32_t event_size)
{
    if (event_size > 0 && event_data == nullptr) return; // 无效事件数据

    QueuedEvent e;
    e.event_type = event_type;
    e.payload.resize(event_size);
    if (event_size > 0)
    {
        std::memcpy(e.payload.data(), event_data, event_size);
    }
    std::lock_guard<std::mutex> lock(mutex_);
    events_.push_back(std::move(e) );
}

std::vector<EventQueue::QueuedEvent> EventQueue::Drain()
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto out = events_;
    events_.clear();
    return out;
}
