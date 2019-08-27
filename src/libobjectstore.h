#ifndef LIBOBJECTSTORE_H_
#define LIBOBJECTSTORE_H_

//Overview: Libreria per la gestione del comunicazioni dei client verso il server


/*
    @REQUIRES:  name != Null 
    @EFFECTS: inizia la connessione all'object store, registrando il cliente con il
    name dato. Restituisce true se la connessione ha avuto successo, false altrimenti.
*/
int os_connect(char *name);

/*
    @REQUIRES: name != Null  && block!= Null
    @EFFECTS: richiede all'object store la memorizzazione dell'oggetto puntato da block, 
    per una lunghezza len, con il nome name. Restituisce true se la memorizzazione 
    ha avuto successo, false altrimenti.
*/
int os_store(char *name, void *block, size_t len);

/*
    @REQUIRES: name != Null 
    @EFFECTS: recupera dall'object store l'oggetto precedentemente
    memorizzatato sotto il nome name. Se il recupero ha avuto successo, restituisce 
    un puntatore a un blocco di memoria, allocato dalla funzione, contenente i dati 
    precedentemente memorizzati. In caso di errore, restituisce NULL.
*/
void *os_retrieve(char *name);

/*
    @REQUIRES: name != Null 
    @EFFECTS: cancella l'oggetto di nome name precedentemente memorizzato.
    Restituisce true se la cancellazione ha avuto successo, false altrimenti.
*/
int os_delete(char *name);

/*
    @EFFECTS: chiude la connessione all'object store. Restituisce true se la disconnessione ha
    avuto successo, false in caso contrario.
*/
int os_disconnect();




#endif /* LIBOBJECTSTORE_H_ */
