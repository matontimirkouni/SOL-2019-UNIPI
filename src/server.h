#ifndef SERVER_H
#define SERVER_H
#include <sys/poll.h>
#include <pthread.h> 

//Overview: ObjectStore

typedef struct server_info {
    volatile _Atomic connected_clients;
    long double store_size;
    int n_object;
    struct sockaddr_un sa; //socket
    struct pollfd poll_fd; //poll
    volatile sig_atomic_t is_running;
    pthread_t signal_thread;
}server_info;



server_info *server;

/*
     @EFFECTS: Stampa le informazioni correnti del server
*/
extern void print_server_info();

#endif /* SERVER_H */
