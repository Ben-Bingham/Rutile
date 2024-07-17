#pragma once

#include "Event.h"

namespace Rutile {
    class EventListener {
    public:
        EventListener() = default;
        EventListener(const EventListener& other) = default;
        EventListener(EventListener&& other) noexcept = default;
        EventListener& operator=(const EventListener& other) = default;
        EventListener& operator=(EventListener&& other) noexcept = default;
        virtual ~EventListener() = default;

        virtual void Notify(Event* event) = 0;
    };
}