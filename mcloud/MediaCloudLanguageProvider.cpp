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

#include "MediaCloudLanguageProvider.h"

using namespace MediaCloud;

LanguageProvider::LanguageProvider()
{
	data = "";
	dataPairs = std::vector<LanguageDataPair>();
}

LanguageProvider::~LanguageProvider()
{
	
}

void LanguageProvider::loadLanguage(std::string name)
{
	std::string path = languageBasePath + getIdentifier(name) + ".lang";
	data = Utils::deleteAll(Utils::loadFile(path), '\r');
	std::vector<std::string> lines = Utils::explode(data, '\n');
	
	for(size_t i = 0;i < lines.size(); ++i)
	{
		LanguageDataPair l = LanguageDataPair();
		std::vector<std::string> elements = Utils::explode(lines[i], '=');
		l.key = elements[0];
		l.value = elements[1];
		
		dataPairs.push_back(l);
	}
}

std::string LanguageProvider::getValue(std::string key)
{
	for(size_t i = 0;i < dataPairs.size(); ++i)
	{
		if (dataPairs[i].key == key)
			return dataPairs[i].value;
	}
	
	return "null";
}

std::string LanguageProvider::getIdentifier(std::string name)
{
	for(size_t i = 0;i < languages.size(); ++i)
	{
		if (languages[i].language == name)
			return languages[i].identifier;
	}
	
	return "fallback";
}

void LanguageProvider::registerAllLanguages()
{
	#define ATTR_SET ".<xmlattr>"
	
	using namespace std;
	using namespace boost;
	using namespace boost::property_tree;
	
	ptree tree;
    read_xml(languagesPath, tree);
    const ptree& formats = tree.get_child("languages", ptree());
	
    BOOST_FOREACH(const ptree::value_type& f, formats){
        const ptree& attributes = f.second.get_child("<xmlattr>", ptree());
		
		LanguagePair pair = LanguagePair();
		pair.language = attributes.get<std::string>("key");
		pair.identifier = attributes.get<std::string>("value");
		languages.push_back(pair);
	}
}