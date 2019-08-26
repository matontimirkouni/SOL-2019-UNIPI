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
#include "handler.h"
#include "server.h"


int readn(long fd, void *buf, size_t size) {
    size_t left = size;
    int r;
    char *bufptr = (char*)buf;
    while(left>0)
    {
    	if ((r=read((int)fd ,bufptr,left)) == -1)
    	{
    	    if (errno == EINTR) 
    	        continue;
    	    return -1;
    	}
    	if (r == 0) return 0;   
            left-= r;
        
        char *lst = strstr(buf,"\n");
        if (lst != NULL)
        {
            return r; 
        }
    	bufptr  += r;
    }
    return size;
}

int read_store_retrieve(long fd, void *buf, size_t size) {
    size_t left = size;
    int r;
    char *bufptr = (char*)buf;
 
    while(left>0)
    {
    	if ((r=read((int)fd ,bufptr,left)) == -1)
    	{
    	    if (errno == EINTR) 
    	        continue;
    	    return -1;
    	}
    	if (r == 0) return 0;   
            left-= r;
        
    	bufptr  += r;
    }
    return size;
}

int writen(long fd, void *buf, size_t size) {
    size_t left = size;
    int r;
    char *bufptr = (char*)buf;
    while(left>0) {
	if ((r=write((int)fd ,bufptr,left)) == -1) {
	    if (errno == EINTR) continue;
	    return -1;
	}
	if (r == 0) return 0;  
        left    -= r;
	bufptr  += r;
    }
    return r;
}



void print_error(char *msg)
{
    if (msg == NULL)
        fprintf(stderr, SERVER_NAME" Err: %s\n", strerror(errno));
    else
        fprintf(stderr, SERVER_NAME" Err: %s\n",msg);
}

