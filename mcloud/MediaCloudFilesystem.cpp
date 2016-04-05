#include "MediaCloudFilesystem.h"

using namespace MediaCloud;

Filesystem::Filesystem(Database *database)
{
	db = database;
}

Filesystem::~Filesystem()
{

}

bool Filesystem::fileRegistered(std::string path)
{
	boost::replace_all(path, "\'", "&quot");
	std::stringstream query = std::stringstream();
	query << "SELECT * FROM tbl_files WHERE path='";
	query << path << "'";
	
	bool exists = false;
	
	Result* r = db->query(query.str());
	
	if (r->data[0].columns[1] == path)
		exists = true;
	
	return exists;
}

void Filesystem::registerFile(std::string path)
{
	if (this->fileRegistered(path))
		return;
	boost::replace_all(path, "\'", "&quot");
	std::stringstream query = std::stringstream();
	query << "INSERT INTO tbl_files (path) VALUES ('";
	query << path << "')";
	
	db->query(query.str());
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
