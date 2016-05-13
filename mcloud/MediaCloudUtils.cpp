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

#include "MediaCloudUtils.hpp"

using namespace MediaCloud;

std::vector<std::string> Utils::explode(std::string s, char delim)
{
	std::vector<std::string> result;
	std::istringstream iss(s);
	
	for (std::string token; std::getline(iss, token, delim);)
		result.push_back(std::move(token));
	
	return result;
}

std::string Utils::replaceAll(std::string s, char a, char b)
{
	std::replace(s.begin(), s.end(), a, b);
	return s;
}

std::string Utils::deleteAll(std::string s, char c)
{
	s.erase(std::remove(s.begin(), s.end(), c), s.end());
	
	return s;
}

std::string Utils::loadFile(std::string path)
{
	std::fstream file;
	file.open(path.c_str(), std::ios::in);
	std::string content = "";
	
	std::string line;
	while(std::getline(file, line)) {
		content += line += "\n";
	}
	file.close();
	
	return content;
}

std::string byte_buffer::getString()
{
	return std::string(this->buffer);
}
