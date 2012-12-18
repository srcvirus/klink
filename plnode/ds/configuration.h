/*
 * configuration.h
 *
 *  Created on: 2012-12-18
 *      Author: sr2chowd
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <string>
#include <cstring>

using namespace std;

class Configuration
{
	string config_file_path;
	string host_file_path;
	int timeout;
	int n_retry;

public:

	Configuration(){}

	Configuration(string config_file_path)
	{
		this->config_file_path = config_file_path;
		load_data();
	}

	void load_data()
	{
		FILE* config_file_ptr = fopen(config_file_path.c_str(), "r");
		char line[200];

		while(fgets(line, sizeof(line), config_file_ptr) != NULL)
		{
			char* key = strtok(line, "=");
			char* value = strtok(line, "=");

			if(strcmp(key, "host_file") == 0 || strcmp(key,"hostfile") == 0)
			{
				host_file_path = value;
			}
			else if(strcmp(key, "timeout") == 0)
			{
				timeout = atoi(value);
			}
			else if(strcmp(key, "retry") == 0)
			{
				n_retry = atoi(value);
			}
		}
		fclose(config_file_ptr);
	}

	string getConfigFilePath() const
	{
		return this->config_file_path;
	}

	void setConfigFilePath(string& config_file_path)
	{
		this->config_file_path = config_file_path;
	}

	int getTimeout() const
	{
		return this->timeout;
	}

	int getNRetry() const
	{
		return this->n_retry;
	}
};


#endif /* CONFIGURATION_H_ */
