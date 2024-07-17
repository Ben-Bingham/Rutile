#pragma once
#include "Event.h"

namespace Rutile {
    struct WindowResize : Event {
        int newWidth;
        int newHeight;
    };
}