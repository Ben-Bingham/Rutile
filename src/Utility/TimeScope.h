#pragma once

#include <chrono>
#include <string>

namespace Rutile {
	// Measures the amount of time it takes to reach the end of the current scope
	class TimeScope {
	public:
		TimeScope(std::chrono::duration<double>* output = nullptr, const std::string& name = "");
		TimeScope(const TimeScope& other) = delete;
		TimeScope(TimeScope&& other) noexcept = default;
		TimeScope& operator=(const TimeScope& other) = delete;
		TimeScope& operator=(TimeScope&& other) noexcept = default;
		~TimeScope();

	private:
		std::chrono::duration<double>* m_Output;
		std::chrono::time_point<std::chrono::steady_clock> m_StartTime;

		std::string m_Name;
	};
}