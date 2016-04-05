#pragma once

#include "MediaCloudFilesystem.h"

namespace MediaCloud {

	class Server
	{
	public:
		Server();
		~Server();
		
	protected:
		Filesystem *filesystem;
	};

}