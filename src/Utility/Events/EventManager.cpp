#include "EventManager.h"
#include <iostream>

namespace Rutile {
    void EventManager::AddListener(EventListener* listener) {
        m_EventListeners.push_back(listener);
    }

    void EventManager::RemoveListener(EventListener* listener) {
        bool found = false;
        int i = 0;
        for (auto listen : m_EventListeners) {
            if (listen == listener) {
                found = true;
                break;
            }
            ++i;
        }

        if (found) {
            m_EventListeners.erase(m_EventListeners.begin() + i);
        }
        else {
            int a = 1;
        }
    }

    void EventManager::Notify(Event* event) {
        m_Events.push_back(event);
    }

    void EventManager::Distribute() {
        for (EventListener* listener : m_EventListeners) {
            for (Event* event : m_Events) {
                listener->Notify(event);
            }
        }

        for (const auto& event : m_Events) {
            delete event;
        }

        m_Events.clear();
    }

    bool EventManager::Empty() {
        return m_Events.empty();
    }
}