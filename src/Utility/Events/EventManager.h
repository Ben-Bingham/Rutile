#pragma once
#include "EventListener.h"
#include <vector>

namespace Rutile {
    class EventManager {
    public:
        void AddListener(EventListener* listener);
        void RemoveListener(EventListener* listener);

        void Notify(Event* event);

        void Distribute(); // TODO need to control who can call this

        bool Empty();

    private:
        std::vector<EventListener*> m_EventListeners;

        std::vector<Event*> m_Events;
    };
}