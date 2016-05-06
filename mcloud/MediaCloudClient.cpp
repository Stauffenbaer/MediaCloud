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

std::string Client::readString()
{
	byte_buffer buf = this->readData();
	std::string s = std::string(buf.buffer);
	
	s = Utils::deleteAll(s, '\n');
	
	return s;
}

void Client::writeString(std::string string)
{
	boost::asio::write(sock, boost::asio::buffer(string.data(), string.size()));
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
	
	connect(button_login, SIGNAL(pressed()), parent, SLOT(lgnPressed()));
	connect(button_register, SIGNAL(pressed()), parent, SLOT(regPressed()));
}

ServerSelector::~ServerSelector()
{
	
}

SelectorWindow::SelectorWindow()
{
	client = 0;
	this->setWindowTitle("MediaCloud");
	
	selector = new ServerSelector(this);
	this->setFixedSize(selector->size());
}

SelectorWindow::~SelectorWindow()
{
	if (client != 0)
		delete client;
}

void SelectorWindow::setupWindow()
{
	this->hide();
	MainWindow wnd;
	wnd.show();
	
	QEventLoop loop;
    connect(&wnd, SIGNAL(destroyed()), &loop, SLOT(quit()));
    loop.exec();
	
	client->writeString("QUIT");
	
	this->close();
}

bool SelectorWindow::login(std::string username, std::string password)
{
	client->writeString(std::string("REQUEST_LOGIN~") + username);
	
	std::string result = client->readString();
	std::vector<std::string> var = Utils::explode(result, ';');
	if (var.size() != 2)
		return false;
	
	std::string token = var[0];
	std::string salt = var[1];
	std::string passwordHash = LoginProvider::sha1(password);
	std::string passw = LoginProvider::sha1(passwordHash + salt);
	std::string hash = LoginProvider::sha1(passw + token);
	
	client->writeString(std::string("VALIDATE_LOGIN~") + hash);
	result = client->readString();
	
	return boost::iequals(result, "TRUE");
}

bool SelectorWindow::usr_register(std::string username, std::string password)
{
	std::string pwHash = LoginProvider::sha1(password);
	client->writeString(std::string("REGISTER_USER~") + username + "~" + pwHash);
	std::string result = client->readString();
	
	return boost::iequals(result, "TRUE");
}

void SelectorWindow::lgnPressed()
{
	std::string hostname = selector->edit_ip->text().toStdString();
	std::string username = selector->edit_user->text().toStdString();
	std::string password = selector->edit_password->text().toStdString();
	
	try {
		client = new Client(serv, hostname);
	}
	catch (boost::exception& ex) {
		QMessageBox box;
		box.setText((std::string("Cannot connect to: ") + hostname).c_str());
		box.setWindowTitle("MediaCloud");
		box.setIcon(QMessageBox::Critical);
		box.exec();
		
		return;
	}
	
	if (username.length() == 0 || password.length() == 0)
		return;
	
	std::string version = client->readString();
	
	if (login(username, password))
		setupWindow();
	else {
		QMessageBox box;
		box.setText("Cannot login to server!");
		box.setWindowTitle("MediaCloud");
		box.setIcon(QMessageBox::Critical);
		box.exec();
	}
}

void SelectorWindow::regPressed()
{
	std::string hostname = selector->edit_ip->text().toStdString();
	std::string username = selector->edit_user->text().toStdString();
	std::string password = selector->edit_password->text().toStdString();
	
	try {
		client = new Client(serv, hostname);
	}
	catch (boost::exception& ex) {
		QMessageBox box;
		box.setText((std::string("Cannot connect to: ") + hostname).c_str());
		box.setWindowTitle("MediaCloud");
		box.setIcon(QMessageBox::Critical);
		box.exec();
		
		return;
	}
	
	std::string version = client->readString();
	
	if (username.length() == 0 || password.length() == 0)
		return;
	
	if (usr_register(username, password))
		setupWindow();
	else {
		QMessageBox box;
		box.setText("Cannot login to server!");
		box.setWindowTitle("MediaCloud");
		box.setIcon(QMessageBox::Critical);
		box.exec();
	}
}

MainWidget::MainWidget(QMainWindow* parent) :
	QWidget(parent)
{
	resize(QSize(800, 600));
	
	sidebar_frame = new QFrame(this); sidebar_frame->setStyleSheet("background-color:fuchsia;");
	content_frame = new QFrame(this); content_frame->setStyleSheet("background-color:red;");
	player_frame = new QFrame(this); player_frame->setStyleSheet("background-color:green;");
	
	sidebar_frame->setFixedWidth(200); player_frame->setFixedHeight(50);
	
	sidebar_layout = new QGridLayout(sidebar_frame);
	
	//TODO: add sidebar
	
	sidebar_frame->setLayout(sidebar_layout);
	
	content_layout = new QGridLayout(content_frame);
	
	//TODO: add content
	
	content_frame->setLayout(content_layout);
	
	player_layout = new QGridLayout(player_frame);
	
	//TODO: add player
	
	player_frame->setLayout(player_layout);
	
	main_layout = new QGridLayout(this);
	main_layout->setMargin(0);
	
	main_layout->addWidget(sidebar_frame, 0, 0, 0, 1);
	main_layout->addWidget(content_frame, 0, 1);
	main_layout->addWidget(player_frame, 1, 1);
	
	this->setLayout(main_layout);
}

MainWidget::~MainWidget()
{
	
}

MainWindow::MainWindow() :
	QMainWindow()
{
	this->setWindowTitle("MediaCloud");
	
	widget = new MainWidget(this);
	resize(widget->size());
}

MainWindow::~MainWindow()
{
	
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);
	widget->resize(event->size());
}