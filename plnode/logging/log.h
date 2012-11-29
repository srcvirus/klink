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
#include "../../communication/error_code.h"
#include <unistd.h>

using namespace std;

class Log
{
	string log_type;
	string host_name;
	string monitor_host_name;
	string remote_ftp_directory;

	string log_file_name;
	string mode;
	string seq_file_name;
	string archive_name;

	int log_sequence_no;
	int current_segment_no;
	int current_row_count;
	int check_point_row_count;
	FILE* log_file_ptr;


public:

	Log();
	Log(const char* seq_file, const char* type, const char* monitor,  const char* hostname = NULL);

	void setLogType(const string& type){ log_type = type; }
	string getLogType(){ return log_type; }

	void setHostName(const string& hostname){ host_name = hostname; }
	string getHostName(){ return host_name; }

	void setMonitorHostName(const string& hostname){ monitor_host_name = hostname;}
	string getMonitorHostName(){ return monitor_host_name; }

	void setLogFileName(const char* name);
	string getLogFileName(){ return log_file_name; }
	
	string getArchiveName(){ return archive_name; }
	void setArchiveName(const char* name);

	void setSeqFileName(const string& seq){ seq_file_name = seq; }
	string getSeqFileName(){ return seq_file_name; }

	void setCheckPointRowCount(int row_count = 100){ check_point_row_count = row_count; }
	int getCheckPointRowCount(){ return check_point_row_count; }

	void setRemoteFtpDirectory(const char* name){ remote_ftp_directory = name; }
	string getRemoteFtpDirectory(){ return remote_ftp_directory; }

	int getCurrentRowCount() const { return current_row_count; }

	int open(const char* mode = "w");
	int write(const char* key, const char* format, ...);
	void ftpUploadLog();
	void ftpUploadArchive();
	void archiveCurrentLog();
	void close();

	~Log();
};

void Log::setLogFileName(const char* name = NULL)
{
	if(name != NULL) log_file_name = name;
	else
	{
		char i_str[10];
		sprintf(i_str, "%d", log_sequence_no);
		log_file_name = i_str;
		log_file_name += "_";
		log_file_name += host_name;
		log_file_name += "_";
		log_file_name += log_type;
		log_file_name += ".txt";
	}
}

void Log::setArchiveName(const char* name = NULL)
{
	if( name != NULL ) archive_name = name;
	else
	{
		char i_str[10];
		sprintf(i_str, "%d", log_sequence_no);
		archive_name = i_str;
		archive_name += "_";
		archive_name += host_name;
		archive_name += "_";
		archive_name += log_type;
		archive_name += ".tar";
	}
}

Log::Log()
{
	log_type = "";
	mode = "";
	char hostname[100];
	gethostname(hostname, 100);
	host_name = hostname;
	monitor_host_name = "";

	log_file_name = "";
	seq_file_name = "";
	archive_name = "";

	log_sequence_no = -1;
	current_segment_no = -1;
	current_row_count = 0;
	check_point_row_count = -1;
	log_file_ptr = NULL;
}

Log::Log(const char* seq_file, const char* type, const char* monitor, const char* hostname)
{
	seq_file_name = seq_file;

	FILE* seq_file_ptr = fopen(seq_file_name.c_str(), "r+");

	fscanf(seq_file_ptr, "%d", &log_sequence_no);
	log_sequence_no++;

	fseek(seq_file_ptr, 0, SEEK_SET);
	fprintf(seq_file_ptr, "%d", log_sequence_no);

	fclose(seq_file_ptr);

	log_type = type;

	if(hostname != NULL) host_name = hostname;
	else
	{
		char h_name[100];
		gethostname(h_name, 100);
		host_name = h_name;
	}

	this->setLogFileName(NULL);
	this->setArchiveName(NULL);

	mode = "w";
	monitor_host_name = monitor;
	check_point_row_count = 100;
	current_row_count = 0;
	current_segment_no = 1;
	log_file_ptr = NULL;
}


int Log::open(const char* mode)
{
	this->mode = mode;
	if(log_file_ptr != NULL)
		return 0;

	setLogFileName();
	log_file_ptr = fopen(log_file_name.c_str(), mode);

	if(log_file_ptr == NULL)
		return ERROR_OPEN_FILE_FAIL;

	current_row_count = 0;
	return 1;
}

int Log::write(const char* key, const char* format, ...)
{
	if(log_file_ptr == NULL)
		return ERROR_FILE_NOT_OPEN;

	int argc = strlen(format);
	char str[20];
	void* arg = NULL;

	string text = key;

	va_list values;
	va_start(values, format);

	int i;
	for(i = 0; i < argc; i++)
	{
		text += " ";
		switch(format[i])
		{
		case 'i':
			arg = new int;
			*((int*)arg) = va_arg(values, int);
			sprintf(str, "%d", *((int*)arg));
			text += str;
			delete ((int*)arg);
			break;
		case 'd':
			arg = new double;
			*((double*)arg) = va_arg(values, double);
			sprintf(str, "%lf", *((double*)arg));
			text += str;
			delete ((double*)arg);
			break;
		case 's':
			arg = new char*;
			*((char**)arg) = va_arg(values, char*);
			sprintf(str, "%s", *((char**)arg));
			text += str;
			delete ((char**)arg);
			break;
		}
	}
	va_end(values);

	int ret = fprintf(log_file_ptr,"%s\n",text.c_str());
//	printf("Bytes written: %d\n", ret);
//	printf("Line written %s\n", text.c_str());
//	printf("Log file name %s\n", log_file_name.c_str());

//	fflush(log_file_ptr);

	current_row_count++;
//	printf("%d %d\n", current_row_count, check_point_row_count);
	if(current_row_count == check_point_row_count)
	{
		fflush(log_file_ptr);
//		printf("%d\n", current_row_count);
//		printf("%d\n", current_segment_no);
		fclose(log_file_ptr);
		log_file_ptr = NULL;

		this->ftpUploadLog();
		this->archiveCurrentLog();
		this->open("w+");
	}

	return ret;
}

void Log::archiveCurrentLog()
{
	FILE* shell_pipe = NULL;
	string command = "";
	string new_name = log_file_name;

	char i_str[10];
	sprintf(i_str, "%d", current_segment_no);

	new_name += "_";
	new_name += i_str;

//	rename the log file
	command = "mv ";
	command += log_file_name;
	command += " ";
	command += new_name;
//	puts(command.c_str());
	shell_pipe = popen(command.c_str(), "w");
	pclose(shell_pipe);

//	if the tar file doesn't exist then create it
	if(current_segment_no <= 1)
	{
		command = "tar -cf ";
		command += archive_name;
		command += " ";
        command += new_name;
	}
//	otherwise add the current log file in the tarball
	else
	{
		command = "tar -rf ";
		command += archive_name;
		command += " ";
		command += new_name;
	}
	puts(command.c_str());
	shell_pipe = popen(command.c_str(), "w");
	pclose(shell_pipe);

	command = "rm ";
	command += new_name;
	shell_pipe = popen(command.c_str(), "w");
	pclose(shell_pipe);
	current_segment_no++;
}
void Log::ftpUploadLog()
{
//	run the ftp command
	string command = "ftp ";
	command += monitor_host_name.c_str();
//	puts("FTP Command 1");
//	puts(command.c_str());
	FILE* shell_pipe = popen(command.c_str(), "w");

//	upload the current log file to the remote ftp directory in append mode
	command = "append ";
	command += log_file_name;
	command += " ";
	command += remote_ftp_directory;
	command += "/";
	command += log_file_name;
//	puts("FTP Command 2");
//	puts(command.c_str());
	fputs(command.c_str(), shell_pipe);
	pclose(shell_pipe);
}

void Log::ftpUploadArchive()
{
//	run the ftp command
	string command = "ftp ";
	command += monitor_host_name.c_str();
//	puts("FTP Command 1");
//	puts(command.c_str());
	FILE* shell_pipe = popen(command.c_str(), "w");

//	upload the current log file to the remote ftp directory in append mode
	command = "append ";
	command += archive_name;
	command += " ";
	command += remote_ftp_directory;
	command += "/";
	command += archive_name;
//	puts("FTP Command 2");
//	puts(command.c_str());
	fputs(command.c_str(), shell_pipe);
	pclose(shell_pipe);
}

void Log::close()
{
	if(log_file_ptr != NULL)
	{
		fclose(log_file_ptr);
		log_file_ptr = NULL;
	}

	if(current_row_count > 0)
	{
		this->ftpUploadLog();
		this->archiveCurrentLog();
		current_row_count = 0;
	}
	current_segment_no++;
}

Log::~Log()
{
	if(log_file_ptr != NULL)
	{
		fclose(log_file_ptr);
		log_file_ptr = NULL;
	}

	if(current_row_count > 0)
	{
		this->ftpUploadLog();
		this->archiveCurrentLog();
		current_row_count = 0;
	}
}
#endif /* LOG_H_ */
