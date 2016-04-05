#include "MediaCloudDatabase.h"

using namespace MediaCloud;

Database::Database()
{
	max = pow10(saltLength) - 1;
	min = max / 9;
	
	hashProvider = boost::uuids::detail::sha1();
	sqlite3_open(this->filename.c_str(), &this->db);
	
	this->query("CREATE TABLE IF NOT EXISTS tbl_users (ID INTEGER PRIMARY KEY AUTOINCREMENT, username TEXT, password TEXT, salt TEXT)");
	this->query("CREATE TABLE IF NOT EXISTS tbl_files (ID INTEGER PRIMARY KEY AUTOINCREMENT, path TEXT, type TEXT)");
	this->query("CREATE TABLE IF NOT EXISTS tbl_settings (ID INTEGER PRIMARY KEY AUTOINCREMENT, key TEXT, value TEXT)");
}

Database::~Database()
{
	sqlite3_close(this->db);
}

std::string Database::calculateSHA1(char *data, size_t length)
{
	hashProvider.reset();
	char hash[20];
	hashProvider.process_bytes(data, length);
	
	unsigned int digest[5];
	hashProvider.get_digest(digest);
	
	for(int i = 0; i < 5; ++i)
	{
		const char* tmp = reinterpret_cast<char*>(digest);
		hash[4 * i + 0] = tmp[4 * i + 3];
		hash[4 * i + 1] = tmp[4 * i + 2];
		hash[4 * i + 2] = tmp[4 * i + 1];
		hash[4 * i + 3] = tmp[4 * i + 0];
	}
	
	std::stringstream stream = std::stringstream();
	
	for(int i = 0; i < 20; ++i)
    {
        stream << std::hex << ((hash[i] & 0x000000F0) >> 4) <<  (hash[i] & 0x0000000F);
    }
    
    return stream.str();
}

bool Database::registerUser(std::string username, std::string password)
{
	//TODO: check if username is available
	
	boost::random::uniform_int_distribution<> usalt(min, max);
	unsigned long salt = usalt(randomProvider);
	
	std::string saltHash = this->calculateSHA1((char*) &salt, sizeof(unsigned long));
	std::string pHash = this->calculateSHA1((char*) password.c_str(), (size_t) password.length());
	std::string p = pHash + saltHash;
	std::string passwordHash = this->calculateSHA1((char*) p.c_str(), (size_t) p.length());
	std::stringstream query = std::stringstream();
	query << "INSERT INTO tbl_users (username, password, salt) VALUES ('";
	query << username << "', '";
	query << passwordHash << "', '";
	query << saltHash << "')";
	
	int r = sqlite3_exec(this->db, query.str().c_str(), 0, 0, 0);
	if (r != 0) {
		std::cerr << r << ": " << sqlite3_errmsg(this->db) << std::endl;
		return false;
	}
	
	return true;
}

bool Database::login(std::string username, std::string password)
{
	//TODO: implement
	return false;
}

Result* Database::query(std::string query)
{
	Result* res = new Result();
	
	sqlite3_stmt* statement;
	int r = sqlite3_prepare(db, query.c_str(), -1, &statement, 0);
	if (r != 0)
		return 0;

	int c = sqlite3_column_count(statement);
	res->columns = c;
	
	res->data = std::vector<ResultRow>();
	while (true) {
        int s = sqlite3_step (statement);
        if (s == SQLITE_ROW) {
			ResultRow row = ResultRow();
			for(int i = 0;i < res->columns; ++i) {
				const unsigned char* text = sqlite3_column_text (statement, i);
				
				row.columns.push_back(std::string((char*) text));
			}
			res->data.push_back(row);
        }
        else if (s == SQLITE_DONE)
            break;
    }
    
    sqlite3_finalize(statement);
	
	res->rows = res->data.size();
	return res;
}