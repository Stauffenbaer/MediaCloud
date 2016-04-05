#include "MediaCloudServer.h"

using namespace MediaCloud;

Server::Server()
{
	database = new Database();
	filesystem = new Filesystem(database);
	settings = new Settings(database);
}

Server::~Server()
{
	delete database;
	delete filesystem;
	delete settings;
}
