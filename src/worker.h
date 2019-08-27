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


/*
     @EFFECTS: Crea il thread worker associato ad un client
*/
extern worker* create_worker(int fd);

/*
     @EFFECTS: Loop del thread worker
*/
extern void *workerloop(void *arg);

/*
     @EFFECTS: Trova un client nella lista degli utenti attivi
*/
extern worker *find_user(char *name);

/*
     @EFFECTS: Stampa le informazioni correnti del server
*/
extern int close_client(worker *client_worker);


#endif /* WORKER_H */
