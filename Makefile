all: server.c client.c Header.h
		gcc -pthread -o server server.c
		gcc -pthread -o client client.c		
		
clean:
		rm -f server client
