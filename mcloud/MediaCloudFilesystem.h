#pragma once

#include "MediaCloudDatabase.h"

#include <fstream>
#include <string>
#include <algorithm>

#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem.hpp>

namespace MediaCloud {

	class Filesystem
	{
	public:
		Filesystem(Database*);
		~Filesystem();
		
		bool fileRegistered(std::string);
		
		void registerFile(std::string);
		void registerFiles(std::string);
		void registerDirectory(std::string);
		
	protected:
		Database *db;
	};

}