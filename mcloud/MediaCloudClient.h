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

#define MCC_PORT 1712

#include "MediaCloudUtils.hpp"
#include "security/MediaCloudLoginProvider.h"

#include <QtGui/QMainWindow>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QLayout>
#include <QtGui/QDesktopWidget>
#include <QtGui/QMessageBox>

#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>

namespace MediaCloud {

	class Client
	{
	public:
		Client(boost::asio::io_service&, std::string);
		~Client();
		
		std::string readString();
		void writeString(std::string);
		
	protected:
		byte_buffer readData();
		void writeData(byte_buffer);
		
	private:
		boost::asio::io_service& ios;
		boost::asio::ip::tcp::socket sock;
	};
	
	class ServerSelector : public QWidget
	{
	public:
		ServerSelector(QMainWindow*);
		~ServerSelector();
		
		QLineEdit *edit_ip;
		QLineEdit *edit_user;
		QLineEdit *edit_password;
		
	protected:
		QGridLayout *main_layout;
		
		QLabel *label_ip;
		QLabel *label_user;
		QLabel *label_password;
		
		QPushButton *button_login;
		QPushButton *button_register;
	};
	
	class MainWindow : public QMainWindow
	{
		Q_OBJECT
	public:
		MainWindow();
		~MainWindow();
		
		void setupWindow();
		
	private:
		boost::asio::io_service serv;
		ServerSelector *selector;
		Client *client;
		
		bool login(std::string, std::string);
		bool usr_register(std::string, std::string);
		
	private slots:
		void lgnPressed();
		void regPressed();
	};

}