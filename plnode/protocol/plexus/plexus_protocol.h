/* 
 * File:   plexus_protocol.h
 * Author: mfbari
 *
 * Created on November 29, 2012, 3:10 PM
 */

#ifndef PLEXUS_PROTOCOL_H
#define	PLEXUS_PROTOCOL_H

#include <pthread.h>
#include <cstring>
#include <queue>
#include "../protocol.h"
#include "../../ds/cache.h"
#include "../../message/message_processor.h"
//#include "plexus_message_processor.h"
#include "../../message/p2p/message_get.h"
#include "../../message/p2p/message_put.h"
#include "../../message/p2p/message_get_reply.h"
#include "../../message/control/peer_initiate_get.h"
#include "../../message/control/peer_initiate_put.h"
#include "../../ds/host_address.h"
#include "../../message/message.h"
#include "../../logging/log.h"
#include "../../logging/log_entry.h"

using namespace std;

class ABSProtocol;

#define MAX_LOGS 2
#define LOG_GET 0
#define LOG_PUT 1

class PlexusProtocol : public ABSProtocol {
        Log *log[MAX_LOGS];

        queue<ABSMessage*> incoming_message_queue;
        queue<ABSMessage*> outgoing_message_queue;
        queue<LogEntry*> logging_queue;

        pthread_mutex_t incoming_queue_lock;
        pthread_mutex_t outgoing_queue_lock;
        pthread_mutex_t log_queue_lock;

        pthread_cond_t cond_incoming_queue_empty;
        pthread_cond_t cond_outgoing_queue_empty;
        pthread_cond_t cond_log_queue_empty;
public:


        PlexusProtocol() :
        ABSProtocol() {
                //this->routing_table = new LookupTable<OverlayID, HostAddress > ();

                //this->msgProcessor = new PlexusMessageProcessor();
                pthread_mutex_init(&incoming_queue_lock, NULL);
                pthread_mutex_init(&outgoing_queue_lock, NULL);
                pthread_mutex_init(&log_queue_lock, NULL);

                pthread_cond_init(&cond_incoming_queue_empty, NULL);
                pthread_cond_init(&cond_outgoing_queue_empty, NULL);
                pthread_cond_init(&cond_log_queue_empty, NULL);

                //this->msgProcessor->setContainerProtocol(this);
        }

        PlexusProtocol(LookupTable<OverlayID, HostAddress>* routing_table,
                LookupTable<string, HostAddress>* index_table, Cache *cache,
                MessageProcessor* msgProcessor, Peer* container) :
        ABSProtocol(routing_table, index_table, cache, msgProcessor,
        container) {
                this->msgProcessor->setContainerProtocol(this);

                pthread_mutex_init(&incoming_queue_lock, NULL);
                pthread_mutex_init(&outgoing_queue_lock, NULL);
                pthread_mutex_init(&log_queue_lock, NULL);

                pthread_cond_init(&cond_incoming_queue_empty, NULL);
                pthread_cond_init(&cond_outgoing_queue_empty, NULL);
                pthread_cond_init(&cond_log_queue_empty, NULL);

                initLogs(container->getLogServerName().c_str(), container->getLogServerUser().c_str());
        }

        PlexusProtocol(Peer* container, MessageProcessor* msgProcessor) :
        ABSProtocol(container, msgProcessor) {
                this->msgProcessor->setContainerProtocol(this);

                pthread_mutex_init(&incoming_queue_lock, NULL);
                pthread_mutex_init(&outgoing_queue_lock, NULL);
                pthread_mutex_init(&log_queue_lock, NULL);

                pthread_cond_init(&cond_incoming_queue_empty, NULL);
                pthread_cond_init(&cond_outgoing_queue_empty, NULL);
                pthread_cond_init(&cond_log_queue_empty, NULL);

                initLogs(container->getLogServerName().c_str(), container->getLogServerUser().c_str());
        }

        void initLogs(const char* log_server_name, const char* log_server_user) {
                log[LOG_GET] = new Log("seq", "get", log_server_name,
                        log_server_user);
                log[LOG_PUT] = new Log("seq", "put", log_server_name,
                        log_server_user);

                log[LOG_GET]->open("a");
                log[LOG_PUT]->open("a");
        }

        void processMessage(ABSMessage *message) {
                msgProcessor->processMessage(message);
        }

        void initiate_join() {
        }

        void process_join() {
        }
        
        bool setNextHop2(ABSMessage* msg) {
                printf("Setting next hop, Message type = %d\n", msg->getMessageType());
                int maxLengthMatch = 0, currentMatchLength = 0, currentNodeMathLength = 0;
                HostAddress next_hop;

                switch (msg->getMessageType()) {
                        case MSG_PEER_INIT:
                        case MSG_PEER_CONFIG:
                        case MSG_PEER_CHANGE_STATUS:
                        case MSG_PEER_START:
                        case MSG_GENERATE_NAME:
                        case MSG_DYN_CHANGE_STATUS:
                        case MSG_PLEXUS_GET_REPLY:
                        case MSG_PEER_INITIATE_GET:
                        case MSG_PEER_INITIATE_PUT:
                        		puts("returning false");
                                return false;
                                break;
                }

                if (msg->getOverlayTtl() == 0)
                        return false;

                currentNodeMathLength = container_peer->getOverlayID().GetMatchedPrefixLength(msg->getDstOid());
                printf("Current match length = %d\n", currentNodeMathLength);
                printf("Message oid = %d\n", msg->getDstOid().GetOverlay_id());
                msg->getDstOid().printBits();
                putchar('\n');

                //cout << endl << "current node match : ";
                //container_peer->getOverlayID().printBits();
                //cout << " <> ";
                //msg->getOID().printBits();
                //cout << " = " << currentNodeMathLength << endl;

                //search in the RT
                //        OverlayID::MAX_LENGTH = GlobalData::rm->rm->k;
                //cout << "S OID M LEN " << OverlayID::MAX_LENGTH << endl;
                puts("creating iterator");
                LookupTableIterator<OverlayID, HostAddress> rtable_iterator(routing_table);
                rtable_iterator.reset_iterator();

                puts("looking up in routing table");
                OverlayID maxMatchOid;
                //routing_table->reset_iterator();
                while (rtable_iterator.hasMoreKey()) {
                        OverlayID oid = rtable_iterator.getNextKey();
                        printf("next key = %d My id = ", oid.GetOverlay_id());
                        msg->getDstOid().printBits();
                        putchar('\n');

                        //cout << endl << "current match ";
                        //oid.printBits();
                        currentMatchLength = msg->getDstOid().GetMatchedPrefixLength(oid);
                        //cout << " ==== " << currentMatchLength << endl;
                        printf(">current match length = %d\n", currentMatchLength);

                        if (currentMatchLength > maxLengthMatch) {
                                maxLengthMatch = currentMatchLength;
                                maxMatchOid = oid;

                                /*printf("next host %s, next port %d\n",
                                                next_hop.GetHostName().c_str(), next_hop.GetHostPort());*/
                        }
                }
                routing_table->lookup(maxMatchOid, &next_hop);
                //search in the Cache
                /*cache->reset_iterator();
                while (cache->has_next())
                {
                        DLLNode *node = cache->get_next();
                        OverlayID id = node->key;
                        currentMatchLength = msg->getDstOid().GetMatchedPrefixLength(id);
                        if (currentMatchLength > maxLengthMatch)
                        {
                                maxLengthMatch = currentMatchLength;
                                cache->lookup(msg->getDstOid(), next_hop);
                                printf("next host %s, next port %d\n",next_hop.GetHostName().c_str(), next_hop.GetHostPort());
                        }
                }*/

                cout << endl << "max match : = " << maxLengthMatch << endl;

                if (maxLengthMatch == 0 || maxLengthMatch < currentNodeMathLength) {
                        puts("returning false");
                        //msg->setDestHost("localhost");
                        //msg->setDestPort(container_peer->getListenPortNumber());
                        return false;
                } else {
                        puts("returning true");
                        msg->setDestHost(next_hop.GetHostName().c_str());
                        msg->setDestPort(next_hop.GetHostPort());
                        return true;
                }
        }

        bool setNextHop(ABSMessage* msg) {
                printf("Setting next hop, Message type = %d\n", msg->getMessageType());
                int maxLengthMatch = 0, currentMatchLength = 0, currentNodeMathLength = 0;
                HostAddress next_hop;

                switch (msg->getMessageType()) {
                        case MSG_PEER_INIT:
                        case MSG_PEER_CONFIG:
                        case MSG_PEER_CHANGE_STATUS:
                        case MSG_PEER_START:
                        case MSG_GENERATE_NAME:
                        case MSG_DYN_CHANGE_STATUS:
                        case MSG_PLEXUS_GET_REPLY:
                        case MSG_PEER_INITIATE_GET:
                        case MSG_PEER_INITIATE_PUT:
                        		puts("returning false");
                                return false;
                                break;
                }

                if (msg->getOverlayTtl() == 0)
                        return false;

                currentNodeMathLength = container_peer->getOverlayID().GetMatchedPrefixLength(msg->getDstOid());
                printf("Current match length = %d\n", currentNodeMathLength);
                printf("Message oid = %d\n", msg->getDstOid().GetOverlay_id());
                msg->getDstOid().printBits();
                putchar('\n');

                //cout << endl << "current node match : ";
                //container_peer->getOverlayID().printBits();
                //cout << " <> ";
                //msg->getOID().printBits();
                //cout << " = " << currentNodeMathLength << endl;

                //search in the RT
                //        OverlayID::MAX_LENGTH = GlobalData::rm->rm->k;
                //cout << "S OID M LEN " << OverlayID::MAX_LENGTH << endl;
                puts("creating iterator");
                LookupTableIterator<OverlayID, HostAddress> rtable_iterator(routing_table);
                rtable_iterator.reset_iterator();

                puts("looking up in routing table");
                OverlayID maxMatchOid;
                //routing_table->reset_iterator();
                while (rtable_iterator.hasMoreKey()) {
                        //while (routing_table->hasMoreKey()) {
                        //   OverlayID oid = routing_table->getNextKey();
                        OverlayID oid = rtable_iterator.getNextKey();
                        printf("next key = %d My id = ", oid.GetOverlay_id());
                        msg->getDstOid().printBits();
                        putchar('\n');

                        //cout << endl << "current match ";
                        //oid.printBits();
                        currentMatchLength = msg->getDstOid().GetMatchedPrefixLength(oid);
                        //cout << " ==== " << currentMatchLength << endl;
                        printf(">current match length = %d\n", currentMatchLength);

                        if (currentMatchLength > maxLengthMatch) {
                                maxLengthMatch = currentMatchLength;
                                maxMatchOid = oid;

                                /*printf("next host %s, next port %d\n",
                                                next_hop.GetHostName().c_str(), next_hop.GetHostPort());*/
                        }
                }
                routing_table->lookup(maxMatchOid, &next_hop);
                //search in the Cache
                /*cache->reset_iterator();
                while (cache->has_next())
                {
                        DLLNode *node = cache->get_next();
                        OverlayID id = node->key;
                        currentMatchLength = msg->getDstOid().GetMatchedPrefixLength(id);
                        if (currentMatchLength > maxLengthMatch)
                        {
                                maxLengthMatch = currentMatchLength;
                                cache->lookup(msg->getDstOid(), next_hop);
                                printf("next host %s, next port %d\n",next_hop.GetHostName().c_str(), next_hop.GetHostPort());
                        }
                }*/

                cout << endl << "max match : = " << maxLengthMatch << endl;

                if (maxLengthMatch == 0 || maxLengthMatch < currentNodeMathLength) {
                        puts("returning false");
                        //msg->setDestHost("localhost");
                        //msg->setDestPort(container_peer->getListenPortNumber());
                        return false;
                } else {
                        puts("returning true");
                        msg->setDestHost(next_hop.GetHostName().c_str());
                        msg->setDestPort(next_hop.GetHostPort());
                        return true;
                }
        }

        void get(string name) {
                int hash_name_to_get = atoi(name.c_str());
                OverlayID destID(hash_name_to_get);

                printf("h_name = %d, oid = %d\n", hash_name_to_get, destID.GetOverlay_id());

                MessageGET *msg = new MessageGET(container_peer->getHostName(),
                        container_peer->getListenPortNumber(), "", -1,
                        container_peer->getOverlayID(), destID, name);

                printf("Constructed Get Message");
                msg->message_print_dump();

                if (msgProcessor->processMessage(msg))
                {
                        msg->setIssueTimeStamp();
                        addToOutgoingQueue(msg);
                }
                getContainerPeer()->incrementGet_generated();
        }

        void get_from_client(string name, HostAddress destination) {
                int hash_name_to_get = atoi(name.c_str());
                OverlayID destID(hash_name_to_get);

                cout << "id = " << hash_name_to_get << " oid = ";
                destID.printBits();
                cout << endl;

                PeerInitiateGET *msg = new PeerInitiateGET(
                        container_peer->getHostName(),
                        container_peer->getListenPortNumber(),
                        destination.GetHostName(), destination.GetHostPort(),
                        container_peer->getOverlayID(), destID, name);
                msg->calculateOverlayTTL(getContainerPeer()->getNPeers());
                msg->message_print_dump();
                send_message(msg);
        }

        void put(string name, HostAddress hostAddress) {
                int hash_name_to_publish = atoi(name.c_str());
                OverlayID destID(hash_name_to_publish);

                MessagePUT *msg = new MessagePUT(container_peer->getHostName(),
                        container_peer->getListenPortNumber(), "", -1,
                        container_peer->getOverlayID(), destID, name, hostAddress);

                if (msgProcessor->processMessage(msg)) {
                        msg->setIssueTimeStamp();
                        addToOutgoingQueue(msg);
                }
                getContainerPeer()->incrementPut_generated();
        }

        void put_from_client(string name, HostAddress hostAddress,
                HostAddress destination) {
                int hash_name_to_publish = atoi(name.c_str());
                OverlayID destID(hash_name_to_publish);

                cout << "id = " << hash_name_to_publish << " oid = ";
                destID.printBits();
                cout << endl;

                PeerInitiatePUT *msg = new PeerInitiatePUT(
                        container_peer->getHostName(),
                        container_peer->getListenPortNumber(),
                        destination.GetHostName(), destination.GetHostPort(),
                        container_peer->getOverlayID(), destID, name, hostAddress);
                msg->calculateOverlayTTL(getContainerPeer()->getNPeers());

                msg->message_print_dump();
                send_message(msg);
        }

        void rejoin() {
        }

        void addToIncomingQueue(ABSMessage* message) {
                pthread_mutex_lock(&incoming_queue_lock);
                if(message->getMessageType() == MSG_PLEXUS_GET || message->getMessageType() == MSG_PLEXUS_PUT)
                	message->setInQueuePushTimeStamp();

                incoming_message_queue.push(message);
                pthread_cond_broadcast(&cond_incoming_queue_empty);
                pthread_mutex_unlock(&incoming_queue_lock);
        }

        bool isIncomingQueueEmpty() {
                bool status;
                pthread_mutex_lock(&incoming_queue_lock);
                status = incoming_message_queue.empty();
                pthread_mutex_unlock(&incoming_queue_lock);
                return status;
        }

        ABSMessage* getIncomingQueueFront() {
                pthread_mutex_lock(&incoming_queue_lock);

                while (incoming_message_queue.empty())
                        pthread_cond_wait(&cond_incoming_queue_empty, &incoming_queue_lock);

                ABSMessage* ret = incoming_message_queue.front();
                if(ret->getMessageType() == MSG_PLEXUS_GET || ret->getMessageType() == MSG_PLEXUS_PUT)
                	ret->setInQueuePopTimeStamp();
                incoming_message_queue.pop();
                pthread_mutex_unlock(&incoming_queue_lock);
                return ret;
        }

        void addToOutgoingQueue(ABSMessage* message) {
                pthread_mutex_lock(&outgoing_queue_lock);
                if(message->getMessageType() == MSG_PLEXUS_GET || message->getMessageType() == MSG_PLEXUS_PUT)
                	message->setOutQueuePushTimeStamp();
                outgoing_message_queue.push(message);
                pthread_cond_broadcast(&cond_outgoing_queue_empty);
                pthread_mutex_unlock(&outgoing_queue_lock);
        }

        bool isOutgoingQueueEmpty() {
                bool status;
                pthread_mutex_lock(&outgoing_queue_lock);
                status = outgoing_message_queue.empty();
                pthread_mutex_unlock(&outgoing_queue_lock);
                return status;
        }

        ABSMessage* getOutgoingQueueFront() {
                pthread_mutex_lock(&outgoing_queue_lock);

                while (outgoing_message_queue.empty()) {
                        //printf("Waiting for a message in outgoing queue");
                        pthread_cond_wait(&cond_outgoing_queue_empty, &outgoing_queue_lock);
                }

                ABSMessage* ret = outgoing_message_queue.front();
                if(ret->getMessageType() == MSG_PLEXUS_GET || ret->getMessageType() == MSG_PLEXUS_PUT)
                	ret->setOutQueuePopTimeStamp();
                //printf("Got a messge from the outgoing queue");
                outgoing_message_queue.pop();
                pthread_mutex_unlock(&outgoing_queue_lock);
                return ret;
        }

        void addToLogQueue(LogEntry* log_entry) {
                pthread_mutex_lock(&log_queue_lock);
                logging_queue.push(log_entry);
                pthread_cond_signal(&cond_log_queue_empty);
                pthread_mutex_unlock(&log_queue_lock);
        }

        bool isLogQueueEmpty() {
                bool status;
                pthread_mutex_lock(&log_queue_lock);
                status = logging_queue.empty();
                pthread_mutex_unlock(&log_queue_lock);
                return status;
        }

        LogEntry* getLoggingQueueFront() {
                pthread_mutex_lock(&log_queue_lock);
                while (logging_queue.empty()) {
                        pthread_cond_wait(&cond_log_queue_empty, &log_queue_lock);
                }
                LogEntry* ret = logging_queue.front();
                logging_queue.pop();
                pthread_mutex_unlock(&log_queue_lock);
                return ret;
        }

        Log* getGetLog() {
                return log[LOG_GET];
        }

        Log* getPutLog() {
                return log[LOG_PUT];
        }

        Log* getLog(int type) {
                if (type >= MAX_LOGS)
                        return NULL;
                return log[type];
        }

        ~PlexusProtocol() {
                pthread_mutex_destroy(&incoming_queue_lock);
                pthread_mutex_destroy(&outgoing_queue_lock);
                pthread_mutex_destroy(&log_queue_lock);

                pthread_cond_destroy(&cond_incoming_queue_empty);
                pthread_cond_destroy(&cond_outgoing_queue_empty);
                pthread_cond_destroy(&cond_log_queue_empty);
        }
};

#endif	/* PLEXUS_PROTOCOL_H */

