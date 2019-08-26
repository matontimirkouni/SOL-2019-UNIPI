#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //api posix   
#include <sys/wait.h>  
#include <sys/types.h> 
#include <sys/socket.h> 
#include <sys/stat.h>
#include <sys/un.h> //dominio AF_UNIX
#include <pthread.h> 
#include <string.h>
#include <errno.h>
#include <fcntl.h> // for open
#include "handler.h"
#include "server.h"
#include "utils.h"


int ok_fun(worker *worker_client)
{
       return writen(worker_client->fd_worker,OK,strlen(OK));
}

int ko_fun(char *msg, worker *worker_client)
{
        char buf[BUFF_SIZE + 1]; 
        sprintf(buf, "%s %s \n", KO, msg);
        fprintf(stdout,SERVER_NAME " [KO]  #%s: FD #%d\n",msg,worker_client->fd_worker);
        return writen(worker_client->fd_worker,buf,strlen(buf));
}


int handle_user_msg(char *msg,int bytes, worker *worker_client)
{
    if (msg == NULL || worker_client == NULL)
        return -1;

    char cmd_array[4][100];
    int i=0;
    char *lst = NULL;
	char *token;

	for (token = strtok_r(msg, " ", &lst);token != NULL;token = strtok_r(NULL, " ", &lst)) 
	{   
        if(strcmp(token,"\n") == 0)
            break;
        sprintf(cmd_array[i], "%s",token);
        i++;
    }

	char *header = cmd_array[0];
	char *name = cmd_array[1];
	char *size = cmd_array[2];

   
    
	if(strcmp(header, "REGISTER") == 0) 
	    return handle_register(name,worker_client);
	
	else if (strcmp(header, "STORE") == 0)
	{
	    bytes -=  (5+strlen(header)+strlen(name)+strlen(size));
	    return handle_store(name,size,worker_client,lst,bytes);
	}
	else if (strcmp(header, "RETRIEVE")== 0) 
	    return handle_retrieve(name,worker_client);
	
	else if (strcmp(header, "DELETE")== 0) 
	    return handle_delete(name,worker_client);

	else if (strcmp(header, "LEAVE")== 0) 
	    return handle_leave(worker_client);
	else
	    return ko_fun("Command not found", worker_client);
	    

	return ko_fun("Command not found", worker_client);
	
}

int handle_register (char *name,worker *worker_client)
{
    if (name == NULL)
        return ko_fun("Name parameter null", worker_client);
    
    if (worker_client == NULL)
        return ko_fun("Worker parameter null", worker_client);
    
    if (find_user(name) != NULL)
        return ko_fun("Name already taken", worker_client);
           
    if (worker_client->signed_up)
        return ko_fun("User has already signed up", worker_client);
    
    //Creazione directory utente
    char user_path[UNIX_PATH_MAX];
    sprintf(user_path, "%s/%s",DATA_FOLDER,name);
    mkdir(user_path,MASK);

    //Set Client name
    worker_client->signed_up=1;
    sprintf(worker_client->name, "%s", name);

    return ok_fun(worker_client);
}




int handle_store(char *name,char *size,worker *worker_client,char *lst,size_t bytes)
{

    if (worker_client == NULL)
        return ko_fun("Worker parameter null", worker_client);
    
    if (name == NULL)
        return ko_fun("Name parameter null", worker_client);
    
    if (size == NULL)
        return ko_fun("Size parameter null", worker_client);
     
    if (!worker_client->signed_up)
        return ko_fun("User hasn't signed up", worker_client);    
        
    //Path di salvataggio file
    char path[UNIX_PATH_MAX];
    sprintf(path,"%s/%s/%s",DATA_FOLDER,worker_client->name,name);
    
    //Conversione char -> int
    char *check;
    long int file_size = strtol(size, &check, 10);  
    
        
    //Buffer di lettura    
    char *buffer_read = (char *)malloc((file_size+1) * sizeof(char));
    

    //file da salvare
    int fd1;
    if((fd1 = open(path,O_CREAT|O_RDWR,0777)) == -1 )
    {
        perror(SERVER_NAME "Err opening file"); 
        char msg_error[BUFF_SIZE];
        strerror_r(errno, msg_error, BUFF_SIZE);
        return ko_fun(msg_error, worker_client);
        
    }


    if (strlen(lst) == 0)
    {
        /* readn modificata*/
        if(read_store_retrieve(worker_client->fd_worker, buffer_read, file_size) == -1)
        {
            perror(SERVER_NAME "Err reading socket"); 
            char msg_error[BUFF_SIZE];
            strerror_r(errno, msg_error, BUFF_SIZE);
            return ko_fun(msg_error, worker_client);
        }
    
        
        if (writen(fd1,buffer_read,file_size) == -1) 
        { 
            perror(SERVER_NAME "Err writing file"); 
            char msg_error[BUFF_SIZE];
            strerror_r(errno, msg_error, BUFF_SIZE);
            return ko_fun(msg_error, worker_client);
            
        }
    }
    else
    {
        /* byte da leggere rimasti come dimensione passata - byte letti da read*/    
        size_t leg= file_size-bytes;
        /* readn modificata*/
        if(read_store_retrieve(worker_client->fd_worker, buffer_read, leg) == -1)
        {
            perror(SERVER_NAME "Err reading socket"); 
            char msg_error[BUFF_SIZE];
            strerror_r(errno, msg_error, BUFF_SIZE);
            return ko_fun(msg_error, worker_client);
        }
        
        char *final = (char *)malloc((file_size+1+bytes) * sizeof(char));
        
        /* unione dei buffer in caso parte dei dati sia presente 
        nella precedente read */
        memcpy(final, lst, sizeof(char) * bytes);
        memcpy(final+(sizeof(char) * bytes), buffer_read, sizeof(char) * leg);
        
        if (writen(fd1,final,file_size) == -1) 
        { 
            perror(SERVER_NAME "Err writing file"); 
            char msg_error[BUFF_SIZE];
            strerror_r(errno, msg_error, BUFF_SIZE);
            return ko_fun(msg_error, worker_client);
            
        }
        free(final);
     
    }
    
    //chiusura file
    close(fd1);
    //free buffer lettura
    free(buffer_read);
    
    //stampa operazione
    fprintf (stdout,SERVER_NAME " #%d: ## Client %s con fd:%d esegue store ##\n",
    getpid(),worker_client->name,worker_client->fd_worker);
    
    return ok_fun(worker_client);
}



int handle_retrieve(char *name,worker *worker_client)
{
    if (name == NULL)
        return ko_fun("Name parameter null", worker_client);
    
    if (worker_client == NULL)
        return ko_fun("Worker parameter null", worker_client);
    
    if (!worker_client->signed_up)
        return ko_fun("User hasn't signed up", worker_client);
    
    char path[UNIX_PATH_MAX];
    sprintf(path,"%s/%s/%s",DATA_FOLDER,worker_client->name,name);
    
    int fd;

    if((fd=open(path,O_RDONLY)) == -1)
    {
        perror(SERVER_NAME "Err reading file"); 
        char msg_error[BUFF_SIZE];
        strerror_r(errno, msg_error, BUFF_SIZE);
        return ko_fun(msg_error, worker_client);
    }
    
    
    //FileSize
    struct stat info;
    if (stat (path, &info) == -1)
    {
        perror(SERVER_NAME "Err reading file"); 
        char msg_error[BUFF_SIZE];
        strerror_r(errno, msg_error, BUFF_SIZE);
        return ko_fun(msg_error, worker_client);    
    }
    
    char *file_buf = malloc(((long long) info.st_size )* sizeof(char));

    char buffer_header[BUFF_SIZE];
    
    /* read file nello store*/
    int rd;
    if((rd=read(fd,file_buf,(long long) info.st_size)) == -1)
    {
        perror(SERVER_NAME "Err reading file"); 
        char msg_error[BUFF_SIZE];
        strerror_r(errno, msg_error, BUFF_SIZE);
        return ko_fun(msg_error, worker_client);
    }
    
    /* header di risposta */
    sprintf(buffer_header, "DATA %lld \n ",(long long) info.st_size);
    if (writen(worker_client->fd_worker,buffer_header,strlen(buffer_header)) == -1) 
    {
        perror(SERVER_NAME "Err socket write"); 
        char msg_error[BUFF_SIZE];
        strerror_r(errno, msg_error, BUFF_SIZE);
        return ko_fun(msg_error, worker_client);
    }
    
    /* invio dati file */
    if (writen(worker_client->fd_worker,file_buf,info.st_size) == -1) 
    {
        perror(SERVER_NAME "Err socket write"); 
        char msg_error[BUFF_SIZE];
        strerror_r(errno, msg_error, BUFF_SIZE);
        return ko_fun(msg_error, worker_client);
    }
    
    //stampa operazione
    fprintf (stdout,SERVER_NAME " #%d: ## Client %s con fd:%d esegue retrieve ##\n",
    getpid(),worker_client->name,worker_client->fd_worker);
    
    close(fd);
    free(file_buf);
    
    return 1;
}

int handle_delete(char *name,worker *worker_client)
{
    if (name == NULL)
        return ko_fun("Name parameter null", worker_client);
    
    if (worker_client == NULL)
        return ko_fun("Worker parameter null", worker_client);
    
    if (!worker_client->signed_up)
        return ko_fun("User hasn't signed up", worker_client);
    
    char path[UNIX_PATH_MAX];
    sprintf(path,"%s/%s/%s",DATA_FOLDER,worker_client->name,name);
    if (unlink(path) != 0)
    {
        perror("unlink() error");
        char msg_error[BUFF_SIZE];
        strerror_r(errno, msg_error, BUFF_SIZE);
        return ko_fun(msg_error, worker_client);
    }
    
    //stampa operazione
    fprintf (stdout,SERVER_NAME " #%d: ## Client %s con fd:%d esegue delete ##\n",
    getpid(),worker_client->name,worker_client->fd_worker);
        
    return ok_fun(worker_client);
}


int handle_leave(worker *worker_client)
{
    
    if (worker_client == NULL)
        return ko_fun("Worker parameter null", worker_client);
    
    worker_client->online=0;
    //stampa operazione
    fprintf (stdout,SERVER_NAME " #%d: ## Client %s con fd:%d leave ##\n",
    getpid(),worker_client->name,worker_client->fd_worker);
    return ok_fun(worker_client);
}



