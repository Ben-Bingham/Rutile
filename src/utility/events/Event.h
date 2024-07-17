#pragma once

namespace Rutile {
    class Event {
    public:
        Event() = default;
        Event(const Event& other) = default;
        Event(Event&& other) noexcept = default;
        Event& operator=(const Event& other) = default;
        Event& operator=(Event&& other) noexcept = default;
        virtual ~Event() = default;
    };
}