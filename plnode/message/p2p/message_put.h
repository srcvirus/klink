/* 
 * File:   message_put.h
 * Author: mfbari
 *
 * Created on November 29, 2012, 3:17 PM
 */

#ifndef MESSAGE_PUT_H
#define	MESSAGE_PUT_H

#include "../message.h"
#include "../../ds/ip_address.h"

#include <cstring>

class MessagePUT : public ABSMessage {
    string deviceName;
    IPAddress ip;
public:

    void SetIp(IPAddress &ip) {
        this->ip = ip;
    }

    IPAddress GetIp() const {
        return ip;
    }

    void SetDeviceName(string &deviceName) {
        this->deviceName = deviceName;
    }

    string GetDeviceName() const {
        return deviceName;
    }
};

#endif	/* MESSAGE_PUT_H */

