#ifndef HOST_ADDRESS_H
#define HOST_ADDRESS_H

#include <cstring>

class HostAddress {
private:
    string hostName;
    int hostPort;
public:

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
};
#endif
