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

#include <iostream>

#include "mcloud/MediaCloudServer.h"

#include <boost/asio.hpp>

using namespace MediaCloud;

int main(int argc, char **argv) 
{
	boost::asio::io_service serv;
	MediaCloud::Server *server = new MediaCloud::Server(serv);
	
	if (server->settings->HasKey(MCD_ENTRY_ROOT_DIRECTORY)) {
		std::string *root = server->settings->GetValue(MCD_ENTRY_ROOT_DIRECTORY);
		if (root != 0) {
			server->filesystem->registerDirectory(*root);
			server->filesystem->registerCovers();
		}
	}
	
	server->startNetworking();
	
	delete server;
	return 0;
}