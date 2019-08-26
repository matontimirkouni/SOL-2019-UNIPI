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



extern int read_store_retrieve(long fdc, void *buf, size_t size);

extern int readn(long fd, void *buf, size_t size);

extern int writen(long fd, void *buf, size_t size);

extern void print_error(char *msg);




#endif /* UTILS_H */
