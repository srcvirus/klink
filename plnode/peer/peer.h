/*
 * peer.h
 *
 *  Created on: 2012-11-21
 *      Author: sr2chowd
 */

#ifndef PEER_H_
#define PEER_H_

#include <string.h>
#include "../protocol/protocol.h"
#include "../ds/overlay_id.h"
#include "../ds/ip_address.h"
#include "../../communication/server_socket.h"

class ABSProtocol;

class ABSProtocol;

class Peer {
        int n_peers;

        int peer_id;
        int code_word;
        OverlayID overlay_id;
        int status;

        IPAddress ip_address;
        string host_name;
        int listen_port_number;

        int publish_name_range_start;
        int publish_name_range_end;
        int lookup_name_range_start;
        int lookup_name_range_end;
        double alpha;
        int k;

        ABSProtocol* protocol;
        ServerSocket* server_socket;

public:

        void INIT() {
                char hostname[100], domain_name[100];
                gethostname(hostname, 100);
                //getdomainname(domain_name, 100);
                struct hostent* host_info;
                host_info = gethostbyname(hostname);
                host_name = string(host_info->h_name);
                //host_name = string(strcat(hostname, strcat(".", domain_name)));
        }

        Peer() {
                INIT();
                listen_port_number = 0;
        }

        Peer(int port) {
                INIT();
                listen_port_number = port;
                server_socket = new ServerSocket(listen_port_number);
        }

        Peer(const char* hosts_file) {
                INIT();
                listen_port_number = -1;
                FILE* hosts_ptr = fopen(hosts_file, "r");
                char host_name[200];
                int port = -1;
                int n_hosts;
                fscanf(hosts_ptr, "%d", &n_hosts);

                for (int i = 0; i < n_hosts; i++) {
                        fscanf(hosts_ptr, "%s %d", host_name, &port);
                        if (strncmp(this->getHostName().c_str(), host_name, strlen(this->getHostName().c_str())) == 0) {
                                listen_port_number = port;
                                break;
                        }
                }
                fclose(hosts_ptr);

                if (listen_port_number != -1) {
                        server_socket = new ServerSocket(listen_port_number);
                }
        }

        ~Peer() {
                delete protocol;
                delete server_socket;
        }

        double getAlpha() {
                return alpha;
        }

        void setAlpha(double alpha) {
                this->alpha = alpha;
        }

        int getCodeWord() {
                return code_word;
        }

        void setCodeWord(int codeWord) {
                code_word = codeWord;
        }

        IPAddress getIpAddress() {
                return ip_address;
        }

        int getIntIpAddress() {
                return ip_address.getIpAddress();
        }

        string getStrIpAddress() {
                return ip_address.getStrIpAddress();
        }

        void setIpAddress(char* ipAddress) {
                ip_address.setIp(ipAddress);
        }

        int getK() {
                return k;
        }

        void setK(int k) {
                this->k = k;
        }

        int getPeerId() {
                return peer_id;
        }

        void setPeerId(int peerId) {
                peer_id = peerId;
        }

        void setHostName(string host_name) {
                this->host_name = host_name;
        }

        string getHostName() {
                return host_name;
        }

        int getListenPortNumber() {
                return listen_port_number;
        }

        void setListenPortNumber(int port) {
                listen_port_number = port;
        }

        ABSProtocol* getProtocol() {
                return protocol;
        }

        void setProtocol(ABSProtocol* protocol) {
                this->protocol = protocol;
        }

        ServerSocket* getServerSocket() {
                return server_socket;
        }

        void setServerSocket(ServerSocket* socket) {
                server_socket = socket;
        }

        void setNPeers(int n) {
                n_peers = n;
        }

        int getNPeers() {
                return n_peers;
        }

        OverlayID getOverlayID() {
                return overlay_id;
        }

        void setOverlayID(OverlayID id) {
                overlay_id = id;
        }

        void setStatus(int status) {
                this->status = status;
        }

        int getStatus() const {
                return status;
        }

        void setLookup_name_range_end(int lookup_name_range_end) {
                this->lookup_name_range_end = lookup_name_range_end;
        }

        int getLookup_name_range_end() const {
                return lookup_name_range_end;
        }

        void setLookup_name_range_start(int lookup_name_range_start) {
                this->lookup_name_range_start = lookup_name_range_start;
        }

        int getLookup_name_range_start() const {
                return lookup_name_range_start;
        }

        void setPublish_name_range_end(int publish_name_range_end) {
                this->publish_name_range_end = publish_name_range_end;
        }

        int getPublish_name_range_end() const {
                return publish_name_range_end;
        }

        void setPublish_name_range_start(int publish_name_range_start) {
                this->publish_name_range_start = publish_name_range_start;
        }

        int getPublish_name_range_start() const {
                return publish_name_range_start;
        }

};

#endif /* PEER_H_ */
