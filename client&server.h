#ifndef ____client&server__
#define ____client&server__

//INPUT
    //good for input / output
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>

//NETWORKING
    #include <sys/types.h>
    #include <sys/socket.h> //need this for sockets
    #include <netinet/in.h>
    #include <netdb.h>
    #include <unistd.h>

//TIME
    #include <time.h> 
    #include <signal.h>

//THREAD
    //good for creating new posix thread
    //each thread executes the same program. (share same globabl memory) but has its own stack
    //http://man7.org/linux/man-pages/man7/pthreads.7.html
    #include <pthread.h>





//Definitions
    #define portNumber 8080



//Structs





//RESOURCES
/*
 Socket Info:
    http://man7.org/linux/man-pages/man2/socket.2.html
 
    int socket(int domain, int type, int protocol);
    DOMAIN = AF_INET for IPv4 protocols
    TYPE = SOCK_STREAM for two way chat
    PROTOCOL =
 
 
 
 
 
 
 Pthread Info:
 
 pthread_create
 arguements:
 1) are thread ID pointer
 2) attr is a structure for additional arguements(if NULL it is defualt attributes)
 3) start routine is the method it starts at
 4) arg is the one arguement for the start function
 http://man7.org/linux/man-pages/man3/pthread_create.3.html
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 */




#endif
