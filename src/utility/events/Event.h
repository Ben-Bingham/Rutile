#pragma once

namespace Rutile {
    struct Event {
        Event() = default;
        Event(const Event& other) = default;
        Event(Event&& other) noexcept = default;
        Event& operator=(const Event& other) = default;
        Event& operator=(Event&& other) noexcept = default;
        virtual ~Event() = default;
    };
}