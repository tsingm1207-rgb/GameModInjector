#pragma once
#include <cstdint>
#include "../../Shared/mod_abi.h"
#include <vector>
#include <mutex>
class EventBus
{
public:
    void Subscribe(uint32_t event_type, mod_event_callback_fn callback, void* user_data);
    void Dispatch(uint32_t event_type, const void* event_data, uint32_t event_size) const;
private:
    struct Subscriber
    {
        uint32_t event_type;
        mod_event_callback_fn callback;
        void* user_data;
    };

    mutable std::mutex mutex_;
    std::vector<Subscriber> subscribers_;
};