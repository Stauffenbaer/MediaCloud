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

#include "MediaCloudDatabase.h"

#include <fstream>
#include <string>
#include <algorithm>
#include <cstdio>

#include <boost/algorithm/string/replace.hpp>
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/filesystem.hpp>

extern "C" {
	#include <libavformat/avformat.h>
	#include <libavutil/avutil.h>
}

namespace MediaCloud {

	struct ExtensionPair
	{
		std::string extension = "";
		std::string type = "";
	};
	
	struct PriorityPair
	{
		std::string name = "";
		int priority = -1;
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
		
		void registerCovers();
		
		File* getFile(int);
		std::vector<File*> getFiles();
		
	protected:
		Database *db;
		
	private:
		const std::string& extensionPath = "data/resources/extensions.xml";
		
		std::string getType(std::string);
		
		std::string getTrackCover(std::string);
		
		void registerAllExtensions();
		std::vector<ExtensionPair> extensions;
		
		struct TagData {
			std::string title;
			std::string artist;
			std::string album;
			int tracknumber;
			int duration;
		};
		
		TagData* getMetaData(std::string);
	};

}