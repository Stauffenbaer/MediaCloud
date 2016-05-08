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

#include "MediaCloudUtils.hpp"

#include <string>
#include <vector>

#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace MediaCloud {
	
	struct LanguagePair
	{
		std::string language = "";
		std::string identifier = "";
	};
	
	struct LanguageDataPair
	{
		std::string key = "";
		std::string value = "";
	};
	
	class LanguageProvider
	{
	public:
		LanguageProvider();
		~LanguageProvider();
		
		void loadLanguage(std::string);
		std::string getValue(std::string);
		
	private:
		const std::string& languageBasePath = "data/resources/lang/";
		const std::string& languagesPath = "data/resources/languages.xml";
		
		std::string data;
		std::vector<LanguageDataPair> dataPairs;
		
		std::string getIdentifier(std::string);
		
		void registerAllLanguages();
		std::vector<LanguagePair> languages;
	};
	
}