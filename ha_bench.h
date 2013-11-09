#ifndef HA_BENCH_H
#define HA_BENCH_H

#include <sys/time.h>
#include <string>

using namespace std;

long	perform_client_io(int, int);
void*	perf_test_thread( void* );
void*	listener_thread( void* );
int	timeval_substract(struct timeval*, struct timeval*, struct timeval*);
void	read_configuration(const char*);
void 	load_query_data(const char*, int);

int	initialize_listen_socket();

void add_unresolved_request(string, timeval);
timeval remove_front_unresolved_requests(string);

#endif
