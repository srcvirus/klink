#include <iostream>
#include "pweb_db.h"
using namespace std;

int main()
{
	//cout << registerUser("faiz6", "", "") << endl;
	//cout << existUsername("faiz6") << endl;
	//cout << authenticate("faiz", "faiz") << endl;
	//cout << registerDevice("faiz", "nexus", "device_type", "ip", "port") << endl;
	//cout << existDevicename("faiz", "nexus") << endl;
	//cout << deleteDevice("faiz", "nexus") << endl;
	//cout << getDeviceList("faiz") << endl;
	//cout << updateDevice("faiz", "nexus", "192.168.10.25", "3128") << endl;
	cout << modifyDevice("faiz", "nexus", "gs4", "public", "private") << endl;
	cout << "DONE" << endl;
	return 0;
}
