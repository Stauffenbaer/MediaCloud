#pragma once

#include <sstream>
#include <string>
#include <boost/random.hpp>
#include <boost/uuid/sha1.hpp>
#include <sqlite3.h>
#include <functional>

namespace MediaCloud {

	struct Login 
	{
		int ID;
		std::string Username;
		std::string PasswordHash;
		std::string Salt;
	};

	class Database
	{
	public:
		Database();
		~Database();
		
		std::string calculateSHA1(char*, size_t);
		
		bool registerUser(std::string, std::string);
		bool login(std::string, std::string);
		
	protected:
		sqlite3 *db;
		
	private:
		const std::string& filename = "mediacloud.db";
		
		boost::uuids::detail::sha1 hashProvider;
		boost::random::mt19937 randomProvider;
		
		const unsigned int saltLength = 16;
		
		unsigned long min;
		unsigned long max;
	};

}