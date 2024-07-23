#pragma once

namespace Rutile {
#define EVENT_IS(e, desiredType) dynamic_cast<desiredType*>(e) != nullptr

    struct Event {
        Event() = default;
        Event(const Event& other) = default;
        Event(Event&& other) noexcept = default;
        Event& operator=(const Event& other) = default;
        Event& operator=(Event&& other) noexcept = default;
        virtual ~Event() = default;
    };
}