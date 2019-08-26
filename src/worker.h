#ifndef WORKER_H
#define WORKER_H

#include "utils.h"


typedef struct worker {
    struct worker *next;
    struct worker *previus;
    int fd_worker;
    int signed_up;
    int online;
    char name[MAX_NAME_LENGHT + 1];
    pthread_t thread_id;
}worker;


//Lista worker
worker *workerlist;


extern worker* create_worker(int fd);

extern void *workerloop(void *arg);

extern worker *find_user(char *name);

extern int close_client(worker *client_worker);


#endif /* WORKER_H */
