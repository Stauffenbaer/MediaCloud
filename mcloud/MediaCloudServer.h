#pragma once

#include "MediaCloudFilesystem.h"
#include "MediaCloudDatabase.h"

namespace MediaCloud {

	class Server
	{
	public:
		Server();
		~Server();
		
		Database* database;
		Filesystem *filesystem;
	};

}