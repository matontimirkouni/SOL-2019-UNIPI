/*  POSIX.1-2008 required for ftw */
#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //api posix   
#include <sys/wait.h>  
#include <sys/types.h> 
#include <sys/socket.h> 
#include <sys/stat.h> 
#include <errno.h> 
#include <sys/un.h> //dominio AF_UNIX
#include <pthread.h> 
#include <sys/poll.h>
#include <ftw.h>
#include <time.h> 
#include <string.h>
#include "worker.h"
#include "server.h"
#include "utils.h"
#include "signal_thread.h"


#define USE_FDS 15 //max fd

/* socket di connessione */
int fd_sk;


int main()
{
  server = (server_info *) malloc(sizeof(server_info));
  server->connected_clients=0;
  server->is_running=0;

  
  if (mkdir(DATA_FOLDER, MASK) != 0)
  {
    perror(SERVER_NAME " Err");
  }
     
  /* Riempio la struttura per l'indirizzamento locale */
  memset (&server->sa,0,sizeof(server->sa));
  sprintf(server->sa.sun_path,"%s",SOCKNAME);
  server->sa.sun_family = AF_UNIX;

  /* Cancello eventuale collegamento precedente */
  unlink(SOCKNAME);
  
  /* Apro il socket */
  if ((fd_sk = socket(AF_UNIX,SOCK_STREAM,0))<0)
  {
    perror(SERVER_NAME" Err socket");
    exit (EXIT_FAILURE);
  }
   
  /* Assegno l'indirizzo al socket */
  if (bind(fd_sk,(struct sockaddr *)&server->sa, sizeof(server->sa))<0)
  {
    perror(SERVER_NAME " Err bind");
    exit (EXIT_FAILURE); 
  }
  
  /* Metto in ascolto il socket */
  if (listen(fd_sk,SOMAXCONN)<0)
  {
    perror(SERVER_NAME " Err listen");
    exit (EXIT_FAILURE); 
  }
  
  /* thred gestore dei segnali*/
  server->signal_thread=create_signal_thread(server);



  server->is_running=1;
  fprintf (stdout,SERVER_NAME " #%d: ## in attesa con FD %d ##\n",getpid(),fd_sk);
  
  /* poll */
  server->poll_fd.fd = fd_sk;
  server->poll_fd.events = POLLIN; 	//Clausola per attesa ricezione su fd
  

  
  while(server->is_running)
  {
    if(poll(&server->poll_fd, 1, 10) >= 1) {

			int client_fd;
			client_fd = accept(fd_sk,NULL,NULL);
			
			/* nuovo client */
			if(client_fd > 0) {
			  fprintf (stdout,SERVER_NAME " #%d: nuova connessione con fd #%d\n",getpid(),client_fd);
				create_worker(client_fd);
			}
		}
  }
  
  close(fd_sk);
  fprintf (stdout,SERVER_NAME " #%d: ## Server in chiusura ##\n",getpid());
  
  /* attesa per le operazioni in sospeso dei client */
  while(server->connected_clients > 0)
  {
    fprintf (stdout,SERVER_NAME " #%d: ## in attesa operazioni pendenti ##\n",getpid());
  }
  
  free(server);
  return 0;
}


int print_item(const char *filepath, const struct stat *info,const int typeflag, struct FTW *pathinfo)
{
  off_t bytes =  info->st_size; 
  if (typeflag == FTW_F)
  {
      server->n_object++;
      /* Conversione in kb */
      server->store_size+=bytes/1024;
  }
  return 0;
}

                
void print_server_info()
{
  int rs;
  server->store_size=0;
  server->n_object=0;
  
  /* chiamata alla nftw */
  rs = nftw(DATA_FOLDER, print_item, USE_FDS, FTW_PHYS);
  
  if (rs == -1)
    {
        errno = rs;
        perror(SERVER_NAME " Err nftw");
        exit(EXIT_FAILURE);
    }
  
  fprintf (stdout,SERVER_NAME " #%d: ## Stampa info ##\n",getpid());
  fprintf (stdout,SERVER_NAME " #%d: ## Client connessi: %d ##\n",getpid(),server->connected_clients);
  fprintf (stdout,SERVER_NAME " #%d: ## N. oggetti presenti: %d ##\n",getpid(),server->n_object);
 
  /* mera questione grafica */
  if(server->store_size >=1024)
     fprintf (stdout,SERVER_NAME " #%d: ## Size dello store: %Lf Mb ##\n",getpid(),server->store_size/1024);
  else
    fprintf (stdout,SERVER_NAME " #%d: ## Size dello store: %Lf Kb ##\n",getpid(),server->store_size);
}
