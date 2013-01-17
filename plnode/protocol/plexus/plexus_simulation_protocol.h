/* 
 * File:   plexus_protocol.h
 * Author: mfbari
 *
 * Created on November 29, 2012, 3:10 PM
 */

#ifndef PLEXUS_SIMULATION_PROTOCOL_H
#define	PLEXUS_SIMULATION_PROTOCOL_H

#include "../plexus/plexus_protocol.h"
#include "../../logging/log.h"

using namespace std;

class ABSProtocol;

class PlexusSimulationProtocol: public PlexusProtocol
{
	static queue<ABSMessage*> incoming_message_queue;
	static queue<ABSMessage*> outgoing_message_queue;
	static queue<LogEntry*> logging_queue;

	static pthread_mutex_t incoming_queue_lock;
	static pthread_mutex_t outgoing_queue_lock;
	static pthread_mutex_t log_queue_lock;

	static pthread_cond_t cond_incoming_queue_empty;
	static pthread_cond_t cond_outgoing_queue_empty;
	static pthread_cond_t cond_log_queue_empty;

	static Log *log[MAX_LOGS];
public:

	PlexusSimulationProtocol() :
			PlexusProtocol()
	{
		//this->routing_table = new LookupTable<OverlayID, HostAddress > ();
		//this->msgProcessor = new PlexusMessageProcessor();
		/*pthread_mutex_init(&incoming_queue_lock, NULL);
		 pthread_mutex_init(&outgoing_queue_lock, NULL);
		 pthread_mutex_init(&log_queue_lock, NULL);
		 pthread_mutex_init(&get_cache_hit_counter_lock, NULL);
		 pthread_mutex_init(&put_cache_hit_counter_lock, NULL);

		 pthread_cond_init(&cond_incoming_queue_empty, NULL);
		 pthread_cond_init(&cond_outgoing_queue_empty, NULL);
		 pthread_cond_init(&cond_log_queue_empty, NULL);

		 incoming_queue_pushed = incoming_queue_popped = 0;
		 outgoing_queue_pushed = outgoing_queue_popped = 0;
		 logging_queue_pushed = logging_queue_popped = 0;
		 get_cache_hit_count = put_cache_hit_count = 0;

		 proactive_cache = new LookupTable <OverlayID, HostAddress>();*/
		//this->msgProcessor->setContainerProtocol(this);
	}

	PlexusSimulationProtocol(LookupTable<OverlayID, HostAddress>* routing_table, LookupTable<string, HostAddress>* index_table, Cache *cache,
			MessageProcessor* msgProcessor, Peer* container) :
			PlexusProtocol(routing_table, index_table, cache, msgProcessor, container)
	{
		/*this->msgProcessor->setContainerProtocol(this);

		 pthread_mutex_init(&incoming_queue_lock, NULL);
		 pthread_mutex_init(&outgoing_queue_lock, NULL);
		 pthread_mutex_init(&log_queue_lock, NULL);
		 pthread_mutex_init(&get_cache_hit_counter_lock, NULL);
		 pthread_mutex_init(&put_cache_hit_counter_lock, NULL);

		 pthread_cond_init(&cond_incoming_queue_empty, NULL);
		 pthread_cond_init(&cond_outgoing_queue_empty, NULL);
		 pthread_cond_init(&cond_log_queue_empty, NULL);

		 incoming_queue_pushed = incoming_queue_popped = 0;
		 outgoing_queue_pushed = outgoing_queue_popped = 0;
		 logging_queue_pushed = logging_queue_popped = 0;
		 get_cache_hit_count = put_cache_hit_count = 0;

		 proactive_cache = new LookupTable <OverlayID, HostAddress>();*/
		//initLogs(container->getLogServerName().c_str(), container->getLogServerUser().c_str());
	}

	PlexusSimulationProtocol(Peer* container, MessageProcessor* msgProcessor) :
			PlexusProtocol(container, msgProcessor)
	{
		/*this->msgProcessor->setContainerProtocol(this);

		 pthread_mutex_init(&incoming_queue_lock, NULL);
		 pthread_mutex_init(&outgoing_queue_lock, NULL);
		 pthread_mutex_init(&log_queue_lock, NULL);
		 pthread_mutex_init(&get_cache_hit_counter_lock, NULL);
		 pthread_mutex_init(&put_cache_hit_counter_lock, NULL);

		 pthread_cond_init(&cond_incoming_queue_empty, NULL);
		 pthread_cond_init(&cond_outgoing_queue_empty, NULL);
		 pthread_cond_init(&cond_log_queue_empty, NULL);

		 incoming_queue_pushed = incoming_queue_popped = 0;
		 outgoing_queue_pushed = outgoing_queue_popped = 0;
		 logging_queue_pushed = logging_queue_popped = 0;
		 get_cache_hit_count = put_cache_hit_count = 0;

		 proactive_cache = new LookupTable <OverlayID, HostAddress>();*/
		//initLogs(container->getLogServerName().c_str(), container->getLogServerUser().c_str());
	}

	void initLogs(int log_seq_no, const char* log_server_name, const char* log_server_user)
	{
		if (log[LOG_GET] == NULL)
		{
			log[LOG_GET] = new Log(log_seq_no, "get", container_peer->getCacheType().c_str(), container_peer->getCacheStorage().c_str(),
					container_peer->getK(), log_server_name, log_server_user);

			log[LOG_GET]->setCheckPointRowCount(container_peer->getConfiguration()->getCheckPointRow());
			log[LOG_GET]->open("a");
		}

		if (log[LOG_PUT] == NULL)
		{
			log[LOG_PUT] = new Log(log_seq_no, "put", container_peer->getCacheType().c_str(), container_peer->getCacheStorage().c_str(),
					container_peer->getK(), log_server_name, log_server_user);

			log[LOG_PUT]->setCheckPointRowCount(container_peer->getConfiguration()->getCheckPointRow());
			log[LOG_PUT]->open("a");
		}

		if (log[LOG_STORAGE] == NULL)
		{
			log[LOG_STORAGE] = new Log(log_seq_no, "storage", container_peer->getCacheType().c_str(), container_peer->getCacheStorage().c_str(),
					container_peer->getK(), log_server_name, log_server_user);

			log[LOG_STORAGE]->setCheckPointRowCount(1);
			log[LOG_STORAGE]->open("a");
		}
	}

	void initLocks()
	{
		pthread_mutex_init(&incoming_queue_lock, NULL);
		pthread_mutex_init(&outgoing_queue_lock, NULL);
		pthread_mutex_init(&log_queue_lock, NULL);

		pthread_cond_init(&cond_incoming_queue_empty, NULL);
		pthread_cond_init(&cond_outgoing_queue_empty, NULL);
		pthread_cond_init(&cond_log_queue_empty, NULL);
	}

	void addToIncomingQueue(ABSMessage* message)
	{
		pthread_mutex_lock(&incoming_queue_lock);
		incoming_message_queue.push(message);
		incoming_queue_pushed++;
		pthread_cond_broadcast(&cond_incoming_queue_empty);
		pthread_mutex_unlock(&incoming_queue_lock);
	}

	bool isIncomingQueueEmpty()
	{
		bool status;
		pthread_mutex_lock(&incoming_queue_lock);
		status = incoming_message_queue.empty();
		pthread_mutex_unlock(&incoming_queue_lock);
		return status;
	}

	ABSMessage* getIncomingQueueFront()
	{
		pthread_mutex_lock(&incoming_queue_lock);

		while (incoming_message_queue.empty())
			pthread_cond_wait(&cond_incoming_queue_empty, &incoming_queue_lock);

		ABSMessage* ret = incoming_message_queue.front();

		incoming_message_queue.pop();
		incoming_queue_popped++;
		pthread_mutex_unlock(&incoming_queue_lock);
		return ret;
	}

	void addToOutgoingQueue(ABSMessage* message)
	{
		pthread_mutex_lock(&outgoing_queue_lock);

		outgoing_message_queue.push(message);
		outgoing_queue_pushed++;
		pthread_cond_broadcast(&cond_outgoing_queue_empty);
		pthread_mutex_unlock(&outgoing_queue_lock);
	}

	bool isOutgoingQueueEmpty()
	{
		bool status;
		pthread_mutex_lock(&outgoing_queue_lock);
		status = outgoing_message_queue.empty();
		pthread_mutex_unlock(&outgoing_queue_lock);
		return status;
	}

	ABSMessage* getOutgoingQueueFront()
	{
		pthread_mutex_lock(&outgoing_queue_lock);

		while (outgoing_message_queue.empty())
		{
			//printf("Waiting for a message in outgoing queue");
			pthread_cond_wait(&cond_outgoing_queue_empty, &outgoing_queue_lock);
		}

		ABSMessage* ret = outgoing_message_queue.front();

		//printf("Got a messge from the outgoing queue");
		outgoing_message_queue.pop();
		outgoing_queue_popped++;
		pthread_mutex_unlock(&outgoing_queue_lock);
		return ret;
	}

	void addToLogQueue(LogEntry* log_entry)
	{
		pthread_mutex_lock(&log_queue_lock);
		logging_queue.push(log_entry);
		logging_queue_pushed++;
		pthread_cond_signal(&cond_log_queue_empty);
		pthread_mutex_unlock(&log_queue_lock);
	}

	bool isLogQueueEmpty()
	{
		bool status;
		pthread_mutex_lock(&log_queue_lock);
		status = logging_queue.empty();
		pthread_mutex_unlock(&log_queue_lock);
		return status;
	}

	LogEntry* getLoggingQueueFront()
	{
		pthread_mutex_lock(&log_queue_lock);
		while (logging_queue.empty())
		{
			pthread_cond_wait(&cond_log_queue_empty, &log_queue_lock);
		}
		LogEntry* ret = logging_queue.front();
		logging_queue.pop();
		logging_queue_popped++;
		pthread_mutex_unlock(&log_queue_lock);
		return ret;
	}

	Log* getGetLog()
	{
		return log[LOG_GET];
	}

	Log* getPutLog()
	{
		return log[LOG_PUT];
	}

	Log* getLog(int type)
	{
		if (type >= MAX_LOGS)
			return NULL;
		return log[type];
	}

	void flushAllLog()
	{
		int i;
		for (i = 0; i < MAX_LOGS; i++)
		{
			log[i]->flush();
		}
	}
};

queue<ABSMessage*> PlexusSimulationProtocol::incoming_message_queue;
queue<ABSMessage*> PlexusSimulationProtocol::outgoing_message_queue;
queue<LogEntry*> PlexusSimulationProtocol::logging_queue;

pthread_mutex_t PlexusSimulationProtocol::incoming_queue_lock;
pthread_mutex_t PlexusSimulationProtocol::outgoing_queue_lock;
pthread_mutex_t PlexusSimulationProtocol::log_queue_lock;

pthread_cond_t PlexusSimulationProtocol::cond_incoming_queue_empty;
pthread_cond_t PlexusSimulationProtocol::cond_outgoing_queue_empty;
pthread_cond_t PlexusSimulationProtocol::cond_log_queue_empty;

Log* PlexusSimulationProtocol::log[MAX_LOGS] = { NULL, NULL, NULL };
#endif	/* PLEXUS_PROTOCOL_H */

