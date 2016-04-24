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
#include <sqlite3.h>
#include <functional>
#include <array>

#include <boost/random.hpp>
#include <boost/uuid/sha1.hpp>

namespace MediaCloud {
	
	struct ResultRow 
	{
		std::vector<std::string> columns;
		
		std::string getColumn(int);
	};
	
	struct Result
	{
		size_t rows = 0;
		size_t columns = 0;
		
		std::vector<ResultRow> data;
		
		ResultRow getRow(int);
		
		std::string getData(int, int);
		
		void dump();
	};

	class Database
	{
	public:
		Database();
		~Database();
				
		Result* query(std::string);
		Result* query(std::stringstream*);
		
	protected:
		sqlite3 *db;
		
	private:
		const std::string& filename = "data/mediacloud.db";
	};

}