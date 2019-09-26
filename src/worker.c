#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //api posix   
#include <sys/wait.h>  
#include <sys/types.h> 
#include <sys/socket.h> 
#include <errno.h> 
#include <sys/un.h> //dominio AF_UNIX
#include <pthread.h> 
#include <sys/poll.h>
#include <string.h>

#include "worker.h"
#include "server.h"
#include "handler.h"



static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


worker *workerlist= NULL;

worker* create(int fd)
{
    pthread_mutex_lock(&mutex);
    
    server->connected_clients++;
    worker* new_node_worker = (worker*)malloc(sizeof(worker));
    
    
    //Inserimento in testa
    new_node_worker->fd_worker = fd;
    new_node_worker->online = 1;
    new_node_worker->signed_up = 0;
    new_node_worker->next = workerlist;
    new_node_worker->previus = NULL;
    sprintf(new_node_worker->name, "%s", "Not_Set");
    if (workerlist != NULL)
        workerlist->previus=new_node_worker;
    workerlist=new_node_worker;
    pthread_mutex_unlock(&mutex);
 
    return new_node_worker;
}

worker* create_worker(int fd)
{
    worker *newworker=create(fd);
    if(pthread_create(&newworker->thread_id,NULL,workerloop,newworker) != 0)
    {
        fprintf(stderr, SERVER_NAME " Err: pthread_create non riuscuta \n");

    }
    /* libera risorse al termine */
    pthread_detach(newworker->thread_id); 
    return newworker;
}


void *workerloop(void *args)
{
    char buf[BUFF_SIZE + 1]; 
    
    //Worker
    worker *selectedworker = args;
    int fd_c = selectedworker->fd_worker;
    
    //POLL
    struct pollfd pfd;
    pfd.fd = fd_c;
    pfd.events =POLLIN;
    
    while (server->is_running && selectedworker->online) 
    {
        fprintf (stdout,SERVER_NAME " #%d: server \n",server->is_running);
        memset (buf,0,sizeof(buf));
       
        if(poll(&pfd, 1, 10) >= 1) {
           int nread = readn(fd_c, buf, BUFF_SIZE);
   
           if (nread<=0)
           {
            // EOF client  
             fprintf (stdout,SERVER_NAME " #%d: EOF client\n",getpid());
             fprintf(stdout,SERVER_NAME " #%d: Chiusura FD #%d\n",getpid(),fd_c);
             break;
           }
           else if(handle_user_msg(buf,nread,selectedworker) <= 0) 
           { 
               break;
           }
          
        } 
    }
    //stampa operazione
    fprintf (stdout,SERVER_NAME " #%d: ## Client %s con fd:%d thread in chiusura ##\n",
    server->is_running ,selectedworker->name,selectedworker->fd_worker);
    
    close(fd_c);
    close_client(selectedworker);
    pthread_exit(NULL);
}



worker *find_user(char *name)
{
    pthread_mutex_lock(&mutex);
    worker *core = workerlist;
    while (core != NULL)
    {
        if (strcmp(core->name,name) == 0)
            return core;
        core=core->next;
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int close_client(worker *client_worker)
{
    pthread_mutex_lock(&mutex);
    
    client_worker->online=0;
    server->connected_clients--;
   
    if(client_worker->previus== NULL)
    {
        if (client_worker->next != NULL)
            client_worker->next->previus=NULL;
        workerlist = client_worker->next;
    }
    else
    {
        if (client_worker->next != NULL)
            client_worker->next->previus=client_worker->previus;
        client_worker->previus->next=client_worker->next;
   
    }
    pthread_mutex_unlock(&mutex);
    free(client_worker);
    return 1;
}

