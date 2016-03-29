#include "MediaCloudDatabase.h"

MediaCloudDatabase::MediaCloudDatabase()
{
	hashProvider = boost::uuids::detail::sha1();
	sqlite3_open(this->filename.c_str(), &this->db);
}

MediaCloudDatabase::~MediaCloudDatabase()
{
	sqlite3_close(this->db);
}

std::string MediaCloudDatabase::calculateSHA1(char *data, size_t length)
{
	char hash[20];
	hashProvider.process_bytes(data, length);
	
	unsigned int digest[5];
	hashProvider.get_digest(digest);
	
	for(int i = 0; i < 5; ++i)
	{
		const char* tmp = reinterpret_cast<char*>(digest);
		hash[4 * i + 0] = tmp[4 * i + 3];
		hash[4 * i + 1] = tmp[4 * i + 2];
		hash[4 * i + 2] = tmp[4 * i + 1];
		hash[4 * i + 3] = tmp[4 * i + 0];
	}
	
	std::stringstream stream = std::stringstream();
	
	for(int i = 0; i < 20; ++i)
    {
        stream << std::hex << ((hash[i] & 0x000000F0) >> 4) <<  (hash[i] & 0x0000000F);
    }
    
    return stream.str();
}