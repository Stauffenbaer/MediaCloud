#pragma once

#include <sstream>
#include <string>
#include <boost/uuid/sha1.hpp>
#include <sqlite3.h>

struct MediaCloudLogin 
{
	int ID;
	std::string Username;
	std::string PasswordHash;
	std::string Salt;
};

class MediaCloudDatabase
{
public:
	MediaCloudDatabase();
	~MediaCloudDatabase();
	
	std::string calculateSHA1(char*, size_t);
	
protected:
	sqlite3 *db;
	
private:
	const std::string& filename = "mediacloud.db";
	
	boost::uuids::detail::sha1 hashProvider;
	
	
};