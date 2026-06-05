#include "Event_Bus.h"

void EventBus::Subscribe(uint32_t event_type, mod_event_callback_fn callback, void *user_data)
{
    if(!callback) return;
    std::lock_guard<std::mutex> lock(mutex_);
    subscribers_.push_back({event_type, callback, user_data});
}

void EventBus::Dispatch(uint32_t event_type, const void *event_data, uint32_t event_size) const
{
    std::vector<Subscriber> snapshot;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        snapshot = subscribers_;
    }

    for(const auto &s : snapshot)
    {
        if(s.event_type == event_type)
        {
            s.callback(event_type, event_data, event_size, s.user_data);
        }
    }
}
