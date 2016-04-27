#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace MediaCloud {
	
	class Utils {
	public:
		static std::vector<std::string> explode(std::string, char);
		
		template<typename T>
		static T clamp(T min, T max, T value)
		{
			if (value > max)
				return max;
			if (value < min)
				return min;
			
			return value;
		}
	};
	
}