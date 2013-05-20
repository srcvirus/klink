#include <iostream>
#include "pweb_db.h"
using namespace std;

int main()
{
	//cout << registerUser("faiz", "pasw", "faiz@gmail.com", "Faizul Bari", "Waterloo, ON, Canada", "University of Waterloo") << endl;
	//cout << existUsername("faiz") << endl;
	//cout << authenticate("faiz", "pasw") << endl;
	//cout << registerDevice("faiz", "nexus", "cell phone", "192.168.20.3", "3128", "android", "my first android phone...", true) << endl;
	//cout << existDevicename("faiz", "nexu8s") << endl;
	//cout << deleteDevice("faiz", "nexus") << endl;
	//cout << getDeviceList("faiz") << endl;
	//cout << updateDevice("faiz", "nexus", "192.168.10.25", "3128") << endl;
	cout << modifyDevice("faiz", "nexus", "goggle galaxy nexus", "public", "private", "android", "my second android phone...", false) << endl;
	cout << "DONE" << endl;
	return 0;
}
