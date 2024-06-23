#pragma once
#include "Bundle.h"

namespace Rutile {
    enum class GeometryMode {
        OPTIMIZED,
		NO_INDICES
    };

    enum class Primitive {
        TRIANGLE
    };

	class GeometryPreprocessor {
	public:
		Bundle GetBundle(GeometryMode mode);

		void Add(Primitive primitive);

	private:
		Bundle m_CurrentBundle;
	};
}