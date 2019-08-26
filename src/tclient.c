#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>  //api posix   
#include <sys/wait.h>  
#include <sys/types.h> 
#include <sys/socket.h> 
#include <errno.h> 
#include <sys/un.h> //dominio AF_UNIX
#define UNIX_PATH_MAX 108
#define SOCKNAME "objstore.sock"
#include "libobjectstore.h"
#include <fcntl.h> // for open
#include <sys/stat.h>
#define N 100

#define file_s "VS"
	

int main()
{
    char msg[100];
    int command=-1;
    /* file */
    struct stat info;
    
    printf("Client interattivo\n");
    printf("1 - Connect\n2 - Store\n3 - Retrieve\n");
    printf("4 - Delete\n5 - Leave\n");
    while(1)
    {
        scanf("%d",&command);
      
        
        if(command == 1) //Connect
        {
            scanf("%s",msg);
            int rd = os_connect(msg);
            if (rd) printf("Connesso\n");
            command=-1;
        }
        else if(command == 2) //Store
        {
            char path[UNIX_PATH_MAX];
            scanf("%s",path);
            
            int fd;

            if((fd=open(path,O_RDONLY)) == -1)
            {
                perror("read() error");
                continue;
            }
            
            //File
           
            if (stat (path, &info) == -1){/* gestione errore */}
            char *file_buf = malloc((info.st_size )* sizeof(char));
            
            /* read file */
            read(fd,file_buf, info.st_size);
            
            char *lst = NULL;
	        char *token;
	        char *filename = malloc(sizeof(char) * UNIX_PATH_MAX);
	        for (token = strtok_r(path, "/", &lst);token != NULL;token = strtok_r(NULL, " ", &lst)) 
	        {   
                sprintf(filename, "%s",token);
            }
    
            /* store */
            int rd = os_store(filename,file_buf, info.st_size); 
            if (rd) printf("Invio con successo \n");
            command=-1;
        }
        else if(command == 3) //Retrieve
        {
            scanf("%s",msg);
            char *res = os_retrieve(msg);
            if (res != NULL) printf("Retrieve corretta \n");
            int fd1;
            if((fd1 = open(msg,O_CREAT|O_RDWR,0777)) == -1 )
            {
                perror("error");
                continue;
        
            }
            if (write(fd1,res,info.st_size) == -1) 
            { 
                perror("error"); 
                continue;
            }
            command=-1;
        }
        else if(command == 4) // Delete
        {
            scanf("%s",msg);
            int rd = os_delete(msg);
            if (rd) printf("File eliminato");
            command=-1;
        }
        else if(command == 5) // Leave
        {
            int rd = os_disconnect();
            if (rd) printf("Uscita client\n");
            break;
        }
        
        else
        {
            continue;
        }

       
    }
 
   
    exit(EXIT_SUCCESS); 
    return 0;
}
