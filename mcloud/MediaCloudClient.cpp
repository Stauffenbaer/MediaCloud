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

LanguageProvider* Provider::lang = 0;

Client::Client(boost::asio::io_service& serv, std::string hostname) :
	ios(serv),
	sock(serv)
{	
	boost::asio::ip::tcp::endpoint ep = boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(hostname.c_str()), MCC_PORT);
	sock.connect(ep);
}

Client::~Client()
{
	sock.close();
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
	label_ip->setText(Provider::lang->getValue("client.login_ip_addr").c_str());
	
	edit_ip = new QLineEdit(this);
	
	label_user = new QLabel(this);
	label_user->setText(Provider::lang->getValue("client.login_username").c_str());
	
	edit_user = new QLineEdit(this);
	
	label_password = new QLabel(this);
	label_password->setText(Provider::lang->getValue("client.login_password").c_str());
	
	edit_password = new QLineEdit(this);
	edit_password->setEchoMode(QLineEdit::Password);
	
	button_login = new QPushButton(this);
	button_login->setText(Provider::lang->getValue("client.login_login_btn").c_str());
	
	button_register = new QPushButton(this);
	button_register->setText(Provider::lang->getValue("client.login_register_btn").c_str());
	
	checkbox = new QCheckBox(this);
	checkbox->setText(Provider::lang->getValue("client.login_remember").c_str());
	checkbox->setCheckState(Qt::Checked);
	
	main_layout->addWidget(label_ip, 0, 0);
	main_layout->addWidget(edit_ip, 0, 1);
	
	main_layout->addWidget(label_user, 1, 0);
	main_layout->addWidget(edit_user, 1, 1);
	
	main_layout->addWidget(label_password, 2, 0);
	main_layout->addWidget(edit_password, 2, 1);
	
	main_layout->addWidget(checkbox, 3, 0);
	
	main_layout->addWidget(button_login, 4, 0);
	main_layout->addWidget(button_register, 4, 1);
	
	this->setLayout(main_layout);
	
	button_login->setDefault(true);
	
	connect(button_login, SIGNAL(clicked()), parent, SLOT(lgnPressed()));
	connect(button_register, SIGNAL(clicked()), parent, SLOT(regPressed()));
	
	connect(edit_password, SIGNAL(returnPressed()), button_login, SIGNAL(clicked()));
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
	
	fileMenu = new QAction((std::string("&") + Provider::lang->getValue("client.menu.file")).c_str(), this);
	fileMenu->setMenu(new QMenu());
	
	QAction *quitAction = new QAction(Provider::lang->getValue("client.menu.file.quit").c_str(), this);
	quitAction->setShortcut(Qt::CTRL + Qt::Key_Q);
	fileMenu->menu()->addAction(quitAction);
	connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
	
	setMenuBar(new QMenuBar(this));
	menuBar()->addAction(fileMenu);
	
	if (boost::filesystem::exists("data/credentials")) {
		std::string content = Utils::loadFile("data/credentials");
		std::vector<std::string> elements = Utils::explode(content, '\n');
		selector->edit_ip->setText(elements[0].c_str());
		selector->edit_user->setText(elements[1].c_str());
	}
}

SelectorWindow::~SelectorWindow()
{
	if (client != 0)
	try {
		delete client;
	}
	catch (std::exception& ex) {
		
	}
}

void SelectorWindow::setupWindow(Client* client)
{
	this->hide();
	MainWindow wnd(client);
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
		
		if (username.length() == 0 || password.length() == 0)
		return;
	
		std::string version = client->readString();
		
		if (login(username, password)) {
			if (selector->checkbox->isChecked()) {
				std::stringstream str;
				str << selector->edit_ip->text().toStdString() << "\n";
				str << selector->edit_user->text().toStdString();
				std::string s = str.str();
				std::fstream fs;
				fs.open("data/credentials", std::ios::out);
				fs.clear();
				fs.write(s.data(), s.size());
				fs.close();
			}
			setupWindow(client);
		}
		else {
			try {
				delete client;
			}
			catch (std::exception& ex) {
				
			}
			QMessageBox box;
			box.setText(Provider::lang->getValue("client.error_login").c_str());
			box.setWindowTitle("MediaCloud");
			box.setIcon(QMessageBox::Critical);
			box.exec();
		}
	}
	catch (boost::exception& ex) {
		QMessageBox box;
		box.setText((Provider::lang->getValue("client.error_connect") + hostname).c_str());
		box.setWindowTitle("MediaCloud");
		box.setIcon(QMessageBox::Critical);
		box.exec();
		
		return;
	}
}

void SelectorWindow::regPressed()
{
	std::string hostname = selector->edit_ip->text().toStdString();
	std::string username = selector->edit_user->text().toStdString();
	std::string password = selector->edit_password->text().toStdString();
	
	try {
		client = new Client(serv, hostname);
		
		std::string version = client->readString();
		
		if (username.length() == 0 || password.length() == 0)
			return;
		
		if (usr_register(username, password))
			setupWindow(client);
		else {
			try {
				delete client;
			}
			catch (std::exception& ex) {
				
			}
			QMessageBox box;
			box.setText(Provider::lang->getValue("client.error_register").c_str());
			box.setWindowTitle("MediaCloud");
			box.setIcon(QMessageBox::Critical);
			box.exec();
		}
	}
	catch (boost::exception& ex) {
		QMessageBox box;
		box.setText((Provider::lang->getValue("client.error_connect") + hostname).c_str());
		box.setWindowTitle("MediaCloud");
		box.setIcon(QMessageBox::Critical);
		box.exec();
		
		return;
	}
}

SidebarWidget::SidebarWidget(QWidget* parent) :
	QWidget(parent)
{
	media_selector = new QFrame(this);
	media_selector->setObjectName("media_selector");
	media_selector->setFixedHeight(100);
	QGridLayout *selector_layout = new QGridLayout(this);
	
	media_music = new QLabel("media_music", this);
	media_music->setText(Provider::lang->getValue("client.media_selector.music").c_str());
	media_music->setFixedHeight(25);
	
	media_video = new QLabel("media_video", this);
	media_video->setText(Provider::lang->getValue("client.media_selector.video").c_str());
	media_video->setFixedHeight(25);
	
	selector_layout->addWidget(media_music, 0, 0, Qt::AlignTop);
	selector_layout->addWidget(media_video, 1, 0, Qt::AlignTop);
	
	media_selector->setLayout(selector_layout);
	
	main_layout = new QGridLayout(this);
	
	main_layout->addWidget(media_selector, 0, 0, Qt::AlignTop);
	
	setLayout(main_layout);
	
	this->setObjectName("sidebar");
}

SidebarWidget::~SidebarWidget()
{
	
}

MainWidget::MainWidget(QMainWindow* parent) :
	QWidget(parent)
{
	resize(QSize(800, 600));
	
	sidebar = new SidebarWidget(this);
	content_frame = new QFrame(this);
	player_frame = new QFrame(this);
	
	sidebar->setFixedWidth(200); player_frame->setFixedHeight(50);
	
	content_layout = new QGridLayout(content_frame);
	
	//TODO: add content
	
	content_frame->setLayout(content_layout);
	
	player_layout = new QGridLayout(player_frame);
	
	//TODO: add player
	
	player_frame->setLayout(player_layout);
	
	main_layout = new QGridLayout(this);
	main_layout->setMargin(0);
	
	main_layout->addWidget(sidebar, 0, 0, 0, 1);
	main_layout->addWidget(content_frame, 0, 1);
	main_layout->addWidget(player_frame, 1, 1);
	
	this->setLayout(main_layout);
	
	this->setObjectName("main_widget");
	this->setStyleSheet(Utils::deleteAll(Utils::loadFile("data/resources/style/client.css"), '\n').c_str());
}

MainWidget::~MainWidget()
{
	
}

MainWindow::MainWindow(Client* c) :
	QMainWindow()
{
	this->client = c;
	this->setWindowTitle("MediaCloud");
	
	widget = new MainWidget(this);
	resize(widget->size());
	
	fileMenu = new QAction((std::string("&") + Provider::lang->getValue("client.menu.file")).c_str(), this);
	fileMenu->setMenu(new QMenu());
	
	QAction *settingsAction = new QAction(Provider::lang->getValue("client.menu.file.settings").c_str(), this);
	settingsAction->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_S);
	fileMenu->menu()->addAction(settingsAction);
	connect(settingsAction, SIGNAL(triggered()), this, SLOT(openSettings()));
	
	QAction *quitAction = new QAction(Provider::lang->getValue("client.menu.file.quit").c_str(), this);
	quitAction->setShortcut(Qt::CTRL + Qt::Key_Q);
	fileMenu->menu()->addAction(quitAction);
	connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
	
	setMenuBar(new QMenuBar(this));
	menuBar()->addAction(fileMenu);
}

MainWindow::~MainWindow()
{
	
}

void MainWindow::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);
	widget->resize(event->size());
}

void MainWindow::openSettings()
{
	SettingsWindow wnd(client);
	wnd.show();
	
	QEventLoop loop;
    connect(&wnd, SIGNAL(destroyed()), &loop, SLOT(quit()));
    loop.exec();
}

SettingsWidget::SettingsWidget(QMainWindow* parent, Client* c) :
	QWidget(parent)
{
	this->client = c;
	keys = std::vector<std::string>();
	oldValues = std::vector<std::string>();
	newValues = std::vector<QLineEdit*>();
	
	resize(QSize(400, 600));
	main_layout = new QGridLayout(this);
	
	this->addSettingsField("application.root_directory");
	
	QPushButton *confirmButton = new QPushButton(this);
	confirmButton->setText(Provider::lang->getValue("client.settings.confirm").c_str());
	connect(confirmButton, SIGNAL(pressed()), this, SLOT(saveSettings()));
	main_layout->addWidget(confirmButton, gridIndex, 0, Qt::AlignBottom);
	
	this->setLayout(main_layout);
}

SettingsWidget::~SettingsWidget()
{
	
}

void SettingsWidget::addSettingsField(std::string identifier)
{
	QLabel *label = new QLabel(this);
	QLineEdit *edit = new QLineEdit(this);
	
	label->setText(Provider::lang->getValue(identifier).c_str());
	
	std::stringstream s;
	s << "SETTINGS_GET~" << identifier;
	client->writeString(s.str());
	std::string resp = client->readString();
	
	edit->setText(resp.c_str());
	
	keys.push_back(identifier);
	oldValues.push_back(resp);
	newValues.push_back(edit);
	
	main_layout->addWidget(label, gridIndex, 0, Qt::AlignTop);
	main_layout->addWidget(edit, gridIndex, 1, Qt::AlignTop);
	
	gridIndex++;
}

void SettingsWidget::saveSettings()
{
	for(size_t i = 0;i < keys.size(); ++i) {
		std::string old = oldValues[i];
		std::string nw = newValues[i]->text().toStdString();
		
		if (old != nw) {
			std::stringstream s;
			s << "SETTINGS_SET~" << keys[i] << "~" << nw;
			client->writeString(s.str());
		}
	}
}

SettingsWindow::SettingsWindow(Client* c) :
	QMainWindow()
{
	this->client = c;
	this->setWindowTitle("MediaCloud - Settings");
	
	widget = new SettingsWidget(this, c);
	resize(widget->size());
}

SettingsWindow::~SettingsWindow()
{
	
}