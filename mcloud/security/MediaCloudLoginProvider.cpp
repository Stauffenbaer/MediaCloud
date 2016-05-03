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

#include "MediaCloudLoginProvider.h"

using namespace MediaCloud;

std::string LoginProvider::sha1(char *buffer, size_t length)
{
	boost::uuids::detail::sha1 hashProvider;
	
	char hash[20];
	hashProvider.process_bytes(buffer, length);
	
	unsigned int digest[5];
	hashProvider.get_digest(digest);
	
	for(int i = 0; i < 5; ++i)
	{
		const char* tmp = reinterpret_cast<char*>(digest);
		hash[4 * i + 0] = tmp[4 * i + 3];
		hash[4 * i + 1] = tmp[4 * i + 2];
		hash[4 * i + 2] = tmp[4 * i + 1];
		hash[4 * i + 3] = tmp[4 * i + 0];
	}
	
	std::stringstream stream = std::stringstream();
	
	for(int i = 0; i < 20; ++i)
    {
        stream << std::hex << ((hash[i] & 0x000000F0) >> 4) <<  (hash[i] & 0x0000000F);
    }
    
    return stream.str();
}

std::string LoginProvider::sha1(std::string string)
{
	char *buffer = (char *) string.c_str();
	size_t length = string.size();
	
	boost::uuids::detail::sha1 hashProvider;
	
	char hash[20];
	hashProvider.process_bytes(buffer, length);
	
	unsigned int digest[5];
	hashProvider.get_digest(digest);
	
	for(int i = 0; i < 5; ++i)
	{
		const char* tmp = reinterpret_cast<char*>(digest);
		hash[4 * i + 0] = tmp[4 * i + 3];
		hash[4 * i + 1] = tmp[4 * i + 2];
		hash[4 * i + 2] = tmp[4 * i + 1];
		hash[4 * i + 3] = tmp[4 * i + 0];
	}
	
	std::stringstream stream = std::stringstream();
	
	for(int i = 0; i < 20; ++i)
    {
        stream << std::hex << ((hash[i] & 0x000000F0) >> 4) <<  (hash[i] & 0x0000000F);
    }
    
    return stream.str();
}

size_t LoginProvider::random(size_t min, size_t max)
{
	typedef boost::mt19937 rng_t;
	rng_t rng(time(0));
	
	boost::uniform_int<> r(min, max);
	boost::variate_generator<rng_t, boost::uniform_int<>> k(rng, r);
	
	size_t n = k();
	
	return n;
}

LoginProvider::LoginProvider(Database *database)
{
	db = database;
	perm = new PermissionProvider(database);
}

LoginProvider::~LoginProvider()
{
	delete perm;
}

void LoginProvider::setLoginToken(size_t n)
{
	this->loginToken = n;
}

void LoginProvider::setLoginUser(std::string username)
{
	user = username;
}

std::string LoginProvider::getLoginSalt(std::string username)
{
	std::stringstream query = std::stringstream();
	query << "SELECT salt FROM tbl_users WHERE username='";
	query << username << "'";
	
	Result *res = db->query(query.str());
	if (res->rows == 0)
		return "";
	
	return res->data[0].columns[0];
}

bool LoginProvider::Login(std::string username, std::string passwordHashToken)
{
	if (username == "nil")
		username = user;
	
	std::stringstream query = std::stringstream();
	query << "SELECT password FROM tbl_users WHERE username='" << username << "'";
	Result *res = db->query(query.str());
	
	std::string password = res->getData(0, 0);
	std::string token = std::to_string(loginToken);
	std::string fin = sha1(password + token);
	
	return fin == passwordHashToken;
}

bool LoginProvider::Register(std::string username, std::string password)
{
	std::stringstream query = std::stringstream();
	query << "SELECT ID FROM tbl_users WHERE username='";
	query << username << "'";
	if (db->query(query.str())->rows != 0)
		return false;
	
	size_t rand = random(11111111, 99999999);
	std::string hash = sha1((char *) &rand, sizeof(rand));
	std::string passwordHash = password;
	
	query = std::stringstream();
	query << "INSERT INTO tbl_users (username, password, salt) VALUES ('";
	query << username << "', '";
	query << sha1(passwordHash + hash) << "', '";
	query << hash << "')";
	db->query(query.str());
	
	perm->registerUser(username);
	
	return true;
}