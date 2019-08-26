#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  //api posix   
#include <sys/wait.h>  
#include <sys/types.h> 
#include <sys/socket.h> 
#include <sys/stat.h>
#include <sys/un.h> //dominio AF_UNIX
#include <string.h>
#include <errno.h>
#include <fcntl.h> // for open
#include "utils.h"



int fd_skt=-1;

int read_response();

int os_connect(char *name)
{
    if (name == NULL)
    {
        print_error("Invalid argument");
        return 0;
    }
    
    if (fd_skt != -1)
    {
        print_error("Socket still opened");
        return 0;
    }
    
    //sockaddr_un per binding
    struct sockaddr_un sa;
    sprintf(sa.sun_path,"%s",SOCKNAME); // il pathname del socket.
    sa.sun_family=AF_UNIX;
    
    // Apro il socket 
    if ((fd_skt = socket(AF_UNIX,SOCK_STREAM,0))<0)
       { 
           print_error(NULL);
           return 0;
       }
    
    
    while (connect(fd_skt,(struct sockaddr*)&sa,sizeof(sa)) == -1 ) 
    {
        if ( errno == ENOENT )
            sleep(1); /* sock non esiste */
        else 
            return 0;
    }
    
    //Invio richiesta
    char buffer[BUFF_SIZE];
    sprintf(buffer, "%s %s \n", "REGISTER", name);
    if (writen(fd_skt,buffer,strlen(buffer)) == -1) 
    {
        print_error(NULL);
        fd_skt=-1;
        return 0;
    }
    
    
    //Attesa risposta
    char server_response[BUFF_SIZE];
    int rd;
    //Attesa risposta
    if ((rd=readn(fd_skt,server_response,BUFF_SIZE)) == -1) 
    {
        print_error(NULL);
        return 0;
    }
    
    //Check risposta
 	char* last = NULL;
	char* header = strtok_r(server_response, " ", &last);

	
    if (strcmp("OK",header) == 0)
        return 1;
    
    if (strcmp("KO",header) == 0)
    {
        print_error(last);
        fd_skt=-1;
        close(fd_skt);
    }
   
    return 0;
    
}

int os_store(char *name, void *block, size_t len)
{
    if (name == NULL || block == NULL )
    {
        print_error("Invalid argument");
        return 0;
    }
    
    if (fd_skt == -1)
    {
        print_error("Socket not opened");
        return 0;
    }
    
    
    //Invio richiesta
    char *buffer = malloc((BUFF_SIZE + len+1) * sizeof(char));
    sprintf(buffer, "%s %s %ld \n ", "STORE", name,len);


    ssize_t msglen =strlen(buffer);
    if (writen(fd_skt,buffer,msglen) == -1) 
    {
        print_error(NULL);
        return 0;
    }
    
    if (writen(fd_skt,(char*) block , len) == -1) 
    {
        print_error(NULL);
        return 0;
    }

    //Attesa risposta
    char server_response[BUFF_SIZE];
    int rd;

    if ((rd=readn(fd_skt,server_response,BUFF_SIZE)) == -1) 
    {
        print_error(NULL);
        return 0;
    }
    
    //Check risposta
 	char* last = NULL;
	char* header = strtok_r(server_response, " ", &last);
	
	free(buffer);
    if (strcmp("OK",header) == 0)
        return 1;
    
    if (strcmp("KO",header) == 0)
        print_error(last);
    
    return 0;
}

void *os_retrieve(char *name)
{
    if (name == NULL)
    {
        print_error("Invalid argument");
        return 0;
    }
    
    if (fd_skt == -1)
    {
        print_error("Socket not opened");
        return 0;
    }
    
    void *data = NULL;
    
    char buffer[BUFF_SIZE];
    sprintf(buffer, "%s %s \n", "RETRIEVE", name);
    if (writen(fd_skt,buffer,strlen(buffer)) == -1) 
    {
        print_error(NULL);
        return 0;
    }
    
    //Attesa risposta
    int rd;
    
    char server_response[BUFF_SIZE];
    
    if ((rd=readn(fd_skt,server_response,BUFF_SIZE)) == -1) 
    {
        print_error(NULL);
        return 0;
    }
   
	
    char *last = NULL;
	char *token;
	char *header = strtok_r(server_response, " ", &last);
	char *size = token = strtok_r(NULL, " ", &last);
	token = strtok_r(NULL, " ", &last);


	if (strcmp("DATA",header) == 0)
	{

        char *check;
        size_t file_size = strtol(size, &check, 10);

        data = (char *) malloc(sizeof(char) * (file_size+1));
        
        size_t letti = rd-(4+ strlen(header)+strlen(size));
        size_t leg= file_size-letti;
        if(letti == 0)
        {
            if (read_store_retrieve(fd_skt,data,file_size)== -1)
            {
                print_error(NULL);
                return 0;
            }
            
        }
        else
        {
            if (leg > 0)
            {
                char *data2 = (char *) malloc(sizeof(char) * (leg+1));
                if (read_store_retrieve(fd_skt,data2,leg)== -1)
                    {
                        print_error(NULL);
                        return 0;
                    }
                memcpy(data, last, sizeof(char) * letti);
                memcpy((char *)data+(sizeof(char) * letti), data2, sizeof(char) * leg);
                free(data2);
                
            }
            else
            {
                memcpy(data, last, sizeof(char) * letti);
            }
        }
         
	}
    
    if (strcmp("KO",header) == 0)
        print_error(last);
    
    return data;
    
    
}

int os_delete(char *name)
{
    if (name == NULL)
    {
        print_error("Invalid argument");
        return 0;
    }
    
    if (fd_skt == -1)
    {
        print_error("Socket not opened");
        return 0;
    }
    
    //Invio richiesta
    char buffer[BUFF_SIZE];
    sprintf(buffer, "%s %s \n", "DELETE", name);
   
    if (writen(fd_skt,buffer,strlen(buffer)) == -1) 
    {
        print_error(NULL);
        return 0;
    }
    
    //Attesa risposta
    char server_response[BUFF_SIZE];
    int rd;
    //Attesa risposta
    if ((rd=readn(fd_skt,server_response,BUFF_SIZE)) == -1) 
    {
        print_error(NULL);
        return 0;
    }
    
    //Check risposta
 	char* last = NULL;
	char* header = strtok_r(server_response, " ", &last);
	
    if (strcmp("OK",header) == 0)
        return 1;
    
    if (strcmp("KO",header) == 0)
        print_error(last);
    
    return 0;
}

int os_disconnect()
{
    if (fd_skt == -1)
    {
        print_error("Socket not opened");
        return 0;
    }
    
    //Invio richiesta al server
    char buffer[BUFF_SIZE];
    sprintf(buffer, "%s \n", "LEAVE");
    if (writen(fd_skt,buffer,strlen(buffer)) == -1) 
    {
        print_error(NULL);
        return 0;
    }
    
     //Attesa risposta
    char server_response[BUFF_SIZE];
    int rd;
    //Attesa risposta
    if ((rd=readn(fd_skt,server_response,BUFF_SIZE)) == -1) 
    {
        print_error(NULL);
        return 0;
    }
    
    
    close(fd_skt);
    fd_skt=-1;
    return 1;
}
