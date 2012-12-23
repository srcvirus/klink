#ifndef HOST_ADDRESS_H
#define HOST_ADDRESS_H

#include <string>
#include <cmath>

using namespace std;

class HostAddress {
private:
        string hostName;
        int hostPort;
public:

        HostAddress() {
        }

        HostAddress(string name, int port) {
                hostName = name;
                hostPort = port;
        }

        void SetHostPort(int hostPort) {
                this->hostPort = hostPort;
        }

        int GetHostPort() const {
                return hostPort;
        }

        void SetHostName(string hostName) {
                this->hostName = hostName;
        }

        string GetHostName() const {
                return hostName;
        }

        char* toString() {
                char* result = new char[getStringSize() + 1];
                sprintf(result, "%s:%d", hostName.c_str(), hostPort);
                result[getStringSize()] = '\0';
                return result;
        }

        unsigned numDigits(unsigned number) {
                int digits = 0;
                if(number == 0) return 1;
                while (number != 0) {
                        number /= 10;
                        digits++;
                }
                return digits;
        }

        int getStringSize() {
                return hostName.size() + 1 + numDigits(hostPort);
        }
};
#endif
