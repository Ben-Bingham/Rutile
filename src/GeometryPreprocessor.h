#pragma once
#include "Bundle.h"

namespace Rutile {
    enum class GeometryMode {
        OPTIMIZED,
		NO_INDICES
    };

	class GeometryPreprocessor {
	public:
		Bundle GetBundle(GeometryMode mode);
	};
}