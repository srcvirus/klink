/*
 * ip_address.h
 *
 *  Created on: 2012-11-22
 *      Author: sr2chowd
 */

#ifndef IP_ADDRESS_H_
#define IP_ADDRESS_H_

#include <string>

using namespace std;

class IPAddress
{
	int ip_address;
	string str_ip_address;
public:
	IPAddress();
	IPAddress(int a, int b, int c, int d);
	IPAddress(int* a);
	IPAddress(const vector <int>& a);
	IPAddress(char* ip);
	IPAddress(const string& ip);

	void setIp(int a, int b, int c, int d);
	void setIp(int* a);
	void setIp(const vector <int>& a);
	void setIp(char* ip);
	void setIp(const string& ip);

	string getStrIpAddress(){ return str_ip_address; }
	int getIpAddress(){ return ip_address; }
};


#endif /* IP_ADDRESS_H_ */
