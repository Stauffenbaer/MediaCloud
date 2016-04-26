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
	:	ios(io_service),
		acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), MCS_PORT))
{
	av_register_all();
	av_log_set_level(AV_LOG_ERROR);
	
	database = new Database();
	filesystem = new Filesystem(database);
	settings = new Settings(database);
	decoder = new Decoder();
	login = new LoginProvider(database);
	audio = new AudioProvider(database, filesystem);
	
	InitializeSocket();
}

Server::~Server()
{
	networkThread.interrupt();
	
	delete database;
	delete filesystem;
	delete settings;
	delete decoder;
	delete login;
	delete audio;
}

void Server::startNetworking()
{
	networkThread = boost::thread(boost::bind(&Server::run_networkThread, this));
}

void Server::run_networkThread()
{
	while(1) {
		startAccept();
		ios.run();
	}
}

void Server::InitializeSocket()
{
	
}

void Server::startAccept()
{
	session* s = new session(this, ios);
	acceptor.accept(s->socket());
	
	s->start();
	
	s->disconnect();
	ios.stop();
}

std::string Server::byte_buffer::getString()
{
	return std::string(this->buffer);
}

Server::session::session(Server* parent, boost::asio::io_service& service)
	: service(service),
	  sock(service)
{
	this->parent = parent;
}

void Server::session::start()
{
	writeString(std::string("mediacloudserver") + parent->server_version + std::string("\n"));
	std::string msg = "";
	do {
		msg = read().getString();
		
		msg.erase(std::remove(msg.begin(), msg.end(), '\r'), msg.end());
		msg.erase(std::remove(msg.begin(), msg.end(), '\n'), msg.end());
		
		std::vector<std::string> arr = Utils::explode(msg, ' ');
		std::vector<std::string> args = std::vector<std::string>();
		
		for(size_t i = 1; i < arr.size(); ++i)
			args.push_back(arr[i]);
		
		std::string command = arr[0];
		
		if (boost::iequals(command, "QUIT"))
			break;
		
		parent->handleCommand(command, args);
		
	} while(1);
	writeString(std::string("Closed connection\n"));
}

void Server::handleCommand(std::string command, std::vector<std::string> args)
{
	if (boost::iequals(command, "REQUEST_TRACK")) {
		int ID = atoi(args[0].c_str());
		Track t = audio->getTrackByID(ID);
		
		audio->playTrack(t);
	}
	
	if (boost::iequals(command, "PAUSE"))
		audio->pauseState();
	
	if (boost::iequals(command, "PLAY"))
		audio->playState();
	
	if (boost::iequals(command, "SET_VOLUME")){
		float v = std::stof(args[0].c_str());
		std::cout << "Setting volume to " << v << std::endl;
		audio->setVolume(v);
	}
}

void Server::session::disconnect()
{
	sock.shutdown(boost::asio::socket_base::shutdown_both);
	sock.close();
}

void Server::session::write(char* buffer, size_t length)
{
	boost::asio::write(sock, boost::asio::buffer(buffer, length));
}

void Server::session::writeString(std::string string)
{
	boost::asio::write(sock, boost::asio::buffer(string.data(), string.size()));
}

void Server::session::writeBuffer(Server::byte_buffer buffer)
{
	boost::asio::write(sock, boost::asio::buffer(buffer.buffer, buffer.length));
}

Server::byte_buffer Server::session::read()
{
	boost::asio::streambuf *buffer = new boost::asio::streambuf();
	size_t n = boost::asio::read(sock, *buffer, boost::asio::transfer_at_least(1));
	Server::byte_buffer buf;
	char *bufferaddr = (char *) boost::asio::buffer_cast<const char *>(buffer->data());
	buf.length = n;
	
	buf.buffer = new char[n];
	std::copy(bufferaddr, bufferaddr + n, buf.buffer);
	
	delete buffer;
	
	return buf;
}

boost::asio::ip::tcp::socket& Server::session::socket()
{
	return sock;
}