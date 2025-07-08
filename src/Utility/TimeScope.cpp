#include "TimeScope.h"

#include <iostream>

namespace Rutile {
	TimeScope::TimeScope(std::chrono::duration<double>* output, const std::string& name) 
		: m_Output(output), m_Name(name) {
		m_StartTime = std::chrono::steady_clock::now();
	}

	TimeScope::~TimeScope() {
		if (m_Output != nullptr) {
			*m_Output = std::chrono::steady_clock::now() - m_StartTime;
		}
		else {
			auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - m_StartTime);
			std::cout << m_Name << ": " <<  dur << std::endl;
		}
	}
}