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

    void SetDeviceName(string deviceName) {
        this->deviceName = deviceName;
    }

    string GetDeviceName() const {
        return deviceName;
    }
};

#endif	/* MESSAGE_GET_H */

