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

#include "../MediaCloudDatabase.h"
#include "MediaCloudPermissionProvider.h"

#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <boost/random.hpp>
#include <boost/uuid/sha1.hpp>

namespace MediaCloud {
	
	struct LoginToken {
		char *data;
		size_t length;
		std::string string;
	};
	
	class LoginProvider
	{
	public:
		LoginProvider(Database*);
		~LoginProvider();
		
		LoginToken* Login(std::string, std::string);
		LoginToken* Register(std::string, std::string);
		
	protected:
		Database *db;
		PermissionProvider *perm;
		
		std::string sha1(char*, size_t);
		std::string sha1(std::string);
	
		size_t random(size_t, size_t);
		
	private:
		LoginToken* createToken(int);
	};

}