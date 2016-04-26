#include "MediaCloudUtils.h"

using namespace MediaCloud;

std::vector<std::string> Utils::explode(std::string s, char delim)
{
	std::vector<std::string> result;
	std::istringstream iss(s);
	
	for (std::string token; std::getline(iss, token, delim);)
		result.push_back(std::move(token));
	
	return result;
}