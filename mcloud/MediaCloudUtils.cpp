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

std::string byte_buffer::getString()
{
	return std::string(this->buffer);
}