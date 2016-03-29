#pragma once

#include <string>
#include <sqlite3.h>

class MediaCloudDatabase
{
public:
	MediaCloudDatabase();
	~MediaCloudDatabase();
	
protected:
	sqlite3* db;
	
private:
	const std::string& filename = "mediacloud.db";
};