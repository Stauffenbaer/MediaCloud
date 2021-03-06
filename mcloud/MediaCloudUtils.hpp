/*

Copyright (c) 2016 Julian Lobe

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>

#include <QtGui/QWidget>
#include <QtCore/QString>

namespace MediaCloud {
	
	class Utils {
	public:
		static std::vector<std::string> explode(std::string, char);
		static std::string replaceAll(std::string, char, char);
		static std::string deleteAll(std::string, char);
		
		static std::string loadFile(std::string);
		
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