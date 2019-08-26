# Progetto di Laboratorio di Sistemi Operativi 2019

Lo studente dovrà realizzare un object store implementato come sistema client-server, e destinato a supportare le richieste di memorizzare e recuperare blocchi di dati da parte di un gran numero di applicazioni. In particolare, sarà necessario implementare la parte server (object store) come eseguibile autonomo; una libreria destinata a essere incorporata nei client che si interfacci con l'object store usando il protocollo definito sotto; e infine un client di esempio che usi la libreria per testare il funzionamento del sistema.


# L'object store

L'object store è un eseguibile il cui scopo è quello di ricevere dai client delle richieste di memorizzare, recuperare, cancellare blocchi di dati dotati di nome, detti “oggetti”. 

L'object store gestisce uno spazio di memorizzazione separato per ogni cliente; i nomi degli oggetti sono garantiti essere univoci all'interno dello spazio di memorizzazione di un cliente, e i nomi dei clienti sono garantiti essere tutti distinti. Tutti i nomi rispettano il formato dei nomi di file POSIX.
 
 L'object store è un server che attende il collegamento di un client su una socket (locale) di nome noto, objstore.sock. Per collegarsi all'object store, un client invia al server un messaggio di registrazione nel formato indicato sotto; in risposta, l'object store crea un thread destinato a servire le richieste di quel particolare cliente. Il thread servente termina quando il client invia un esplicito comando di deregistrazione oppure se si verifica un errore nella comunicazione. Le altre richieste che possono essere inviate riguardano lo store di un blocco di dati, il retrieve di un blocco di dati, e il delete di un blocco di dati. 
 I dettagli del protocollo sono dati nel seguito. Internamente, l'object store memorizza gli oggetti che gli vengono affidati (e altri eventuali dati che si rendessero necessari) nel file system, all'interno di file che hanno per nome il nome dell'oggetto. Questi file sono poi contenuti in directory che hanno per nome il nome del client a cui l'oggetto appartiene. Tutte le directory dei client sono contenute in una directory data all'interno della working directory del server dell'object store. Il server quando riceve un segnale termina il prima possibile lasciando l’object store in uno stato consistente, cioè non vengono mai lasciati nel file system oggetti parziali. Quando il server riceve il segnale SIGUSR1, vengono stampate sullo standard output alcune informazioni di stato del server; tra queste, almeno le seguenti: numero di client connessi, numero di oggetti nello store, size totale dello store.


## Autore
Gioacchino Mirko Matonti


