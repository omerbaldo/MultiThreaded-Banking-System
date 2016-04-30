#include "client&server.h"

//GLOBAL STRUCTS-------------------------------------------------------------------------------------
    //SOCKET STUFF
    int sockfd = -1;
    struct sockaddr_in serverAddressInfo; //Address info
    struct hostent *serverIPAddress; //Info about the computer

    pthread_mutex_t lock; //LOCK SO READ AND WRITE THREADS CAN'T HAPPEN AT THE SAME TIME



//HELPER METHODS--------------------------------------------------------------------------------------
void error(char *msg)
{
    perror(msg);
    exit(0);
}

//INPUT OUTPUT THREADS-------------------------------------------------------------------------------
void * commandLineInput(void * arg){ //arg is there as a placeholder.
  
    
    char string[256];
    bzero(string, 256);//NULL OUT ARRAY
    
    while(1){
    
        pthread_mutex_lock(&lock);
        
            fgets(string,255,stdin); //reads in command line input until the last character
            
            long n = write(sockfd, string, strlen(string));
            
            if(n<0){ error("error writing to socket");}
            
            
            if(strcmp("exit",string)){
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
                if (n < 0) {error("ERROR reading from socket"); }
                
                if(strcmp("exit",string)){
                    printf("exiting\n");
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
//STEP 1: ERROR CHECKING------------------------------------------------------------------------------------------------
        if(argc != 2){ error("Error, Argument Count");}
    
//STEP 2 ATTEMPT TO CONNECT TO SERVER-----------------------------------------------------------------------------------

    
        //STEP 2.5 CREATE THREADS
        pthread_t writeThread ;//creates a thread ID for client command line
        pthread_t readThread;//created thread ID for reading from server response
    
        //do some signaling and lock stuff. idk yet
    
        printf("Welcome ! Attempting to connect to host \n");
        serverIPAddress = gethostbyname(argv[1]);
        if(serverIPAddress==NULL){ error("server does not exsist!"); }
        
        sockfd =  socket(AF_INET, SOCK_STREAM, 0);
        if(sockfd<0){error("error creating socket"); }
        
        bzero((char *) &serverAddressInfo, sizeof(serverAddressInfo)); //zero out the struct. (sin_zero becomes 0)
        serverAddressInfo.sin_family = AF_INET; //internet family
        serverAddressInfo.sin_port = htons(portNumber); //port number
        
        //set sin_addr;
        //set sin_zero to
        //                                      union, 4 byte IP in network byte order
        bcopy((char *)serverIPAddress->h_addr, (char *)&serverAddressInfo.sin_addr.s_addr, serverIPAddress->h_length);
        //      src                             desc                                        length to copy
        
        while(connect(sockfd, (struct sockaddr *)&serverAddressInfo, sizeof(serverAddressInfo))==-1)
        {
            printf("error trying to connect! retrying in three 3 seconds\n");
            sleep(3);
        }
        
    
    //if it is here it is successful
    
    
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
    
    
    
        pthread_create(&writeThread, NULL,&commandLineInput,NULL);//returns errno on failure, 0 if succesful
        pthread_create(&readThread, NULL,&serverOutput ,NULL);
    
        pthread_join(writeThread, NULL);
        pthread_join(readThread, NULL);
        //call join, join suspends the main program from going further until the thread terminates from start fnc
    
//STEP 4 CLOSE SOCKET---------------------------------------------------------------------------------------------------
    
    close(sockfd);
    return 0;
}
