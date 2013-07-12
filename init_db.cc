#include <iostream>
#include "plnode/db/database.h"
using namespace std;


int main()
{
	Database *db;
	db = new Database("pweb.sqlite");

	//create table user
	db->query("CREATE TABLE IF NOT EXISTS user (username VARCHAR(100), password VARCHAR(100), email VARCHAR(100), fullname VARCHAR(200), location VARCHAR(200), affiliation VARCHAR(200));");
	db->query("delete from user;");

	//create table device	
	db->query("CREATE TABLE IF NOT EXISTS device (username VARCHAR(100), devicename VARCHAR(100), type VARCHAR(100), ip VARCHAR(100), port VARCHAR(100), dir_ip VARCHAR(100), dir_port VARCHAR(100), public_folder VARCHAR(100), private_folder VARCHAR(100), last_seen INTEGER, os VARCHAR(100), description VARCHAR(500), content_meta varchar(1500), searchable INTEGER);");
	db->query("delete from device;");

	//create table user_device
	//db->query("CREATE TABLE IF NOT EXISTS user_device (username VARCHAR(100), device_id INTEGER);");
	//db->query("delete from user_device;");
	
	return 0;

	//db->query("INSERT INTO a VALUES(5, 4);");
	db->query("INSERT INTO user VALUES('faiz', 'faiz', 'faiz')");
	vector<vector<string> > result = db->query("SELECT name, password FROM user;");
	for(vector<vector<string> >::iterator it = result.begin(); it < result.end(); ++it)
	{	
		vector<string> row = *it;
		cout << "Values: (name=" << row.at(0) << ", password=" << row.at(1) << ")" << endl;
	}
	
	result = db->query("SELECT count(*) FROM user;");
	cout << "count = " << result.at(0).at(0) << endl;

	db->close();
	return 0;	
}
