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

    MessageGET(string &deviceName) :
    ABSMessage() {
        messageType = MSG_PLEXUS_GET;
        this->deviceName = deviceName;
    }

    virtual char* serialize(int* serialize_length) {
        *serialize_length = getBaseSize();
        *serialize_length += (sizeof (int) + sizeof (char) * deviceName.size());

        char* buffer = new char[*serialize_length];
        int offset = 0;
        int destHostLength = dest_host.size();
        int sourceHostLength = source_host.size();

        memcpy(buffer + offset, (char*) (&messageType), sizeof (char));
        offset += sizeof (char);
        memcpy(buffer + offset, (char*) (&sequence_no), sizeof (int));
        offset += sizeof (int);

        memcpy(buffer + offset, (char*) (&destHostLength), sizeof (int));
        offset += sizeof (int);
        for (int i = 0; i < destHostLength; i++) {
            char ch = dest_host[i];
            memcpy(buffer + offset, (char*) (&ch), sizeof (char));
            offset += sizeof (char);
        }
        memcpy(buffer + offset, (char*) (&dest_port), sizeof (int));
        offset += sizeof (int);

        memcpy(buffer + offset, (char*) (&sourceHostLength), sizeof (int));
        offset += sizeof (int);
        for (int i = 0; i < sourceHostLength; i++) {
            char ch = source_host[i];
            memcpy(buffer + offset, (char*) (&ch), sizeof (char));
            offset += sizeof (char);
        }
        memcpy(buffer + offset, (char*) (&source_port), sizeof (int));
        offset += sizeof (int);

        memcpy(buffer + offset, (char*) (&overlay_hops), sizeof (char));
        offset += sizeof (char);
        memcpy(buffer + offset, (char*) (&overlay_ttl), sizeof (char));
        offset += sizeof (char);
        memcpy(buffer + offset, (char*) (&oID), sizeof (OverlayID));
        offset += sizeof (OverlayID);

        int deviceNameLength = deviceName.size();
        memcpy(buffer + offset, (char*) (&deviceNameLength), sizeof (int));
        offset += sizeof (int);
        for (int i = 0; i < deviceNameLength; i++) {
            char ch = deviceName[i];
            memcpy(buffer + offset, (char*) (&ch), sizeof (char));
            offset += sizeof (char);
        }

        return buffer;
    }

    ABSMessage* deserialize(char* buffer, int buffer_length) {
        int offset = 0;
        int destHostLength, sourceHostLength, deviceNameLength,
                hostAddrNameLength;

        memcpy(&messageType, buffer + offset, sizeof (char));
        offset += sizeof (char); //printf("offset = %d\n", offset);
        memcpy(&sequence_no, buffer + offset, sizeof (int));
        offset += sizeof (int); //printf("offset = %d\n", offset);
        memcpy(&destHostLength, buffer + offset, sizeof (int));
        offset += sizeof (int); //printf("offset = %d\n", offset);
        dest_host = "";
        //printf("DH Length : %d\n", destHostLength);
        for (int i = 0; i < destHostLength; i++) {
            char ch;
            memcpy(&ch, buffer + offset, sizeof (char));
            offset += sizeof (char); //printf("offset = %d\n", offset);
            dest_host += ch;
        }
        memcpy(&dest_port, buffer + offset, sizeof (int));
        offset += sizeof (int); //printf("offset = %d\n", offset);
        memcpy(&sourceHostLength, buffer + offset, sizeof (int));
        offset += sizeof (int); //printf("offset = %d\n", offset);
        source_host = "";
        for (int i = 0; i < sourceHostLength; i++) {
            char ch;
            memcpy(&ch, buffer + offset, sizeof (char));
            offset += sizeof (char); //printf("offset = %d\n", offset);
            source_host += ch;
        }
        memcpy(&source_port, buffer + offset, sizeof (int));
        offset += sizeof (int); //printf("offset = %d\n", offset);
        memcpy(&overlay_hops, buffer + offset, sizeof (char));
        offset += sizeof (char); //printf("offset = %d\n", offset);
        memcpy(&overlay_ttl, buffer + offset, sizeof (char));
        offset += sizeof (char); //printf("offset = %d\n", offset);
        memcpy(&oID, buffer + offset, sizeof (OverlayID));
        offset += sizeof (OverlayID); //printf("offset = %d\n", offset);

        memcpy(&deviceNameLength, buffer + offset, sizeof (int));
        offset += sizeof (int);
        deviceName = "";
        for (int i = 0; i < deviceNameLength; i++) {
            char ch;
            memcpy(&ch, buffer + offset, sizeof (char));
            offset += sizeof (char); //printf("offset = %d\n", offset);
            deviceName += ch;
        }

        return this;
    }

    void SetDeviceName(string &deviceName) {
        this->deviceName = deviceName;
    }

    string GetDeviceName() const {
        return deviceName;
    }

    virtual void message_print_dump() {
        printf("Get Message");
        ABSMessage::message_print_dump();
        printf("Device Name %s\n", deviceName.c_str());
    }
};

#endif	/* MESSAGE_GET_H */

