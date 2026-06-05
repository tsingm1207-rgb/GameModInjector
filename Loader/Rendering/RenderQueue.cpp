#include <Windows.h>
#include "RenderQueue.h"
#include "../../Shared/mod_abi.h"

bool RenderQueue::Enqueue(uint32_t type, const void *data, uint32_t size)
{
    if(!data && size == 0)
        return false; //TODO: log error
    if(!IsValid(type, size))
        return false; //TODO: log error
    Packet packet;
    packet.type = type;
    packet.payload.resize(size);
    std::memcpy(packet.payload.data(), data, size);
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push_back(std::move(packet));
    return true; 
}

std::vector<RenderQueue::Packet> RenderQueue::Drain()
{
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<Packet> out;
    out.reserve(queue_.size());
    while(!queue_.empty())
    {
        out.push_back(std::move(queue_.front()));
        queue_.pop_front();
    }
    return out;
}

bool RenderQueue::IsValid(uint32_t type, uint32_t size)
{
    switch(type)
    {
        case MOD_RENDER_TEXT:
            return size == sizeof(mod_render_text_command);
        case MOD_RENDER_RECT:
            return size == sizeof(mod_render_rect_command);
        default:
            return false;
    }
}
