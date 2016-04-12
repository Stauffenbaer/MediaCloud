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

#include "MediaCloudPermissionProvider.h"

using namespace MediaCloud;

PermissionProvider::PermissionProvider(Database *database)
{
	db = database;
}

PermissionProvider::~PermissionProvider()
{

}

void PermissionProvider::registerUser(std::string username)
{
	std::stringstream query = std::stringstream();
	query << "INSERT INTO tbl_perm (username, level, name) VALUES ('" << username << "', '0', 'default')";
	db->query(query.str());
}

void PermissionProvider::setPermissionLevel(std::string username, PermissionSet set)
{
	std::stringstream query = std::stringstream();
	query << "UPDATE tbl_perm SET level='" << set.level << "' WHERE username='" << username << "'";
	db->query(query.str());
	
	query = std::stringstream();
	query << "UPDATE tbl_perm SET name='" << set.name << "' WHERE username='" << username << "'";
	db->query(query.str());
}