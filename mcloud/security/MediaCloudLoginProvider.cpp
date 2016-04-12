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

std::string sha1(char *buffer, size_t length)
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

std::string sha1(std::string string)
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

size_t random(size_t min, size_t max)
{
	boost::random::mt19937 randomProvider;
	boost::random::uniform_int_distribution<> usalt(min, max);
	
	return usalt(randomProvider);
}

LoginProvider::LoginProvider(Database *database)
{
	db = database;
}

LoginProvider::~LoginProvider()
{
	
}

LoginToken* LoginProvider::Login(std::string username, std::string password)
{
	std::stringstream query = std::stringstream();
	query << "SELECT salt, password FROM tbl_users WHERE username='";
	query << username << "'";
	
	Result *res = db->query(query.str());
	if (res->rows == 0)
		return 0;
	
	std::string salt = res->data[0].columns[0];
	
	std::string passwordHash = sha1(sha1(password) + salt);
	
	if (passwordHash != res->data[0].columns[1])
		return 0;
	
	LoginToken token = this->createToken(32);
	LoginToken *tok = new LoginToken();
	memcpy(tok, &token, sizeof(token));
	
	return tok;
}

LoginToken* LoginProvider::Register(std::string username, std::string password)
{
	size_t rand = random(11111111, 99999999);
	std::string hash = sha1((char *) &rand, sizeof(rand));
	std::string passwordHash = sha1(password);
	
	std::stringstream query = std::stringstream();
	query << "INSERT INTO tbl_users (username, password, salt) VALUES ('";
	query << username << "', '";
	query << sha1(passwordHash + hash) << "', '";
	query << hash << "')";
	db->query(query.str());
	
	return this->Login(username, password);
}

LoginToken LoginProvider::createToken(int security)
{
	size_t max = pow10(security) - 1;
	size_t min = max / 9;
	size_t rand = random(min, max);
	std::string hash = sha1((char *) &rand, sizeof(rand));
	
	LoginToken token;
	token.data = (char *) hash.c_str();
	token.length = sizeof(token.data);
	
	return token;
}