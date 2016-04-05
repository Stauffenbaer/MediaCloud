#pragma once

#include "MediaCloudFilesystem.h"
#include "MediaCloudDatabase.h"
#include "MediaCloudSettings.h"

namespace MediaCloud {

	class Server
	{
	public:
		Server();
		~Server();
		
		Database* database;
		Filesystem *filesystem;
		Settings *settings;
	};

}