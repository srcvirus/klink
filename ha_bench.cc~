#include <stdio.h>
#include <map>
#include <string>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdlib.h>
#include <vector>
#include <queue>

#include "ha_bench.h"

#include "communication/server_socket.h"
#include "communication/client_socket.h"

#include "plnode/message/control/peer_initiate_get.h"
#include "plnode/message/p2p/message_get_reply.h"

using namespace std;

#define	MAX_BUFFER_LEN		100
#define MAX_WORKER_THREADS	8

#define _min(a,b) (a) < (b) ? (a) : (b)
#define _max(a,b) (a) > (b) ? (a) : (b)

/* Global variables */

int	requests_per_second 	= 200;
int	experiment_duration 	= 10;
string	home_agent_name 	= "localhost";
int	home_agent_port 	= 55231;
int	listen_port		= 57123;
int	n_threads 		= 4;
string	query_file		= "query_sequence_benchmark.dat";
string	config_file_name	= "habench.conf";
string	source_host		= "localhost";

ServerSocket *listen_socket = NULL;
int fd_max;
fd_set connection_pool, read_fds;

vector <string> query[MAX_WORKER_THREADS];
map < string, queue <timeval> > unresolved_requests;

pthread_mutex_t unresolved_requests_lock;

double min_latency, max_latency, avg_latency;
vector <int> latency_set;

double get_average(vector <int> v)
{
	int sum = 0;
	for(int i = 0; i < v.size(); i++) sum += v[i];
	return (double)sum / (double)v.size();
}

void add_unresolved_request(string q, timeval ts)
{
	pthread_mutex_lock(&unresolved_requests_lock);
	if(unresolved_requests.find(q) == unresolved_requests.end())
	{
		unresolved_requests.insert(make_pair(q, queue <timeval>()));
	}
	unresolved_requests[q].push(ts);
	pthread_mutex_unlock(&unresolved_requests_lock);
}

timeval remove_front_unresolved_requests(string q)
{
	timeval ret;
	ret.tv_sec = ret.tv_usec = 0;

	pthread_mutex_lock(&unresolved_requests_lock);
	ret = unresolved_requests[q].front();
	unresolved_requests[q].pop();
	pthread_mutex_unlock(&unresolved_requests_lock);

	return ret;
}

int initialize_listen_socket()
{
	FD_ZERO(&connection_pool);
	FD_ZERO(&read_fds);
	listen_socket = new ServerSocket(listen_port);

	if( listen_socket->init_connection() < 0 )
		return -1;

	FD_SET(listen_socket->getSocketFd(), &connection_pool);
	fd_max = listen_socket->getSocketFd();
	source_host = listen_socket->getServerHostName();
	printf("Listening for incoming home agent connection at %s:%d\n", source_host.c_str(), listen_port);
	return 0;
}

int timeval_subtract(timeval* result, timeval* x, timeval* y)
{
	/* Perform the carry for the later subtraction by updating y. */
       	if (x->tv_usec < y->tv_usec) 
	{
	     	int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
      		y->tv_usec -= 1000000 * nsec;
	   	y->tv_sec += nsec;
   	}
       	if (x->tv_usec - y->tv_usec > 1000000) 
	{
     		int nsec = (x->tv_usec - y->tv_usec) / 1000000;
	    	y->tv_usec += 1000000 * nsec;
       		y->tv_sec -= nsec;
       	}

       /* Compute the time remaining to wait.
          tv_usec is certainly positive. */
       result->tv_sec = x->tv_sec - y->tv_sec;
       result->tv_usec = x->tv_usec - y->tv_usec;

       /* Return 1 if result is negative. */
       return x->tv_sec < y->tv_sec;
}

long perform_client_io(int thread_id, int row_index)
{
	struct timeval start, end, diff, ts;
	long delay_ms = 0.0;
	string q = query[thread_id][row_index];

	gettimeofday(&start, NULL);
	PeerInitiateGET* pInitGet = new PeerInitiateGET(source_host, listen_port,
					home_agent_name, home_agent_port, OverlayID(),
					OverlayID(), q);
	ClientSocket* client_socket = new ClientSocket(home_agent_name, home_agent_port);
	client_socket->connect_to_server();
	char* buffer;
	int buffer_length = 0;

	buffer = pInitGet->serialize(&buffer_length);	
	timeval timeout;
	timeout.tv_sec = 5;
	timeout.tv_usec = 500;
	client_socket->send_data(buffer, buffer_length, &timeout);
	client_socket->close_socket();

	delete client_socket;
	delete pInitGet;
	delete buffer;
	
	q = q.substr(0, (int)q.size() - 1);
	int last_dot = q.find_last_of(".");
	q = q.substr(0, last_dot);
	//printf("[Worker thread %d]\tSent request for %s\n", thread_id, q.c_str());
	add_unresolved_request(q, start);

	gettimeofday(&end, NULL);
	timeval_subtract(&diff, &end, &start);
	delay_ms = diff.tv_sec * 1000 + (diff.tv_usec / 1000);
	return delay_ms;
}

void* listener_thread( void* args )
{
	size_t buffer_length;
	char *buffer;
	ABSMessage *received_message;
	timeval send_time, receive_time, diff_time;
	int n_response_received = 0, latency;
	string q = "";

	printf("Starting a listener thread on port %d\n", listen_port);
	int total_requests = experiment_duration * requests_per_second;

	while(n_response_received < total_requests)
	{
		read_fds = connection_pool;
		int n_select = select(fd_max + 1, &read_fds, NULL, NULL, NULL);
		if(n_select < 0) continue;

		for(int i = 0; i <= fd_max; i++)
		{
			if(FD_ISSET(i, &read_fds))
			{
				if( i == listen_socket->getSocketFd() )
				{
					int connection_fd = listen_socket->accept_connection();
					if(connection_fd < 0) continue;
					FD_SET(connection_fd, &connection_pool);
					if(connection_fd > fd_max)
						fd_max = connection_fd;
				}
				else
				{
					buffer_length = listen_socket->receive_data(i, &buffer);
					listen_socket->close_connection(i);
					FD_CLR(i, &connection_pool);
					fd_max = listen_socket->getMaxConnectionFd();
					
					if(buffer_length > 0)
					{
						gettimeofday(&receive_time, NULL);
						received_message = new MessageGET_REPLY();
						received_message->deserialize(buffer, buffer_length);
						q = ((MessageGET_REPLY*)received_message)->getDeviceName();

						send_time = remove_front_unresolved_requests(q);
						timeval_subtract(&diff_time, &receive_time, &send_time);

						latency = diff_time.tv_sec * 1000 + diff_time.tv_usec / 1000;	
						latency_set.push_back(latency);

						avg_latency = get_average(latency_set);
						min_latency = _min(min_latency, latency);
						max_latency = _max(max_latency, latency);											
						
						++n_response_received;
						printf("[LOG]\t(%d/%d) transactions completed, Response time (miliseconds): min / avg / max: %.3lf, %.3lf, %.3lf\n", 
												n_response_received, total_requests, min_latency, avg_latency, max_latency);

						delete received_message;
						delete[] buffer;
					}
				}
			}
		}		
		
	}
	
}

void* perf_test_thread(void * arg)
{
	int *thread_args = (int*)arg;
	int my_req_per_second = thread_args[0];
	int duration = thread_args[1];
	int thread_id = thread_args[2];

	printf("[Worker thread %d]\tStarting ...\n", thread_id);
	printf("[Worker thread %d]\tRequests per second = %d, Duration = %ds\n", thread_id, my_req_per_second, duration);

	int requests_to_complete = my_req_per_second * duration;
	printf("[Worker thread %d]\tPerforming %d requests in %d seconds\n", thread_id, requests_to_complete, duration);

	double t = 1000.0 * (1.0 / (double)my_req_per_second);
	double delta = 0;
	int n_completed = 0;

	int usleep_interval = (int)(t) - delta;
	while(n_completed < requests_to_complete)
	{
		delta = perform_client_io(thread_id, n_completed);
		++n_completed;
		
//		if(n_completed % 50 == 0)
//			printf("[Worker thread %d]\t Completed (%d/%d) requests\n", thread_id, n_completed, requests_to_complete);

		usleep_interval = (int)t - delta;
		usleep(usleep_interval);
	}
	return 0;
}

void read_configuration(const char* conf_file_name)
{
	printf("Reading configuration from %s\n", conf_file_name);
	char *key_token, *value_token;
	char buffer[MAX_BUFFER_LEN];
	FILE *fp = fopen(conf_file_name, "r");
	
	if(!fp) perror("cannot open configuration file");

	while(fgets(buffer, MAX_BUFFER_LEN, fp))
	{
		//printf("[DEBUG]\t line = %s, line length = %d\n", buffer, strlen(buffer));
		key_token = strtok(buffer, "=\n\r");
		value_token = strtok(NULL, "=\n\r");

		if(strcmp(key_token, "req_per_second") == 0)
			requests_per_second = atoi(value_token);

		else if(strcmp(key_token, "home_agent_name") == 0)
			home_agent_name = value_token;

		else if(strcmp(key_token, "home_agent_port") == 0)
			home_agent_port = atoi(value_token);

		else if(strcmp(key_token, "n_threads")==0)
		{
			n_threads = atoi(value_token);
			n_threads = _min(n_threads, MAX_WORKER_THREADS);
		}
		else if(strcmp(key_token, "duration")==0)
			experiment_duration = atoi(value_token);
		
		else if(strcmp(key_token, "listen_port")==0)
			listen_port = atoi(value_token);

		else if(strcmp(key_token, "query_file") == 0)
			query_file = value_token;
	}

	fclose(fp);
}

void load_query_data(const char* query_file_name, int total_rows)
{
	printf("Loading query data from %s\n", query_file_name);
	FILE *fp = fopen(query_file_name, "r");
	char buffer[MAX_BUFFER_LEN];
	int n_rows = 0;
	int rows_per_thread = total_rows / n_threads;
	int buf_end_idx;

	while(fgets(buffer, MAX_BUFFER_LEN, fp) && n_rows < total_rows)
	{
		buf_end_idx = (int)strlen(buffer) - 1;	
		buffer[buf_end_idx] = '\0';	
		query[n_rows / rows_per_thread].push_back(buffer);
		++n_rows;
	}

	fclose(fp);
}

int main(int argc, char *argv[])
{
	puts("Starting home agent performance benchmark");

	if(argc > 1)
	{
		if(strcmp(argv[1], "-c") == 0)
			config_file_name = argv[2];
	}
	
	
	read_configuration(config_file_name.c_str());
	printf("Successfully read configuration\n\tRequests per second = %d\n\tHome Agent Name = %s\tNo. of threads = %d\n\tBenchmark Duration = %ds\n",
							requests_per_second, home_agent_name.c_str(), n_threads, experiment_duration);

	load_query_data(query_file.c_str(), experiment_duration * requests_per_second);
	initialize_listen_socket();
	min_latency = 86400000.0;
	max_latency = 0.0;
	avg_latency = 0.0;

	int i;
	int *thread_args;
	int total_requests = requests_per_second * experiment_duration;
	int req_per_thread = requests_per_second / n_threads;

	pthread_t *worker_threads = new pthread_t[n_threads];
	pthread_t listener;

	pthread_create(&listener, NULL, listener_thread, NULL);

	for(i = 0; i < n_threads; i++)
	{
		thread_args = new int[3];
		thread_args[0] = req_per_thread;
		thread_args[0] += (i != n_threads - 1) ? 0 : (requests_per_second % n_threads);
		thread_args[1] = experiment_duration;
		thread_args[2] = i;
		pthread_create(&worker_threads[i], NULL, perf_test_thread, thread_args);
	}

	for(i = 0; i < n_threads; i++)
		pthread_join(worker_threads[i], NULL);

	pthread_join(listener, NULL);
	
	//printf("[DEBUG]: latency set = <");
	//for(i = 0; i < latency_set.size(); i++) printf(" %d", latency_set[i]);
	//printf(">\n");
	printf("Response time (miliseconds): min / avg / max: %.3lf, %.3lf, %.3lf\n", min_latency, avg_latency, max_latency);
	return 0;
}
