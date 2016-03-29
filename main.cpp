#include <iostream>

#include "mcloud/MediaCloudDatabase.h"

int main(int argc, char **argv) 
{
    MediaCloudDatabase* db = new MediaCloudDatabase();
	std::string s = "Hallo Welt!";
	std::cout << db->calculateSHA1((char*) s.c_str(), (size_t) s.length()) << std::endl;
    
	getchar();
	return 0;
}
