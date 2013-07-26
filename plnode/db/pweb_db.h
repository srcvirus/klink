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

void ReplaceStringInPlace(std::string& subject, const std::string& search,
                          const std::string& replace) {
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
         subject.replace(pos, search.length(), replace);
         pos += replace.length();
    }
}

void escape_single_quote(string& value){
	ReplaceStringInPlace(value, "'","''");
}

void searchDeviceName(string name, HostAddress &ha){

	escape_single_quote(name);

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

	escape_single_quote(username);

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

	escape_single_quote(username);

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

	escape_single_quote(username);
	escape_single_quote(password);

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
	escape_single_quote(username);
	escape_single_quote(password);
	escape_single_quote(email);
	escape_single_quote(fullname);
	escape_single_quote(location);
	escape_single_quote(affiliation);

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
	escape_single_quote(username);
	escape_single_quote(devicename);
	escape_single_quote(type);
	escape_single_quote(ip);
	escape_single_quote(port);
	escape_single_quote(public_folder);
	escape_single_quote(private_folder);
	escape_single_quote(os);
	escape_single_quote(description);

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
	escape_single_quote(username);
	escape_single_quote(devicename);

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
	escape_single_quote(username);
	escape_single_quote(devicename);

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
	escape_single_quote(username);
	escape_single_quote(devicename);

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
	escape_single_quote(username);
	escape_single_quote(devicename);
	escape_single_quote(ip);
	escape_single_quote(port);

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

string updateMeta(string username, string devicename, string data)
{
	escape_single_quote(username);
	escape_single_quote(devicename);
	escape_single_quote(data);

	open_db();
	bool is_error = false;
	string error_message = "";

	time_t timestamp = time(NULL);
	stringstream ss;
	ss << timestamp;
	string timestamp_str = ss.str();

	vector<vector<string> > result = db->query("UPDATE device SET content_meta='" + data + "', content_timestamp=" + timestamp_str + " WHERE username='" + username + "' and devicename='" + devicename + "'", is_error, error_message);
	close_db();

	if(is_error){
		return "{\"status\":\"failure\", \"error\":\"" + error_message + "\"}";
	}
	return "{\"status\":\"success\"}";
}

string modifyDevice(string username, string old_devicename, string new_devicename, string public_folder, string private_folder, string os, string description, bool searchable)
{
	escape_single_quote(username);
	escape_single_quote(old_devicename);
	escape_single_quote(new_devicename);
	escape_single_quote(public_folder);
	escape_single_quote(private_folder);
	escape_single_quote(os);
	escape_single_quote(description);

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

string modifyDevice(string username, string old_devicename, string new_devicename, string ip, string port, string public_folder, string private_folder)
{
	escape_single_quote(username);
	escape_single_quote(old_devicename);
	escape_single_quote(new_devicename);
	escape_single_quote(ip);
	escape_single_quote(port);
	escape_single_quote(public_folder);
	escape_single_quote(private_folder);

	open_db();
	bool is_error = false;
	string error_message = "";
	vector<vector<string> > result = db->query("UPDATE device SET devicename='" + new_devicename + "', ip= '" + ip + "', port='" + port + "', public_folder='" + public_folder + "', private_folder='" + private_folder + "' WHERE username='" + username + "' and devicename='" + old_devicename + "'", is_error, error_message);
	close_db();

	if(is_error){
		return "{\"status\":\"failure\", \"error\":\"" + error_message + "\"}";
	}
	return "{\"status\":\"success\"}";
}

string getDeviceList(string username)
{
	escape_single_quote(username);

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
		data.append("{\"name\":\"" + row.at(1) + "\", \"type\":\"" + row.at(2) + "\", \"ip\":\"" + row.at(3) + "\", \"port\":\"" + row.at(4) + "\", \"dir_ip\":\"" + row.at(5) + "\", \"dir_port\":\"" + row.at(6) + "\", \"public_folder\":\"" + row.at(7) + "\", \"private_folder\":\"" + row.at(8) + "\", \"os\":\"" + row.at(10) + "\", \"description\":\"" + row.at(11) + "\"},");
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
	escape_single_quote(timestamp);

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
		data.append("<device><owner>"+row.at(3)+"</owner><name>"+row.at(7)+"."+row.at(0)+"</name><port>"+row.at(10)+"</port><timestamp>"+row.at(15)+"</timestamp><location>"+row.at(4)+"</location><description>"+row.at(17)+"</description><content_meta>"+row.at(19)+"</content_meta></device>");
	}
	data.append("</devices>");

	open_db();
	//get content data
	is_error = false;
	error_message = "";
	vector<vector<string> > result2 = db->query("SELECT user.username, device.devicename FROM user, device WHERE device.searchable=1 AND user.username=device.username AND device.content_timestamp >= " + timestamp, is_error, error_message);
	close_db();

	if(is_error){
		return "{\"error\":\"" + error_message + "\"}";
	}	

	data.append("<content updates>");
	data_appended = false;
	for(vector<vector<string> >::iterator it = result2.begin(); it < result2.end(); ++it)
	{	
		vector<string> row = *it;
		data.append(row.at(1) + "." + row.at(0) + ",");
		if(!data_appended) 
			data_appended = true;
	}
	if(data_appended)
	{
		data = data.substr(0, data.size()-1);
	}
	data.append("</content updates>");

	return data;
}

string getContentList(string username, string devicename)
{
	escape_single_quote(username);
	escape_single_quote(devicename);

	open_db();
	bool is_error = false;
	string error_message = "";
	vector<vector<string> > result = db->query("SELECT content_meta FROM device WHERE device.searchable=1 AND device.username='" + username + "' AND device.devicename='" + devicename + "'", is_error, error_message);
	//vector<vector<string> > result = db->query("SELECT content_meta FROM device WHERE device.username='" + username + "' AND device.devicename='" + devicename + "'", is_error, error_message);
	close_db();

	if(is_error){
		return "{\"error\":\"" + error_message + "\"}";
	}
	
	string data = "";
	if(result.size() > 0 && result.at(0).size() > 0)
		data.append(result.at(0).at(0));

	return data;
}
#endif
