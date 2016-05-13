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

#include "MediaCloudFilesystem.h"

using namespace MediaCloud;

Filesystem::Filesystem(Database *database)
{
	db = database;
	extensions = std::vector<ExtensionPair>();
	
	registerAllExtensions();
}

Filesystem::~Filesystem()
{

}

bool Filesystem::fileRegistered(std::string path)
{
	boost::replace_all(path, "\'", "&quot");
	std::stringstream query;
	query << "SELECT * FROM tbl_files WHERE path='";
	query << path << "'";
	
	bool exists = false;
	
	Result* r = db->query(query.str());
	
	if (r->rows == 0)
		return false;
	
	if (r->data[0].columns[1] == path)
		exists = true;
	
	return exists;
}

void Filesystem::registerFile(std::string path)
{
	if (boost::filesystem::path(path).filename().c_str()[0] == '.')
		return;
	
	if (this->fileRegistered(path))
		return;
	
	std::string type = this->getType(path);
	boost::replace_all(path, "\'", "&quot");
	std::stringstream query;
	query << "INSERT INTO tbl_files (path, type) VALUES ('";
	query << path << "', '";
	query << type << "')";
	db->query(query.str());
	
	if (type == "audio") {
		
		TagData *data = this->getMetaData(path);
		
		
		query.str(std::string());
		query << "SELECT ID FROM tbl_files WHERE path='";
		query << path << "'";
		
		Result* res = db->query(query.str());
		if (res != 0) {
			int fID = atoi(res->data[0].columns[0].c_str());
			
			query.str(std::string());
			query << "INSERT INTO tbl_music (file, title, artist, album, tracknr, duration) VALUES ('";
			query << fID << "', '";
			query << data->title << "', '";
			query << data->artist << "', '";
			query << data->album << "', '";
			query << data->tracknumber << "', '";
			query << data->duration << "')";
			
			db->query(query.str());
			query.str(std::string());
			
			query << "SELECT ID FROM tbl_music WHERE file='" << fID << "'";
			Result *res = db->query(query.str());
			if (res->rows != 0) {
				int tID = atoi(res->data[0].columns[0].c_str());
				
				query.str(std::string());
				query << "INSERT INTO tbl_covers (trackid, cover) VALUES ('" << tID << "', ' ')";
				db->query(query.str());
			}
		}
	}
}

void Filesystem::registerFiles(std::string path)
{
	boost::filesystem::path dir (path);
    boost::filesystem::directory_iterator end_it;

    for (boost::filesystem::directory_iterator it(dir); it != end_it; ++it)
    {
		boost::filesystem::path p = it->path();
		if (boost::filesystem::is_regular_file(p))
			this->registerFile(p.string());
    }
}

void Filesystem::registerDirectory(std::string directory)
{
	struct rd
	{
		rd(boost::filesystem::path dir) : _path(dir) {
			
		}

		boost::filesystem::recursive_directory_iterator begin() {
			return boost::filesystem::recursive_directory_iterator(_path); 
			
		}
		boost::filesystem::recursive_directory_iterator end() {
			return boost::filesystem::recursive_directory_iterator();
		}

		boost::filesystem::path _path;
	};

	for (auto it : rd(directory))
	{
		if (boost::filesystem::is_regular_file(it))
			this->registerFile(std::string(it.path().c_str()));
	}
}

std::string Filesystem::getType(std::string file)
{
	std::string ext = boost::filesystem::path(file).extension().string();
	for(size_t i = 0;i < extensions.size(); ++i) {
		if ("." + extensions[i].extension == ext)
			return extensions[i].type;
	}
	return "none";
}

void Filesystem::registerAllExtensions()
{
	#define ATTR_SET ".<xmlattr>"
	
	using namespace std;
	using namespace boost;
	using namespace boost::property_tree;
	
	ptree tree;
    read_xml(extensionPath, tree);
    const ptree& formats = tree.get_child("extensions", ptree());
	
    BOOST_FOREACH(const ptree::value_type& f, formats){
        const ptree& attributes = f.second.get_child("<xmlattr>", ptree());
		
		ExtensionPair pair = ExtensionPair();
		pair.extension = attributes.get<std::string>("key");
		pair.type = attributes.get<std::string>("value");
		extensions.push_back(pair);
	}
}

File* Filesystem::getFile(int ID)
{
	std::stringstream query;
	query << "SELECT * FROM tbl_files WHERE ID='";
	query << ID << "'";
	
	Result *res = db->query(query.str());
	if (res->rows == 0)
		return 0;
	File *f = new File();
	f->ID = ID;
	f->path = res->data[0].columns[1];
	f->type = res->data[0].columns[2];
	
	boost::replace_all(f->path, "&quot", "\'");
	return f;
}

std::vector<File*> Filesystem::getFiles()
{
	std::vector<File*> files = std::vector<File*>();
	Result *res = db->query("SELECT * FROM tbl_files");
	for(int i = 0;i < res->rows; ++i) {
		File* f = new File();
		f->ID = i;
		f->path = res->data[i].columns[1];
		f->type = res->data[i].columns[2];
		
		boost::replace_all(f->path, "&quot", "\'");
		files.push_back(f);
	}
	
	return files;
}

Filesystem::TagData* Filesystem::getMetaData(std::string path)
{
	boost::replace_all(path, "&quot", "\'");
	AVFormatContext *container = avformat_alloc_context();
	if (avformat_open_input(&container, path.c_str(), 0, 0)) {
		return 0;
	}
	
	AVDictionary *metadata = container->metadata;
	AVDictionaryEntry *title = av_dict_get(metadata, "title", 0, 0);
	AVDictionaryEntry *artist = av_dict_get(metadata, "artist", 0, 0);
	AVDictionaryEntry *album = av_dict_get(metadata, "album", 0, 0);
	AVDictionaryEntry *tracknr = av_dict_get(metadata, "track", 0, 0);
	
	avformat_find_stream_info(container, 0);
	int64_t duration = floor(container->duration / (1000 * 1000)) + 1;
	
	TagData *data = new TagData();
	
	if (title)
		data->title = std::string(title->value);
	else
		data->title = "";
	
	if (artist)
		data->artist = std::string(artist->value);
	else
		data->artist = "";
	
	if (album)
		data->album = std::string(album->value);
	else
		data->album = "";
	
	if (tracknr)
		data->tracknumber = atoi(tracknr->value);
	else
		data->tracknumber = 0;
	
	data->duration = duration;
	
	avformat_free_context(container);
	return data;
}

std::string Filesystem::getTrackCover(std::string trackpath)
{
	std::string basedir = trackpath.substr(0, trackpath.find_last_of("/\\"));
	
	std::stringstream query;
	query << "SELECT path FROM tbl_files WHERE type='image' AND path LIKE '" << basedir << "%'";
	
	Result *res = db->query(query.str());
	if (res->rows == 0)
		return "";
	
	return res->data[0].columns[0];
}

void Filesystem::registerCovers()
{
	std::stringstream query;
	query << "SELECT trackid FROM tbl_covers WHERE cover=' '";
	
	Result *res = db->query(query.str());
	for(int i = 0;i < res->rows; ++i) {
		ResultRow row = res->data[i];
		
		int trackID = atoi(row.columns[0].c_str());
		query.str(std::string());
		query << "SELECT path FROM tbl_files WHERE ID='" << trackID << "'";
		
		Result *r = db->query(query.str());
		query.str(std::string());
		query << "UPDATE tbl_covers SET cover='" << getTrackCover(r->data[0].columns[0]) << "' WHERE trackid='" << trackID << "'";
		db->query(query.str());
		
		delete r;
	}
	
	delete res;
}
