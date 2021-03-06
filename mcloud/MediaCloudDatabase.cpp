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

#include "MediaCloudDatabase.h"

using namespace MediaCloud;

Database::Database()
{
	sqlite3_open(this->filename.c_str(), &this->db);
	
	this->query("CREATE TABLE IF NOT EXISTS tbl_users (ID INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT, password TEXT, salt TEXT)");
	this->query("CREATE TABLE IF NOT EXISTS tbl_files (ID INTEGER PRIMARY KEY AUTOINCREMENT, path TEXT, type TEXT)");
	this->query("CREATE TABLE IF NOT EXISTS tbl_settings (ID INTEGER PRIMARY KEY AUTOINCREMENT, key TEXT, value TEXT)");
	this->query("CREATE TABLE IF NOT EXISTS tbl_music (ID INTEGER PRIMARY KEY AUTOINCREMENT, file INTEGER, title TEXT, artist TEXT, album TEXT, tracknr INT, duration INT)");
	this->query("CREATE TABLE IF NOT EXISTS tbl_perm (ID INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT, level INT(8), name TEXT)");
	this->query("CREATE TABLE IF NOT EXISTS tbl_playlists (ID INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, user INT(8))");
	this->query("CREATE TABLE IF NOT EXISTS tbl_playlisttracks (ID INTEGER PRIMARY KEY AUTOINCREMENT, plistid INT(8), plistindex INT(8), trackid INT(8))");
	this->query("CREATE TABLE IF NOT EXISTS tbl_covers (ID INTEGER PRIMARY KEY AUTOINCREMENT, trackid INT(8), cover TEXT)");
}

Database::~Database()
{
	sqlite3_close(this->db);
}

Result* Database::query(std::string query)
{
	Result* res = new Result();
	
	sqlite3_stmt* statement;
	int r = sqlite3_prepare(db, query.c_str(), -1, &statement, 0);
	if (r != 0)
		return 0;

	int c = sqlite3_column_count(statement);
	res->columns = c;
	
	res->data = std::vector<ResultRow>();
	while (true) {
        int s = sqlite3_step (statement);
        if (s == SQLITE_ROW) {
			ResultRow row = ResultRow();
			for(int i = 0;i < res->columns; ++i) {
				const unsigned char* text = sqlite3_column_text (statement, i);
				if (text != 0)
					row.columns.push_back(std::string((char*) text));
			}
			res->data.push_back(row);
        }
        else if (s == SQLITE_DONE)
            break;
    }
    
    sqlite3_finalize(statement);
	
	res->rows = res->data.size();
	return res;
}

Result* Database::query(std::stringstream* query)
{
	return this->query(query->str());
}

ResultRow Result::getRow(int n)
{
	ResultRow r = this->data[n];
	
	return r;
}

std::string Result::getData(int n, int k)
{
	std::string s = this->data[n].columns[k];
	
	return s;
}

std::string ResultRow::getColumn(int k)
{
	std::string s = this->columns[k];
	
	return s;
}

void Result::dump()
{
	for (int i = 0;i < rows; ++i) {
		std::cout << "[r: " << i << "]" << std::endl;
		for(int j = 0;j < columns; ++j) {
			std::cout << "    {c: " << j << "}" << data[i].columns[j] << std::endl;
		}
		std::cout << "[/r]" << std::endl;
	}
}