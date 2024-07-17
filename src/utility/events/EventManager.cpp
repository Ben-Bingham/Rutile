#include "EventManager.h" {

namespace Rutile {
    void EventManager::AddListener(EventListener* listener) {
        m_EventListeners.push_back(listener);
    }

    void EventManager::RemoveListener(EventListener* listener) {
        int i = 0;
        for (auto listen : m_EventListeners) {
            if (listen == listener) {
                break;
            }
            ++i;
        }

        m_EventListeners.erase(m_EventListeners.begin() + i);
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

        m_Events.clear();
    }
}