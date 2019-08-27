#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //api posix   
#include <sys/wait.h>  
#include <sys/types.h> 
#include <sys/socket.h> 
#include <sys/stat.h>
#include <sys/un.h> //dominio AF_UNIX
#include <string.h>
#include <pthread.h> 
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include "utils.h"
#include "server.h"


sigset_t set; int sig;


void *loop_signal()
{
    fprintf (stdout,SERVER_NAME " #%d: ## signal thread partito ##\n",getpid());
    /* costruisco la maschera */
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
	sigaddset(&set, SIGTERM);
	sigaddset(&set, SIGUSR1);

    /* blocco */
    sigprocmask(SIG_BLOCK, &set, NULL);
  
    while(server->is_running)
    {
        sigwait(&set,&sig);
        if (sig == 2) //SIGINT
        {   
            fprintf(stdout,SERVER_NAME" #%d: ## SIGINT ricevuto  ##\n",getpid());
            server->is_running=0;
        }
        else if (sig == 15) //SIGTERM
        {
            fprintf(stdout,SERVER_NAME" #%d: ## SIGTERM ricevuto  ##\n",getpid());
            server->is_running=0;
        }
        else if (sig == 10) // SIGUSR1
        {
            print_server_info();
            /* riattesa nuovo segnale */
            continue;
           
        }
        else
        {
            fprintf(stdout,SERVER_NAME" #%d: ## Altro segnale ricevuto  ##\n",getpid());
            server->is_running=0;
        }
        
    }

    return NULL;
}


pthread_t create_signal_thread(server_info *server)
{
    sigset_t set;
	sigfillset(&set);	
    /* blocco tutti i segnali prima dell'avvio del signal handler  */
    
    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1)
        perror(SERVER_NAME "Err: signal_set");
    
    
    /* signal handler thread */
    pthread_t tid;
    if(pthread_create(&tid,NULL,loop_signal,NULL) != 0)
    {
        fprintf(stderr, SERVER_NAME " Err: pthread_create non riuscuta \n");

    }
    pthread_detach(tid); 
    return tid;
}

