#include <iostream>

#include "mcloud/MediaCloudServer.h"

int main(int argc, char **argv) 
{
	MediaCloud::Server *server = new MediaCloud::Server();
	
	server->filesystem->registerDirectory("/home/julian/Musik/");
	
	return 0;
}
