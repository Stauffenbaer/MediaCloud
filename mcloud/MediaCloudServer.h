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

#pragma once

#include "MediaCloudUtils.h"

#include "MediaCloudFilesystem.h"
#include "MediaCloudDatabase.h"
#include "MediaCloudSettings.h"
#include "MediaCloudDecoder.h"
#include "MediaCloudAudioPlayer.h"
#include "MediaCloudAudioProvider.h"
#include "security/MediaCloudLoginProvider.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>

namespace MediaCloud {

	#define MCS_PORT 1712
	
	class Server
	{
	public:
		Server(boost::asio::io_service&);
		~Server();
		
		Database *database;
		Filesystem *filesystem;
		Settings *settings;
		Decoder *decoder;
		LoginProvider *login;
		AudioProvider *audio;
		
		void startNetworking();
		
	protected:
		boost::asio::io_service& ios;
		boost::asio::ip::tcp::acceptor acceptor;
		
		boost::thread networkThread;
		
	private:
		std::string server_version = "1.0.0-a";
		void run_networkThread();
		
		void handleCommand(std::string, std::vector<std::string>);
		
		void InitializeSocket();
		
		struct byte_buffer {
			char *buffer;
			uint64_t length;
			
			std::string getString();
		};
		
		class session {
		public:
			session(Server*, boost::asio::io_service&);
			
			void start();
			void disconnect();
			
			boost::asio::ip::tcp::socket& socket();
			
		protected:			
			void write(char*, size_t);
			void writeString(std::string);
			void writeBuffer(byte_buffer);
			byte_buffer read();
			
			Server *parent;
			
		private:
			boost::asio::ip::tcp::socket sock;
			boost::asio::io_service& service;
			
			bool active = true;
		};
		
		void startAccept();
	};

}