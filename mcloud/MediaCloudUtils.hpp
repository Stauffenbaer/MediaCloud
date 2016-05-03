#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

namespace MediaCloud {
	
	class Utils {
	public:
		static std::vector<std::string> explode(std::string, char);
		static std::string replaceAll(std::string, char, char);
		static std::string deleteAll(std::string, char);
		
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
	
	struct byte_buffer {
		char *buffer;
		uint64_t length;
		
		std::string getString();
	};
	
}