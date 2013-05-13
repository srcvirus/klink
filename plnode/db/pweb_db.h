#ifndef __PWEB_DB_H__
#define __PWEB_DB_H__

#include <iostream>
#include <cstdlib>
#include <sstream>
#include "database.h"

using namespace std;

Database *db;

void open_db()
{
	db = new Database("./pweb.sqlite");	
}

void close_db()
{
	db->close();	
}

string existUsername(string username)
{
	open_db();
	bool is_error = false;
	string error_message = "";
	vector<vector<string> > result = db->query("SELECT COUNT(*) FROM user WHERE username='" + username + "'", is_error, error_message);
	close_db();

	if(!is_error)
	{
		int count = atoi(result.at(0).at(0).c_str());
		if(count > 0)
		{
			return "{\"name\":\"" + username + "\", \"availability\":\"no\"}";
		}
		else
		{
			return "{\"name\":\"" + username + "\", \"availability\":\"yes\"}";
		}
	}
	else
	{
		return "{\"error\":\"" + error_message + "\"}";
	}
}


string authenticate(string username, string password)
{
	open_db();
	bool is_error = false;
	string error_message = "";
	vector<vector<string> > result = db->query("SELECT COUNT(*) FROM user WHERE username='" + username + "' and password='" + password + "'", is_error, error_message);
	close_db();

	if(!is_error)
	{
		int count = atoi(result.at(0).at(0).c_str());
		if(count > 0)
		{
			return "{\"status\":\"success\"}";
		}
		else
		{
			return "{\"status\":\"failure\"}";
		}
	}
	else
	{
		return "{\"error\":\"" + error_message + "\"}";
	}
}


string registerUser(string username, string password, string email, string fullname, string location, string affiliation)
{
	open_db();
	bool is_error = false;
	string error_message = "";
	vector<vector<string> > result = db->query("INSERT INTO user VALUES('" + username + "', '" + password + "', '" + email + "', '" + fullname + "', '" + location + "', '" + affiliation + "')", is_error, error_message);
	close_db();

	if(!is_error)
	{
		return "{\"status\":\"success\"}";
	}
	else
	{
		return "{\"status\":\"failure\", \"error\":\"" + error_message + "\"}";
	}
}

string registerDevice(string username, string devicename, string type, string ip, string port, string os, string description, bool searchable)
{
	open_db();
	bool is_error = false;
	string error_message = "";

	//insert into device
	string searchable_str = "1";
	if(!searchable) searchable_str = "0";

	time_t timestamp = time(NULL);
	stringstream ss;
	ss << timestamp;
	string timestamp_str = ss.str();

	db->query("INSERT INTO device (username, devicename, type, ip, port, last_seen, os, description, searchable) VALUES('" + username + "', '" + devicename + "', '" + type + "', '" + ip + "', '" + port + "', " + timestamp_str + ",'" + os + "','" + description + "', " + searchable_str + ")", is_error, error_message);
	if(is_error){
		return "{\"status\":\"failure\", \"error\":\"" + error_message + "\"}";
	}
	close_db();

	return "{\"status\":\"success\"}";
}

string existDevicename(string username, string devicename)
{
	open_db();
	bool is_error = false;
	string error_message = "";
	vector<vector<string> > result = db->query("SELECT COUNT(*) FROM device WHERE username='" + username + "' and devicename='" + devicename + "'", is_error, error_message);
	close_db();

	if(!is_error)
	{
		int count = atoi(result.at(0).at(0).c_str());
		if(count > 0)
		{
			return "{\"name\":\"" + devicename + "." + username + "\", \"availability\":\"no\"}";
		}
		else
		{
			return "{\"name\":\"" + devicename + "." + username + "\", \"availability\":\"yes\"}";
		}
	}
	else
	{
		return "{\"error\":\"" + error_message + "\"}";
	}
}

string deleteDevice(string username, string devicename)
{
	open_db();
	bool is_error = false;
	string error_message = "";
	vector<vector<string> > result = db->query("DELETE FROM device WHERE username='" + username + "' and devicename='" + devicename + "'", is_error, error_message);
	close_db();

	if(is_error){
		return "{\"status\":\"failure\", \"error\":\"" + error_message + "\"}";
	}
	return "{\"status\":\"success\"}";
}

string updateDevice(string username, string devicename, string ip, string port)
{
	open_db();
	bool is_error = false;
	string error_message = "";

	time_t timestamp = time(NULL);
	stringstream ss;
	ss << timestamp;
	string timestamp_str = ss.str();

	vector<vector<string> > result = db->query("UPDATE device SET ip='" + ip + "', port='" + port + "', last_seen=" + timestamp_str + " WHERE username='" + username + "' and devicename='" + devicename + "'", is_error, error_message);
	close_db();

	if(is_error){
		return "{\"status\":\"failure\", \"error\":\"" + error_message + "\"}";
	}
	return "{\"status\":\"success\"}";
}

string modifyDevice(string username, string old_devicename, string new_devicename, string public_folder, string private_folder, string os, string description, bool searchable)
{
	open_db();
	bool is_error = false;
	string error_message = "";
	
	string searchable_str = "1";
	if(!searchable) searchable_str = "0";
	vector<vector<string> > result = db->query("UPDATE device SET devicename='" + new_devicename + "', public_folder='" + public_folder + "', private_folder='" + private_folder + "', os='" + os + "', description='" + description + "', searchable=" + searchable_str + " WHERE username='" + username + "' and devicename='" + old_devicename + "'", is_error, error_message);
	close_db();

	if(is_error){
		return "{\"status\":\"failure\", \"error\":\"" + error_message + "\"}";
	}
	return "{\"status\":\"success\"}";
}

string getDeviceList(string username)
{
	open_db();
	bool is_error = false;
	string error_message = "";
	vector<vector<string> > result = db->query("SELECT * FROM device WHERE username='" + username + "'", is_error, error_message);
	close_db();

	if(is_error){
		return "{\"error\":\"" + error_message + "\"}";
	}
	
	string data = "{\"username\":\"" + username + "\", \"devices\":[";
	bool data_appended = false;
	for(vector<vector<string> >::iterator it = result.begin(); it < result.end(); ++it)
	{	
		vector<string> row = *it;
		//cout << "Values: (name=" << row.at(0) << ", password=" << row.at(1) << ")" << endl;
		data.append("{\"name\":\"" + row.at(1) + "\", \"type\":\"" + row.at(2) + "\", \"ip\":\"" + row.at(3) + "\", \"port\":\"" + row.at(4) + "\", \"os\":\"" + row.at(8) + "\", \"description\":\"" + row.at(9) + "\"},");
		if(!data_appended) 
			data_appended = true;
	}
	if(data_appended)
	{
		data = data.substr(0, data.size()-1);
	}
	data.append("]}");
	return data;
}

#endif
