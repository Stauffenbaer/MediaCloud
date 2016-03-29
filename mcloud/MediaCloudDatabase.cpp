#include "MediaCloudDatabase.h"

MediaCloudDatabase::MediaCloudDatabase()
{
	sqlite3_open(this->filename.c_str(), &this->db);
}

MediaCloudDatabase::~MediaCloudDatabase()
{
	sqlite3_close(this->db);
}
