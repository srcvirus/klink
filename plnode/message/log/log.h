/*
 * log.h
 *
 *  Created on: 2012-11-26
 *      Author: sr2chowd
 */

#ifndef LOG_H_
#define LOG_H_

#include <string>
#include <stdarg.h>
#include <stdio.h>
#include "../../../communication/error_code.h"
#include <unistd.h>

using namespace std;

class Log
{
	string log_type;
	string host_name;

	string log_file_name;
	string seq_file_name;

	int log_sequence_no;
	FILE* log_file_ptr;


public:

	Log()
	{
		log_type = "";
		char hostname[100];
		gethostname(hostname, 100);
		host_name = hostname;

		log_file_name = "";
		seq_file_name = "";

		log_sequence_no = -1;
		log_file_ptr = NULL;
	}

	Log(const char* seq_file, const char* type, const char* hostname = NULL)
	{
		seq_file_name = seq_file;

		FILE* seq_file_ptr = fopen(seq_file_name.c_str(), "r+");
		fscanf(seq_file_ptr, "%d", &log_sequence_no);
		log_sequence_no++;
		fprintf(seq_file_ptr, "%d", log_sequence_no);
		fclose(seq_file_ptr);

		log_type = type;
		setLogFileName();

		if(hostname != NULL) host_name = hostname;
		else
		{
			char h_name[100];
			gethostname(h_name, 100);
			host_name = h_name;
		}
	}

	void setLogType(const string& type){ log_type = type; }
	string getLogType(){ return log_type; }

	void setHostName(const string& hostname){ host_name = hostname; }
	string getHostName(){ return host_name; }

	void setLogFileName(const char* name = NULL)
	{
		if(name != NULL) log_file_name = name;
		else
		{
			char i_str[10];
			sprintf(i_str, "%d", log_sequence_no);
			log_file_name = i_str + "_" + host_name + "_" + log_type + ".txt";
		}
	}
	string getLogFileName(){ return log_file_name; }

	void setSeqFileName(const string& seq){ seq_file_name = seq; }
	string getSeqFileName(){ return seq_file_name; }

	int open(const char* mode = "r");
	int write(char* key, int argc, char* format, ...);
	void close();

	~Log();
};



int Log::open(const char* mode = "r")
{
	if(log_file_ptr == NULL)
		return 0;

	setLogFileName();
	log_file_ptr = fopen(log_file_name.c_str(), mode);

	if(log_file_ptr == NULL)
		return ERROR_OPEN_FILE_FAIL;

	return 1;
}

int Log::write(char* key, int argc, char* format, ...)
{
	va_list values;
	va_start(values, format);

	int i;
	for(i = 0; i < argc; i++)
	{
		switch(format[i])
		{
		case 'i':
			break;
		case 'f':
			break;
		case 'd':
			break;
		case 'c':
			break;
		case 's':
			break;
		}
	}
}

void Log::close()
{
	if(log_file_ptr != NULL)
		fclose(log_file_ptr);
}

Log::~Log()
{
	if(log_file_ptr != NULL)
		fclose(log_file_ptr);
}
#endif /* LOG_H_ */
