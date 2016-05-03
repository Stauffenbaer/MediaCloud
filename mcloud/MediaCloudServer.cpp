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
		acceptor(io_service, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), MCS_PORT))
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
	networkThread.join();
}

void Server::run_networkThread()
{
	InitializeSocket();
	while(server_active) {
		startAccept();
		ios.run();
	}
}

void Server::InitializeSocket()
{
	controlHandlers = std::vector<commandHandler>();
	
	controlHandlers.push_back(&Server::commandHandlerRequestTrack);
	controlHandlers.push_back(&Server::commandHandlerRequestMeta);
	controlHandlers.push_back(&Server::commandHandlerRequestAlben);
	controlHandlers.push_back(&Server::commandHandlerRequestAlbumTracks);
	controlHandlers.push_back(&Server::commandHandlerPause);
	controlHandlers.push_back(&Server::commandHandlerPlay);
	controlHandlers.push_back(&Server::commandHandlerStop);
	controlHandlers.push_back(&Server::commandHandlerSetVolume);
}

void Server::startAccept()
{
	authentificated = false;
	session* s = new session(this, ios);
	acceptor.accept(s->socket());
	
	s->start();
	
	s->disconnect();
	ios.stop();
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
		
		if (arr.size() <= 0)
			continue;
		
		std::vector<std::string> args = std::vector<std::string>();
		
		for(size_t i = 1; i < arr.size(); ++i)
			args.push_back(arr[i]);
		
		std::string command = arr[0];
		
		if (command.size() == 0)
			continue;
		
		for(size_t i = 0;i < command.size(); ++i) {
			if (command[i] == 127)
				command = command.substr(0, command.size() - 1);
		}
		
		if (boost::iequals(command, "QUIT")) {
			active = false;
			break;
		}
		
		if (boost::iequals(command, "SHUTDOWN")) {
			active = false;
			parent->server_active = false;
			break;
		}
		
		parent->handleCommand(this, command, args);
		
	} while(active);
	writeString(std::string("Closed connection\n"));
}

void Server::handleCommand(Server::session* sessptr, std::string command, std::vector<std::string> args)
{
	if (!authentificated) {
		if (commandHandlerRegisterUser(command, &args, sessptr))
			return;
		if (commandHandlerRequestLogin(command, &args, sessptr))
			return;
		if (commandHandlerValidateLogin(command, &args, sessptr))
			return;
		
		return;
	}
	for(auto it = controlHandlers.begin(); it != controlHandlers.end(); ++it) {
		commandHandler handler = *it;
		if (handler(command, &args, sessptr))
			return;
	}
	
	std::cout << "No command handler found for [" << command << "]" << std::endl;
}

void Server::session::disconnect()
{
	try {
		sock.shutdown(boost::asio::socket_base::shutdown_both);
		sock.close();
	}
	catch(boost::exception& ex) {	}
	
	active = false;
}

void Server::session::write(char* buffer, size_t length)
{
	try {
		boost::asio::write(sock, boost::asio::buffer(buffer, length));
	}
	catch(boost::exception& ex) {
		disconnect();
	}
}

void Server::session::writeString(std::string string)
{
	try {
		boost::asio::write(sock, boost::asio::buffer(string.data(), string.size()));
	}
	catch(boost::exception& ex) {
		disconnect();
	}
}

void Server::session::writeBuffer(byte_buffer buffer)
{
	try {
		boost::asio::write(sock, boost::asio::buffer(buffer.buffer, buffer.length));
	}
	catch(boost::exception& ex) {
		disconnect();
	}
}

byte_buffer Server::session::read()
{
	byte_buffer buf;
	boost::asio::streambuf *buffer = new boost::asio::streambuf();
	size_t n = 0;
	try {
		n = boost::asio::read(sock, *buffer, boost::asio::transfer_at_least(1));
	}
	catch(boost::exception& ex) {
		disconnect();
		return buf;
	}
	char *bufferaddr = (char *) boost::asio::buffer_cast<const char *>(buffer->data());
	buf.length = n + 1;
	
	buf.buffer = new char[n + 1];
	std::copy(bufferaddr, bufferaddr + n, buf.buffer);
	buf.buffer[n] = 0;
	
	delete buffer;
	
	return buf;
}

boost::asio::ip::tcp::socket& Server::session::socket()
{
	return sock;
}

bool Server::commandHandlerRegisterUser(std::string cmd, std::vector< std::string >* args, Server::session* sessionptr)
{
	if (!boost::iequals(cmd, "REGISTER_USER"))
		return false;
	
	bool succ = sessionptr->parent->login->Register((*args)[0], (*args)[1]);
	if (succ) {
		sessionptr->writeString("TRUE");
		sessionptr->parent->authentificated = true;
	}
	else
		sessionptr->writeString("FALSE");
	
	return true;
}

bool Server::commandHandlerRequestLogin(std::string cmd, std::vector< std::string >* args, Server::session* sessionptr)
{
	if (!boost::iequals(cmd, "REQUEST_LOGIN"))
		return false;
	
	size_t n = LoginProvider::random(11111111, 99999999);
	
	sessionptr->parent->login->setLoginToken(n);
	sessionptr->parent->login->setLoginUser((*args)[0]);
	
	std::string salt = sessionptr->parent->login->getLoginSalt((*args)[0]);
	
	std::stringstream stream = std::stringstream();
	stream << std::to_string(n) << ";" << salt << "\n";
	
	sessionptr->writeString(stream.str());
	return true;
}

bool Server::commandHandlerValidateLogin(std::string cmd, std::vector< std::string >* args, Server::session* sessionptr)
{
	if (!boost::iequals(cmd, "VALIDATE_LOGIN"))
		return false;
	
	std::string hash = (*args)[0];
	bool succ = sessionptr->parent->login->Login("nil", hash);
	if (succ) {
		sessionptr->writeString("TRUE");
		sessionptr->parent->authentificated = true;
	}
	else
		sessionptr->writeString("FALSE");
	
	return true;
}

bool Server::commandHandlerRequestTrack(std::string cmd, std::vector< std::string >* args, Server::session* sessionptr)
{
 	if (!boost::iequals(cmd, "REQUEST_TRACK"))
		return false;
	
	AudioProvider *audio = sessionptr->parent->audio;
	
	int ID = atoi((*args)[0].c_str());
	Track t;
	if (audio->getTrackByID(ID, &t) != -1)
		audio->playTrack(t);
	else
		sessionptr->writeString(std::string("Track-ID not found!\n"));
	
	return true;
}

bool Server::commandHandlerRequestMeta(std::string cmd, std::vector< std::string >* args, Server::session* sessionptr)
{
	if (!boost::iequals(cmd, "REQUEST_META"))
		return false;
	
	AudioProvider *audio = sessionptr->parent->audio;
	
	Track track;
	if (args->size() == 0)
		audio->getTrackByID(audio->currentTrack, &track);
	else
		audio->getTrackByID(atoi((*args)[0].c_str()), &track);
	
	std::stringstream meta = std::stringstream();
	meta << "TITLE:" << track.title << ";";
	meta << "ARTIST:" << track.artist << ";";
	meta << "ALBUM:" << track.album << ";";
	meta << "NUMBER:" << track.tracknr << "\n";
	
	sessionptr->writeString(meta.str());
	return true;
}

bool Server::commandHandlerRequestAlben(std::string cmd, std::vector<std::string>* args, Server::session* sessionptr)
{
	if (!boost::iequals(cmd, "REQUEST_ALBEN"))
		return false;
	
	AudioProvider *audio = sessionptr->parent->audio;
	
	std::stringstream meta = std::stringstream();
	std::vector<Album> alben = audio->GetAlben();
	for(auto it = alben.begin(); it != alben.end(); ++it) {
		Album a = *it;
		
		meta << "ID:" << a.ID << ";";
		meta << "NAME:" << a.name << "\n";
	}
	
	sessionptr->writeString(meta.str());
	return true;
}

bool Server::commandHandlerRequestAlbumTracks(std::string cmd, std::vector<std::string>* args, Server::session* sessionptr)
{
	if (!boost::iequals(cmd, "REQUEST_ALBUM_TRACKS"))
		return false;
	
	AudioProvider *audio = sessionptr->parent->audio;
	
	Album album;
	std::vector<Album> alben = audio->GetAlben();
	for(auto it = alben.begin(); it != alben.end(); ++it) {
		if (atoi((*args)[0].c_str()) == (*it).ID)
			album = *it;
	}
	
	std::stringstream tracklist = std::stringstream();
	
	for(auto it = album.tracks.begin(); it != album.tracks.end(); ++it) {
		Track t = *it;
		
		tracklist << t.ID << "\n";
	}
	
	sessionptr->writeString(tracklist.str());
	return true;
}

bool Server::commandHandlerPause(std::string cmd, std::vector< std::string >* args, Server::session* sessionptr)
{
	if (!boost::iequals(cmd, "PAUSE"))
		return false;
	
	AudioProvider *audio = sessionptr->parent->audio;
	
	audio->pauseState();
	
	return true;
}

bool Server::commandHandlerPlay(std::string cmd, std::vector< std::string >* args, Server::session* sessionptr)
{
	if (!boost::iequals(cmd, "PLAY"))
		return false;
	
	AudioProvider *audio = sessionptr->parent->audio;
	
	audio->playState();
	
	return true;
}

bool Server::commandHandlerStop(std::string cmd, std::vector< std::string >* args, Server::session* sessionptr)
{
	if (!boost::iequals(cmd, "STOP"))
		return false;
	
	AudioProvider *audio = sessionptr->parent->audio;
	
	audio->stopState();
	
	return true;
}

bool Server::commandHandlerSetVolume(std::string cmd, std::vector< std::string >* args, Server::session* sessionptr)
{
	if (!boost::iequals(cmd, "SET_VOLUME"))
		return false;
	
	AudioProvider *audio = sessionptr->parent->audio;
	
	float v = std::stof((*args)[0].c_str());
	v = Utils::clamp<float>(0.0f, 2.0f, v);
	audio->setVolume(v);
	
	return true;
}