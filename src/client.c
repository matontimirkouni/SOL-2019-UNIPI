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
#define MAX_NAME_LENGHT 108
#define SOCKNAME "objstore.sock"
#include "libobjectstore.h"
#include <fcntl.h> // for open
#include <sys/stat.h>

#define N_DATI 20
#define TEST_STRING "Nel mezzo del cammin di nostra vita mi ritrovai per una selva oscura\
ché la diritta via era smarrita"

char *data[N_DATI];
int  op_ok=0;
int  op_ko=0;

char *client_name;



void prep_data()
{

    //dimensione blocco fisso di test
    size_t block_size=strlen(TEST_STRING);
   
    //1*elemento
    data[0]=malloc(block_size*sizeof(char) +1);
    sprintf(data[0],"%s",TEST_STRING);
   
    for(int i=1;i < N_DATI;i++)
    {
        //dimensione percentuale
        size_t size = i * 54 *block_size;
        
        data[i]=malloc(size*sizeof(char) +1);
        memset(data[i], '\0', size);
        
        size_t sk=0;
        //creazione stringa in serie
        for(size_t j = 0; j < size/block_size; j++)
           {
                memcpy(data[i] + sk, TEST_STRING, block_size);
                sk = sk + block_size;
           }
           
        data[i][size] = '\0';
		
    }
    
}

void test_1()
{
    prep_data();
    for(int i=0;i < N_DATI;i++)
    {
        char *name_file=malloc(MAX_NAME_LENGHT*sizeof(char));
        sprintf(name_file,"%s-file-%d",client_name,i);
        size_t file_size=strlen(data[i]);
     
        if(os_store(name_file,data[i],file_size))
            op_ok++;
        else
            op_ko++;
        free(name_file);
    }
 
    for(int i=0;i < N_DATI;i++)
    {
        free(data[i]);
    }
    fprintf(stdout,"1@%d@%d\n",op_ok,op_ko);
}

void test_2()
{
    prep_data();
    for(int i=0;i < N_DATI;i++)
    {
        char *name_file=malloc(MAX_NAME_LENGHT*sizeof(char));
        sprintf(name_file,"%s-file-%d",client_name,i);
        char *file_block= NULL;
      
        if((file_block=os_retrieve(name_file)) != NULL && data[i]!= NULL)
            {
                if(memcmp(data[i],file_block,strlen(data[i]))==0)
                    op_ok++;
                else
                    op_ko++;
            }
        else
            op_ko++;
        free(file_block);
        free(name_file);
    }
    for(int i=0;i < N_DATI;i++)
    {
        free(data[i]);
    }
    fprintf(stdout,"2@%d@%d\n",op_ok,op_ko);
}

void test_3()
{
    for(int i=0;i < N_DATI;i++)
    {
        char *name_file=malloc(MAX_NAME_LENGHT*sizeof(char));
        sprintf(name_file,"%s-file-%d",client_name,i);
        if(os_delete(name_file))
            op_ok++;
        else
            op_ko++;
        free(name_file);
    }
    fprintf(stdout,"3@%d@%d\n",op_ok,op_ko);
}


int main(int argc, char *argv[])
{
    //check argomenti main
    if(argc <= 2)
    {
        fprintf(stderr,"Errore pochi parametri\n");
        exit(EXIT_FAILURE);
    }
    //nome client
    client_name=argv[1];


    //connessione
    if(!os_connect(client_name))
    {
        fprintf(stderr,"Errore nella connessione al socket\n");
        exit(EXIT_FAILURE);
    }
   
    //parametro test passato come argomento
    long int test_number = strtol(argv[2], NULL, 10); 
   
    
    switch(test_number)
    {
        case 1:
            test_1();
            break;
        
        case 2:
            test_2();
            break;
        
        case 3:
            test_3();
            break;

        default:
            fprintf(stderr,"Errore numero di test errato\n");
            exit(EXIT_FAILURE);
            
    }
    
    //Disconnessione
    if(!os_disconnect())
    {
        fprintf(stderr,"Errore nella disconessione dal socket\n");
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
    return 0;
}
