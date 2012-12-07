/* 
 * File:   message_get.h
 * Author: mfbari
 *
 * Created on November 29, 2012, 4:31 PM
 */

#ifndef MESSAGE_GET_H
#define	MESSAGE_GET_H

#include "../message.h"

#include <cstring>

class MessageGET : public ABSMessage {
    string deviceName;
public:

    MessageGET() : ABSMessage() {
        messageType = MSG_PLEXUS_GET;
    }

    char* serialize(int* serialize_length) {
        char* buffer = new char[sizeof (MessageGET)];
        memcpy(buffer, (char*) (this), sizeof (MessageGET));
        *serialize_length = sizeof (MessageGET);
        return buffer;
    }

    ABSMessage* deserialize(char* buffer, int buffer_length) {
        memcpy(this, buffer, buffer_length);
        return this;
    }

    void SetDeviceName(string &deviceName) {
        this->deviceName = deviceName;
    }

    string GetDeviceName() const {
        return deviceName;
    }

    virtual void message_print_dump() {
        printf("Device Name %s\n", deviceName.c_str());
    }
};

#endif	/* MESSAGE_GET_H */

