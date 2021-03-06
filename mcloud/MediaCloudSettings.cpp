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
	std::stringstream query;
	query << "SELECT value FROM tbl_settings WHERE key='";
	query << key << "'";
	Result *res = db->query(query.str());
	std::string *r = 0;
	if (res->rows != 0)
		r = new std::string(res->data[0].columns[0]);
	
	delete res;
	return r;
}

void Settings::SetValue(std::string key, std::string value)
{
	std::stringstream query;
	query << "SELECT * FROM tbl_settings WHERE key='" << key << "'";
	Result *res = db->query(query.str());
	if (res->rows == 0) {
		query.str(std::string());
		query << "INSERT INTO tbl_settings (key, value) VALUES ('";
		query << key << "', '";
		query << value << "')";
		db->query(query.str());
	}
	else {
		query.str(std::string());
		query << "UPDATE tbl_settings SET value='" << value << "' WHERE key='" << key << "'";
		db->query(query.str());
	}
	
	delete res;
}

bool Settings::HasKey(std::string key)
{
	std::stringstream query;
	query << "SELECT * FROM tbl_settings WHERE key='" << key << "'";
	Result *res = db->query(query.str());
	bool r = res->rows != 0;
	
	delete res;
	return r;
}
