#pragma once

namespace Rutile {
	class GLEW {
	public:
		GLEW();
		GLEW(const GLEW& other) = delete;
		GLEW(GLEW&& other) noexcept = default;
		GLEW& operator=(const GLEW& other) = delete;
		GLEW& operator=(GLEW&& other) noexcept = default;
		~GLEW();
	};
}