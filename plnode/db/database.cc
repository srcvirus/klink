#include "database.h"
#include <iostream>

Database::Database(string filename)
{
	database = NULL;
	open(filename);
}

Database::~Database()
{
}

bool Database::open(string filename)
{
	if(sqlite3_open(filename.c_str(), &database) == SQLITE_OK)
		return true;
		
	return false;   
}

vector<vector<string> > Database::query(string query)
{
	sqlite3_stmt *statement;
	vector<vector<string> > results;
	//ReplaceStringInPlace(query, "'", "''");

	if(sqlite3_prepare_v2(database, query.c_str(), -1, &statement, 0) == SQLITE_OK)
	{
		int cols = sqlite3_column_count(statement);
		int result = 0;
		while(true)
		{
			result = sqlite3_step(statement);
			
			if(result == SQLITE_ROW)
			{
				vector<string> values;
				for(int col = 0; col < cols; col++)
				{
					std::string  val;
					char * ptr = (char*)sqlite3_column_text(statement, col);
					//cout << ptr << endl;
					if(ptr)
					{
						val = ptr;
					}
					else val = ""; // this can be commented out since std::string  val;
							// initialize variable 'val' to empty string anyway
					values.push_back(val);  // now we will never push NULL
					//values.push_back((char*)sqlite3_column_text(statement, col));
				}
				results.push_back(values);
			}
			else
			{
				break;   
			}
		}
	   
		sqlite3_finalize(statement);
	}
	
	string error = sqlite3_errmsg(database);
	if(error != "not an error") cout << query << " " << error << endl;
	
	return results;  
}

vector<vector<string> > Database::query(string query, bool &is_error, string &error_message)
{
	sqlite3_stmt *statement;
	vector<vector<string> > results;
	//ReplaceStringInPlace(query, "'", "''");
	//cout << "^^^^" << query << endl;
	if(sqlite3_prepare_v2(database, query.c_str(), -1, &statement, 0) == SQLITE_OK)
	{
		int cols = sqlite3_column_count(statement);
		int result = 0;
		while(true)
		{
			result = sqlite3_step(statement);
			
			if(result == SQLITE_ROW)
			{
				vector<string> values;
				for(int col = 0; col < cols; col++)
				{
					std::string  val;
					char * ptr = (char*)sqlite3_column_text(statement, col);
					//cout << ptr << endl;
					if(ptr)
					{
						val = ptr;
					}
					else val = ""; // this can be commented out since std::string  val;
							// initialize variable 'val' to empty string anyway
					values.push_back(val);  // now we will never push NULL
					//values.push_back((char*)sqlite3_column_text(statement, col));
				}
				results.push_back(values);
			}
			else
			{
				break;   
			}
		}
	   
		sqlite3_finalize(statement);
	}
	
	string error = sqlite3_errmsg(database);
	if(error != "not an error") //cout << query << " " << error << endl;
	{
		is_error = true;
		error_message = error;
	}
	else
	{
		is_error = false;
		error_message = "";
	}
	
	return results; 	
}

void Database::close()
{
	sqlite3_close(database);   
}
