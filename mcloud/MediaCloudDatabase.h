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

#pragma once

#include <sstream>
#include <string>
#include <boost/random.hpp>
#include <boost/uuid/sha1.hpp>
#include <sqlite3.h>
#include <functional>
#include <array>

namespace MediaCloud {

	struct Login 
	{
		int ID;
		std::string Username;
		std::string PasswordHash;
		std::string Salt;
	};
	
	struct ResultRow 
	{
		std::vector<std::string> columns;
	};
	
	struct Result
	{
		size_t rows = 0;
		size_t columns = 0;
		
		std::vector<ResultRow> data;
	};

	class Database
	{
	public:
		Database();
		~Database();
		
		std::string calculateSHA1(char*, size_t);
		
		bool registerUser(std::string, std::string);
		bool login(std::string, std::string);
		
		Result* query(std::string);
		
		std::vector<std::string> GetAlben();
		
	protected:
		sqlite3 *db;
		
	private:
		const std::string& filename = "data/mediacloud.db";
		
		boost::uuids::detail::sha1 hashProvider;
		boost::random::mt19937 randomProvider;
		
		const unsigned int saltLength = 16;
		
		unsigned long min;
		unsigned long max;
	};

}