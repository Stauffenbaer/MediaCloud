#include "MediaCloudServer.h"

using namespace MediaCloud;

Server::Server()
{
	filesystem = new Filesystem();
}

Server::~Server()
{
	delete filesystem;
}
