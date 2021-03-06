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
	
	class LoginProvider
	{
	public:
		LoginProvider(Database*);
		~LoginProvider();
		
		bool Login(std::string, std::string);
		bool Register(std::string, std::string);
		
		void setLoginToken(size_t);
		void setLoginUser(std::string);
		
		std::string getLoginSalt(std::string);
		
		static std::string sha1(char*, size_t);
		static std::string sha1(std::string);
		
		static size_t random(size_t, size_t);
		
	protected:
		Database *db;
		PermissionProvider *perm;
		
	private:
		size_t loginToken;
		std::string user;
	};

}