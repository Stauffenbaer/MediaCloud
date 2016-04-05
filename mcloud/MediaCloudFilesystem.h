#pragma once

#include "MediaCloudDatabase.h"

#include <fstream>
#include <string>
#include <algorithm>

#include <boost/algorithm/string/replace.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/filesystem.hpp>

namespace MediaCloud {

	struct ExtensionPair
	{
		std::string extension = "";
		std::string type = "";
	};
	
	struct File
	{
		int ID;
		std::string path;
		std::string type;
	};
	
	class Filesystem
	{
	public:
		Filesystem(Database*);
		~Filesystem();
		
		bool fileRegistered(std::string);
		
		void registerFile(std::string);
		void registerFiles(std::string);
		void registerDirectory(std::string);
		
		File* getFile(int);
		std::vector<File*> getFiles();
		
	protected:
		Database *db;
		
	private:
		const std::string& extensionPath = "data/extensions.xml";
		
		std::string getType(std::string);
		
		void registerAllExtensions();
		std::vector<ExtensionPair> extensions;
	};

}