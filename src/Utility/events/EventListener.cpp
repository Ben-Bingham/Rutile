#include "EventListener.h"

#include "Settings/App.h"

namespace Rutile {
    EventListener::EventListener() {
        App::eventManager.AddListener(this);
    }

    EventListener::~EventListener() {
        App::eventManager.RemoveListener(this);
    }
}