#include "client&server.h"




void * commandLineInput(void * arg){ //arg is there as a placeholder.
    
    char string[100];//
    
    while(1){
        scanf("%s",(char*)&string); //read one string from command line
        
    }
}



//reads server output
void * serverOutput (void *arg){
    
    
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
    char * server = argv[1]; //IP address
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
