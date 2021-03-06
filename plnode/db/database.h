#ifndef __DATABASE_H__
#define __DATABASE_H__

#include <string>
#include <vector>
#include <sqlite3.h>

using namespace std;

class Database
{
public:
	Database(string filename);
	~Database();
	
	bool open(string filename);
	vector<vector<string> > query(string query);
	vector<vector<string> > query(string query, bool &is_error, string &error_message);
	void close();
	
private:
	sqlite3 *database;
};

#endif


