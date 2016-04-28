#include "client&server.h"


//---------------------------GLOBAL STRUCTS
int sockfd = -1;
struct sockaddr_in serverAddressInfo; //address info
struct hostent *serverIPAddress; //Info about the computer

//-------------------------------------------------------------------------------INPUT OUTPUT THREADS
void * commandLineInput(void * arg){ //arg is there as a placeholder.
    char string[100];//
    while(1){
        scanf("%s",(char*)&string); //read one string from command line
    }
}

//reads server output
void * serverOutput (void *arg){
    return NULL;
}



//try to connect to server . if it cannot try to connect every 3 seconds.

//prompt for input and send input to server every 2 seconds

//create two threads, one command input thread to read commands from client
    //two to read messages from server and send to the user



int main(int argc, char ** argv)
{
    
    //STEP 1: ERROR CHECKING
    if(argc != 2){
        printf("Error, Argument Count");
        exit(0);
    }
    
    
    //STEP 2 ATTEMPT TO CONNECT TO SERVER
    char * servername = argv[1]; //IP address
    printf("Welcome ! Attempting to connect to host \n");
    
    
    pthread_t clientThread ;//creates a thread ID for client command line
    pthread_t serverThread;//created thread ID for reading from server response
    
    
    //NETWORKING
    
        //create a connection structer
            //init you should have socket #(should start at -1
            //you should have IP address of connection your connecting to
                //to do this use gethostbyname to return a hostent * struct
                    //if this is null. error the host does not exsist
            //next try to connect to the server by calling socket()
                //error if socket <0 . cannot create a valid connection
    
    serverIPAddress = gethostbyname(servername);
    if(serverIPAddress==NULL){
        printf("server does not exsist!");
        exit(0);
    }
    
    sockfd =  socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd<0){
        printf("error creating socket");
        exit(0);
    }
    
    bzero((char *) &serverAddressInfo, sizeof(serverAddressInfo)); //zero out the struct. good cause it sets sin_zero
    serverAddressInfo.sin_family = AF_INET; //internet family
    serverAddressInfo.sin_port = htons(portNumber); //port number
    
    //set sin_addr;
    //set sin_zero to
    //                                      union, 4 byte IP in network byte order
    bcopy((char *)serverIPAddress->h_addr, (char *)&serverAddressInfo.sin_addr.s_addr, serverIPAddress->h_length);
    //      src                             desc                                        length to copy
    
    while(connect(sockfd, (struct sockaddr *)&serverAddressInfo, sizeof(serverAddressInfo))==-1)
    {
        printf("error trying to connect! retrying in three3 seconds");
        sleep(3);
    }
    
    
    //if it is here it is successful
    
    
    //STEP 3 CREATE THREADS
    printf("Connection Successful \n");
    printf("Please choose one of the following options \n");
    printf("open <account name> \n");
    printf("start <account name> \n");
    printf("credit <amount> \n");
    printf("debit <amount> \n");
    printf("balance \n");
    printf("finish \n");
    printf("exit \n");
    
    
    pthread_create(&clientThread, NULL,&commandLineInput,NULL);
    pthread_create(&serverThread, NULL,&serverOutput ,NULL);
}