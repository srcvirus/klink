/* 
 * File:   message_put.h
 * Author: mfbari
 *
 * Created on November 29, 2012, 3:17 PM
 */

#ifndef MESSAGE_PUT_H
#define	MESSAGE_PUT_H

#include "../message.h"
#include "../../ds/host_address.h"

#include <cstring>

class MessagePUT : public ABSMessage {
    string deviceName;
    HostAddress hostAddress;
public:

    MessagePUT() : ABSMessage() {
        messageType = MSG_PLEXUS_PUT;
    }

    char* serialize(int* serialize_length) {
        char* buffer = new char[sizeof (MessagePUT)];
        memcpy(buffer, (char*) (this), sizeof (MessagePUT));
        *serialize_length = sizeof (MessagePUT);
        return buffer;
    }

    ABSMessage* deserialize(char* buffer, int buffer_length) {
        memcpy(this, buffer, buffer_length);
        return this;
    }

    void SetHostAddress(HostAddress &hostAddress) {
        this->hostAddress = hostAddress;
    }

    HostAddress GetHostAddress() const {
        return hostAddress;
    }

    void SetDeviceName(string &deviceName) {
        this->deviceName = deviceName;
    }

    string GetDeviceName() const {
        return deviceName;
    }

    virtual void message_print_dump() {
        printf("Device Name %s\n", deviceName.c_str());
        printf("Host Address %s:%d\n", hostAddress.GetHostName().c_str(), hostAddress.GetHostPort());
    }
};

#endif	/* MESSAGE_PUT_H */

