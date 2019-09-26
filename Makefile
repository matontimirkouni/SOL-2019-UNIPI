CC = gcc
CFLAGS = -std=c99 -g -pedantic -Wall  -Wmissing-field-initializers -D_POSIX_C_SOURCE=200809L 
.PHONY: clean test 
all:	server utils libobjectstore client 

server: src/server.c src/worker.c src/handler.c src/utils.c src/signal_thread.c
		@echo "Compilo Server"
		$(CC) $(CFLAGS) $^ -o $@.o  -lpthread
		
utils:	src/utils.c
		$(CC) $(CFLAGS) $^ -c -o  $@.o
		
libobjectstore:	src/libobjectstore.c
		@echo "Compilo Libreria"
		$(CC) $(CFLAGS) $^ -c -o $@.o
		ar rvs $@.a $@.o utils.o

client: src/client.c
	    @echo "Compilo Client"
	    $(CC) $(CFLAGS) $^ -o $@.o  -L . -lobjectstore 

testclient: src/utils.c src/tclient.c
	    @echo "Compilo Client di prova"
	    $(CC) $(CFLAGS) $^ -o $@.o  -L . -lobjectstore
	    ./$@.o

clean:
		@echo "Pulizia"
		-rm -f *.a
		-rm -f *.o
		-rm -f *.log
		-rm -f *.sock
		-rm -rf data

test: 
		@echo 'Inizio fase di test'
		seq 1 50 | xargs -n 1 -P50 -I{} ./client.o client{} 1 1>>testout.log;
		(seq 1 30 | xargs -n 1 -P30 -I{} ./client.o client{} 2 1>>testout.log) & 
		(seq 31 50 | xargs -n 1 -P20 -I{} ./client.o client{} 3 1>>testout.log) &
	
