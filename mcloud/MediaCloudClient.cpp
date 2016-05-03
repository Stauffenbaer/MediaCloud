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

#include "MediaCloudClient.h"

using namespace MediaCloud;

Client::Client(boost::asio::io_service& serv, std::string hostname) :
	ios(serv),
	sock(serv)
{
	boost::asio::ip::tcp::endpoint ep = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(hostname.c_str()), MCC_PORT);
	sock.connect(ep);
}

Client::~Client()
{
	
}

byte_buffer Client::readData()
{
	byte_buffer buf;
	
	boost::asio::streambuf *buffer = new boost::asio::streambuf();
	size_t n = 0;
	try {
		n = boost::asio::read(sock, *buffer, boost::asio::transfer_at_least(1));
	}
	catch(boost::exception& ex) {
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

void Client::writeData(byte_buffer buffer)
{
	try {
		boost::asio::write(sock, boost::asio::buffer(buffer.buffer, buffer.length));
	}
	catch(boost::exception& ex) {
		
	}
}

ServerSelector::ServerSelector(QMainWindow* parent) :
	QWidget(parent)
{
	this->setFixedSize(QSize(400, 200));
	
	main_layout = new QGridLayout(this);
	
	label_ip = new QLabel(this);
	label_ip->setText("IP-Addresse");
	
	edit_ip = new QLineEdit(this);
	
	label_user = new QLabel(this);
	label_user->setText("Username");
	
	edit_user = new QLineEdit(this);
	
	label_password = new QLabel(this);
	label_password->setText("Password");
	
	edit_password = new QLineEdit(this);
	edit_password->setEchoMode(QLineEdit::Password);
	
	button_login = new QPushButton(this);
	button_login->setText("Login");
	
	button_register = new QPushButton(this);
	button_register->setText("Register");
	
	main_layout->addWidget(label_ip, 0, 0);
	main_layout->addWidget(edit_ip, 0, 1);
	
	main_layout->addWidget(label_user, 1, 0);
	main_layout->addWidget(edit_user, 1, 1);
	
	main_layout->addWidget(label_password, 2, 0);
	main_layout->addWidget(edit_password, 2, 1);
	
	main_layout->addWidget(button_login, 3, 0);
	main_layout->addWidget(button_register, 3, 1);
	
	this->setLayout(main_layout);
}

ServerSelector::~ServerSelector()
{
	
}

MainWindow::MainWindow()
{
	this->setWindowTitle("MediaCloud");
	
	ServerSelector *sel = new ServerSelector(this);
	this->setFixedSize(sel->size());
}

MainWindow::~MainWindow()
{
	
}