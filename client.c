#include "Header.h"

//GLOBAL STRUCTS-------------------------------------------------------------------------------------
int sockfd = -1;
pthread_mutex_t lock; //LOCK SO READ AND WRITE THREADS CAN'T HAPPEN AT THE SAME TIME https://en.wikipedia.org/wiki/Readers%E2%80%93writer_lock

//HELPER METHODS--------------------------------------------------------------------------------------
void error(char *msg)
{
    perror(msg);
    exit(1);
}

//INPUT OUTPUT THREADS-------------------------------------------------------------------------------
void * commandLineInput(void * arg){ //arg is there as a placeholder.
    
    char string[256];
    bzero(string, 256);//NULL OUT ARRAY
    while(1){
        
        pthread_mutex_lock(&lock);
        
        fgets(string,255,stdin); //reads in command line input until the last character
        
        long n = write(sockfd, string, strlen(string));
        
        if(n<0){ error("error writing to socket \n");}
        
        if(strcmp("exit",string)==0){
            printf("exiting\n");
            pthread_exit(0);
            return 0;
        }
        
        bzero(string, 256);
        sleep(2);
        
        pthread_mutex_unlock(&lock);
    }
    
    return NULL;
}


//reads server output
void * serverOutput (void *arg){
    char string[256];
    bzero(string, 256);//NULL OUT ARRAY
    
    while (1){
        
        pthread_mutex_lock(&lock);
        
        long n = read(sockfd,string,255); //read until the last character to keep \0
        
        // if we couldn't read from the server for some reason, complain and exit
        if (n < 0) {error("ERROR reading from socket \n"); }
        
        if(strcmp("exit",string)){
            printf("Server Shut Down !!\n");
            pthread_exit(0);
            return 0;
        }
        
        printf("%s\n",string);
        bzero(string, 256);
        
        pthread_mutex_unlock(&lock);
        
    }
    
    return NULL;
}



int main(int argc, char ** argv)
{
    
    struct sockaddr_in serverAddressInfo; //Address info
    struct hostent *serverIPAddress; //Info about the computer
    
    //STEP 1: ERROR CHECKING------------------------------------------------------------------------------------------------
    if(argc != 2){ error("Error, Argument Count \n");}
    
    //STEP 2 ATTEMPT TO CONNECT TO SERVER-----------------------------------------------------------------------------------
    
    if(pthread_mutex_init(&lock, NULL)!=0){error("mutex init failed \n");}
    
    //STEP 2.5 CREATE THREADS
    pthread_t readThread;//creates a thread ID for reading from server response, and writing back to user
    pthread_t writeThread ;//creates a thread ID for reading from client command line
    
    //do some signaling and lock stuff. idk yet
    
    
    printf("Welcome! Attempting to connect to host \n");
 
 printf("0 \n");
    
    serverIPAddress = gethostbyname(argv[1]);
    if(serverIPAddress==NULL){ error("server does not exist! \n"); }
    
  printf("1 \n");
    
    sockfd =  socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd<0){error("error creating socket \n"); }
    
    memset(&serverAddressInfo, '\0', sizeof(serverAddressInfo));
    
    serverAddressInfo.sin_family = AF_INET; //internet family

    serverAddressInfo.sin_port = htons(portNumber); //port number in network btye order
    serverAddressInfo.sin_addr.s_addr = inet_addr("127.0.0.1");//local host
    
    
    
    //memcpy(&serverAddressInfo.sin_addr, serverIPAddress->h_addr_list[0], serverIPAddress->h_length);
    
   printf("2 \n");

    
    //set sin_addr;
    //set sin_zero to
    //                                      union, 4 byte IP in network byte order
   // bcopy((char *)serverIPAddress->h_addr, (char *)&serverAddressInfo.sin_addr.s_addr, serverIPAddress->h_length);
    //      src                             desc                                        length to copy
    
    
    
    
    
    
    printf("3 \n");
    
    socklen_t size = sizeof(serverAddressInfo);
    
    while(connect(sockfd, (struct sockaddr *)&serverAddressInfo,size)<0)
    {

        printf("4 \n");

        printf("Error Trying To Connect: %s . Trying Again In 3 Seconds\n", strerror(errno));
        sleep(3);
    }
    
    
    //if it is here it is successful
    
    printf("5 \n");

    //STEP 3 JOIN/START THREADS--------------------------------------------------------------------------------------------
    
    printf("Connection Successful \n");
    printf("Please choose one of the following options \n");
    printf("open <account name> \n");
    printf("start <account name> \n");
    printf("credit <amount> \n");
    printf("debit <amount> \n");
    printf("balance \n");
    printf("finish \n");
    printf("exit \n");
    
    //maybe have a lock so read and write cant happen at the same time
    
    pthread_create(&writeThread, NULL,&commandLineInput,NULL);//returns errno on failure, 0 if successful
    pthread_create(&readThread, NULL,&serverOutput ,NULL);
    
    pthread_join(writeThread, NULL);
    pthread_join(readThread, NULL);
    //call join, join suspends the main program from going further until the thread terminates from start fnc
    
    //STEP 4 Free/Close STUFF---------------------------------------------------------------------------------------------------
    
    pthread_mutex_destroy(&lock);
    close(sockfd);
    return 0;
}
