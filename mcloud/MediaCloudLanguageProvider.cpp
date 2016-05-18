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
	languages = std::vector<LanguagePair>();
	dataPairs = std::vector<LanguageDataPair>();
	registerAllLanguages();
}

LanguageProvider::~LanguageProvider()
{
	
}

void LanguageProvider::loadLanguage(std::string name)
{
	std::stringstream path;
	std::string identifier = getIdentifier(name);
	if (!checkID(identifier))
		identifier = "en_US";
	path << "data/resources/lang/" << identifier << ".lang";
	data = Utils::deleteAll(Utils::loadFile(path.str()), '\r');
	std::vector<std::string> lines = Utils::explode(data, '\n');
	
	for(size_t i = 0;i < lines.size(); ++i)
	{
		if (lines[i].substr(0, 1) == "#")
			continue;
		
		std::vector<std::string> elements = Utils::explode(lines[i], '=');
		if (elements.size() != 2)
			continue;
		
		LanguageDataPair l = LanguageDataPair();
		l.key = elements[0];
		l.value = elements[1];
		
		dataPairs.push_back(l);
	}
}

void LanguageProvider::loadByIdentifier(std::string identifier)
{
	std::stringstream path;
	if (!checkID(identifier))
		identifier = "en_US";
	path << "data/resources/lang/" << identifier << ".lang";
	data = Utils::deleteAll(Utils::loadFile(path.str()), '\r');
	std::vector<std::string> lines = Utils::explode(data, '\n');
	
	for(size_t i = 0;i < lines.size(); ++i)
	{
		if (lines[i].substr(0, 1) == "#")
			continue;
		
		std::vector<std::string> elements = Utils::explode(lines[i], '=');
		if (elements.size() != 2)
			continue;
		
		LanguageDataPair l = LanguageDataPair();
		l.key = elements[0];
		l.value = elements[1];
		
		dataPairs.push_back(l);
	}
}

std::string LanguageProvider::getValue(std::string key)
{
	if (dataPairs.size() == 0)
		return "null";
	
	for(size_t i = 0;i < dataPairs.size(); ++i)
	{
		if (dataPairs[i].key == key)
			return dataPairs[i].value;
	}
	
	return key;
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

bool LanguageProvider::checkID(std::string ID)
{
	std::stringstream path;
	path << "data/resources/lang/" << ID << ".lang";
	return (boost::filesystem::exists(path.str()));
}
