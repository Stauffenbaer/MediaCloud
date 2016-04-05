#include "MediaCloudSettings.h"

using namespace MediaCloud;

Settings::Settings(Database *database)
{
	db = database;
}

Settings::~Settings()
{

}

std::string* Settings::GetValue(std::string key)
{
	std::stringstream query = std::stringstream();
	query << "SELECT value FROM tbl_settings WHERE key='";
	query << key << "'";
	Result* res = db->query(query.str());
	if (res->rows != 0)
		return new std::string(res->data[0].columns[0]);
	
	return 0;
}