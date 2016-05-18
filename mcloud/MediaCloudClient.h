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
#include "MediaCloudLanguageProvider.h"

#include <QtGui/QMainWindow>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QLayout>
#include <QtGui/QDesktopWidget>
#include <QtGui/QMessageBox>
#include <QtGui/QListWidget>
#include <QtGui/QSizePolicy>
#include <QtGui/QResizeEvent>
#include <QtGui/QCheckBox>
#include <QtGui/QAction>
#include <QtGui/QMenuBar>

#include <QtCore/QEventLoop>

#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>

namespace MediaCloud {
	
	struct Provider {
		static LanguageProvider *lang;
	};
	
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
		
		QCheckBox *checkbox;
		
	protected:
		QGridLayout *main_layout;
		
		QLabel *label_ip;
		QLabel *label_user;
		QLabel *label_password;
		
		QPushButton *button_login;
		QPushButton *button_register;
	};
	
	class SelectorWindow : public QMainWindow
	{
		Q_OBJECT
	public:
		SelectorWindow();
		~SelectorWindow();
		
		void setupWindow(Client*);
		
	protected:
		QAction *fileMenu;
		
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
	
	class SidebarWidget : public QWidget
	{
	public:
		SidebarWidget(QWidget*);
		~SidebarWidget();
		
	protected:
		QGridLayout *main_layout;
		
		QFrame *media_selector;
		QLabel *media_music;
		QLabel *media_video;
	};
	
	class SettingsWidget : public QWidget
	{
		Q_OBJECT
	public:
		SettingsWidget(QMainWindow*, Client*);
		~SettingsWidget();
		
	protected:
		QGridLayout *main_layout;
		
	private:
		Client *client;
		
		uint8_t gridIndex = 0;
		void addSettingsField(std::string);
		
		std::vector<std::string> keys;
		std::vector<std::string> oldValues;
		std::vector<QLineEdit*> newValues;
		
	private slots:
		void saveSettings();
	};
	
	class SettingsWindow : public QMainWindow
	{
		Q_OBJECT
	public:
		SettingsWindow(Client*);
		~SettingsWindow();
		
	protected:
		SettingsWidget *widget;
		
	private:
		Client *client;
	};
	
	class MainWidget : public QWidget
	{
	public:
		MainWidget(QMainWindow*);
		~MainWidget();
		
	protected:
		QGridLayout *main_layout;
		
		QFrame *content_frame, *player_frame;
		SidebarWidget *sidebar;
		
		QGridLayout *content_layout, *player_layout;
	};
	
	class MainWindow : public QMainWindow
	{
		Q_OBJECT
	public:
		MainWindow(Client*);
		~MainWindow();
		
	protected:
		MainWidget *widget;
		void resizeEvent(QResizeEvent*);
		
		QAction *fileMenu;
		
	private:
		Client *client;
	private slots:
		void openSettings();
	};

}