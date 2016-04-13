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

#include "MediaCloudServer.h"

using namespace MediaCloud;

Server::Server(boost::asio::io_service& io_service)
	: ios(io_service)
{
	av_register_all();
	av_log_set_level(AV_LOG_ERROR);
	
	database = new Database();
	filesystem = new Filesystem(database);
	settings = new Settings(database);
	decoder = new Decoder();
	login = new LoginProvider(database);
	
	boost::asio::ssl::context ctx(boost::asio::ssl::context::sslv23);
	
	this->controlsocket = new boost::asio::ssl::stream<boost::asio::ip::tcp::socket>(ios, ctx);
	this->datasocket = new boost::asio::ssl::stream<boost::asio::ip::tcp::socket>(ios, ctx);
	
	this->controlacceptor = new boost::asio::ip::tcp::acceptor(ios, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), controlport));
	this->dataacceptor = new boost::asio::ip::tcp::acceptor(ios, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), dataport));
}

Server::~Server()
{
	delete database;
	delete filesystem;
	delete settings;
	delete decoder;
	delete login;
}
