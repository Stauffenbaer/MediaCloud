#pragma once

#include "MediaCloudDatabase.h"

namespace MediaCloud {

	class Settings
	{
	public:
		Settings(Database*);
		~Settings();
		
 		std::string* GetValue(std::string);
		
	protected:
		Database* db;
	};

}