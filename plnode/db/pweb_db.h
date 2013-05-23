#ifndef __PWEB_DB_H__
#define __PWEB_DB_H__

#include <iostream>
#include <cstdlib>
#include <sstream>
#include "database.h"
#include "../ds/host_address.h"

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


void searchDeviceName(string name, HostAddress &ha){
	string devicename, username;
	int dot_index = name.find_last_of('.');
	devicename = name.substr(0, dot_index);
	username = name.substr(dot_index+1, string::npos);

	open_db();
	bool is_error = false;
	string error_message = "";
	vector<vector<string> > result = db->query("SELECT * FROM device WHERE username='" + username + "' AND devicename='" + devicename + "'", is_error, error_message);
	close_db();

	if(is_error || result.size() == 0){
		ha = HostAddress();
	}
	else{
		ha = HostAddress(result.at(0).at(3), atoi(result.at(0).at(4).c_str()));	
	}
}

bool isUserNameAvailable(string username){
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
			return false;
		}
	}
	else
	{
		return false;
	}
	return true;
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
			string suggestions = "";
			for(int i=0;i<5;i++){
				ostringstream new_username;
				int rnd = rand() %100;
				new_username << username;
				new_username << rnd;
				string uname(new_username.str());
				if(isUserNameAvailable(uname)){
					suggestions.append("\"" + uname + "\",");
				}
				else{
					i--;
				}
			}
			//string suggestions(suggestion_stream.str());
			if(suggestions != ""){
				suggestions = suggestions.substr(0, suggestions.size()-1);
			}
			return "{\"name\":\"" + username + "\", \"availability\":\"no\", \"suggestion\":[" + suggestions + "]}";
		}
		else
		{
			return "{\"name\":\"" + username + "\", \"availability\":\"yes\"}";
		}
	}
	else
	{
		return "{\"status\":\"failure\", \"error\":\"" + error_message + "\"}";
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

string registerDevice(string username, string devicename, string type, string ip, string port, string public_folder, string private_folder, string os, string description, bool searchable)
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

	db->query("INSERT INTO device (username, devicename, type, ip, port, last_seen, public_folder, private_folder, os, description, searchable) VALUES('" + username + "', '" + devicename + "', '" + type + "', '" + ip + "', '" + port + "', " + timestamp_str + ",'" + public_folder + "','" + private_folder + "','" + os + "','" + description + "', " + searchable_str + ")", is_error, error_message);
	if(is_error){
		return "{\"status\":\"failure\", \"error\":\"" + error_message + "\"}";
	}
	close_db();

	return "{\"status\":\"success\"}";
}

bool isDeviceNameAvailable(string username, string devicename)
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
			return false;
		}
	}
	else
	{
		return false;
	}
	return true;
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
			string suggestions = "";
			for(int i=0;i<5;i++){
				ostringstream new_devicename;
				int rnd = rand() %100;
				new_devicename << devicename;
				new_devicename << rnd;
				string dname(new_devicename.str());
				if(isDeviceNameAvailable(dname, username)){
					suggestions.append("\"" + dname + "\",");
				}
				else{
					i--;
				}
			}
			//string suggestions(suggestion_stream.str());
			if(suggestions != ""){
				suggestions = suggestions.substr(0, suggestions.size()-1);
			}
			return "{\"name\":\"" + devicename + "." + username + "\", \"availability\":\"no\", \"suggestion\":[" + suggestions + "]}";
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

string modifyDevice(string username, string old_devicename, string new_devicename, string port, string public_folder, string private_folder)
{
	open_db();
	bool is_error = false;
	string error_message = "";
	vector<vector<string> > result = db->query("UPDATE device SET devicename='" + new_devicename + "', port='" + port + "', public_folder='" + public_folder + "', private_folder='" + private_folder + "' WHERE username='" + username + "' and devicename='" + old_devicename + "'", is_error, error_message);
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
		data.append("{\"name\":\"" + row.at(1) + "\", \"type\":\"" + row.at(2) + "\", \"ip\":\"" + row.at(3) + "\", \"port\":\"" + row.at(4) + "\", \"public_folder\":\"" + row.at(5) + "\", \"private_folder\":\"" + row.at(6) + "\", \"os\":\"" + row.at(8) + "\", \"description\":\"" + row.at(9) + "\"},");
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

string getall(string timestamp)
{
	open_db();
	bool is_error = false;
	string error_message = "";
	vector<vector<string> > result = db->query("SELECT * FROM user, device WHERE device.searchable=1 AND user.username=device.username AND device.last_seen >= " + timestamp, is_error, error_message);
	close_db();

	if(is_error){
		return "{\"error\":\"" + error_message + "\"}";
	}
	
	string data = "<devices>";
	bool data_appended = false;
	for(vector<vector<string> >::iterator it = result.begin(); it < result.end(); ++it)
	{	
		vector<string> row = *it;
		data.append("<device><owner>"+row.at(3)+"</owner><name>"+row.at(7)+"</name><timestamp>"+row.at(13)+"</timestamp><localtion>"+row.at(4)+"</location><description>"+row.at(15)+"</description></device>");
	}
	data.append("</devices>");
	return data;
}


#endif