#ifndef UTILS_H
#define UTILS_H

#define MAX_NAME_LENGHT 100
#define BUFF_SIZE 150
#define SERVER_NAME "[OBJSTORE]"
#define DATA_FOLDER "data"
#define MASK 0777
#define N 108
#define UNIX_PATH_MAX 108
#define SOCKNAME "objstore.sock"
#define TRUE 1
#define TIMEOUT_SEC 50
#define OK "OK \n"

//Overview: Modulo di supporto

/*
     @REQUIRES: fdc != null && buf != null && size != null
     @EFFECTS: Readn standard
*/
extern int read_store_retrieve(long fdc, void *buf, size_t size);

/*
     @REQUIRES: fdc != null && buf != null && size != null
     @EFFECTS: Readn modificata per terminare la lettura al carattere '\n'
*/
extern int readn(long fd, void *buf, size_t size);


/*
     @REQUIRES: fdc != null && buf != null && size != null
     @EFFECTS: writen standard
*/
extern int writen(long fd, void *buf, size_t size);

/*
     @REQUIRES: msg != null 
     @EFFECTS: Stampa gli errori lato client
*/
extern void print_error(char *msg);




#endif /* UTILS_H */
