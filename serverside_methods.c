#include "client&server.h"
//GLOBALS------------------------------------------------------------------------
int socketNumba = -1;
pthread_t printThread;
pthread_t customerThread;


//STRUCTERS------------------------------------------------------------------------
struct acc_info{
    int is_free; //0 means not free, 1 means free space
    char acc_name[100];
    float balance;
    int in_session; //0 means not in use, 1 means in use
};

acc_info* account_list[20] = (acc_info*)malloc(20 * sizeof(struct acc_info)); //array of strings consisting of the account names
//int[20] freelist; //tells if a particular position in the account list array is free
int numused = 0; //number of accounts currently in bank


struct socketNode{
    int socketNumber;
    struct socketNode *next;
};
struct socketNode * head;
struct socketNode * last;


struct socketNode* constructor(int socketNumber, struct socketNode * next){
    
    struct socketNode * nn = (struct socketNode *) malloc(1*(sizeof(struct socketNode)));
    
    nn->next = next;
    nn->socketNumber = socketNumber;
    
    return nn;
}


//HELPER METHODS----------------------------------------------------------------------------
void error(char *msg){
    perror(msg);
    exit(0);
}

void shutDownHandler(){
    if(head==NULL){return;}
    struct socketNode * ptr;
    ptr = head;
    while(ptr!=NULL){
        ptr=head->next;
        close(head->socketNumber);
        free(head);
        head=ptr;
    }
    exit(0);
}


//INPUT/OUTPUT TO CLIENT---------------------------------------------------------------------

char* open_account(char* name){ //RETURNS 0 ON SUCCESS, RETURNS 1, 2, AND 3 ON ERROR
     char retstr[100];
    
    if(numused == 20){
        strcpy(retstr, "ERROR! SORRY, NO SPACE IN BANK");
        return retstr; //ERROR! no space in bank
    }
    
    for(i = 0; i< 20; i++){
        if(account_list[i]->is_free == 1){ //this location account space is free
            continue;
        }
        if(strcmp(name, account_list[i]->acc_name) == 0){
            strcpy(retstr, "ERROR! ACCOUNT WITH SAME NAME ALREADY EXISTS. PLEASE CHOOSE A DIFFERENT NAME");
            return retstr; //ERROR! ACCOUNT WITH SAME NAME ALREADY EXISTS
        }
    }
    
    //CHECK IF CLIENT IS ALREADY IN CUSTOMER SESSION, IF YES, CAUSE ERROR
    
    
    if(strlen(name) == 0 || strlen(name) > 100){
        strcpy(retstr, "ERROR! ACCOUNT NAME LENGTH IS INAPPROPRIATE");
        return retstr; //ERROR! ACCOUNT LENGTH IS INAPPROPRIATE. IT CANNOT BE ZERO OR GREATER THAN 100
    }
    
    //AT THIS POINT, IT IS SAFE TO CREATE THE NEW ACCOUNT
    for(i = 0; i< 20; i++){
        if(account_list[i]->is_free == 1){
            break;
        }
    }
    //At this point, we are at the first available free location in array
    
    account_list[i]->is_free = 0;
    account_list[i]->balance = 0;
    strcpy(account_list[i]->acc_name, name);
    account_list[i]->in_session = 0;
    numused++;
    //Account successfully created, and added to the account_list array
     strcpy(retstr, "WOOT! ACCOUNT SUCCESSFULLY CREATED.");
    return retstr;
    
}




int start_account(char* name){ //RETURNS location index (positive number) of account in array if successfully started; returns -1, and -2 on ERROR!
    int i, location_index;
    
    for(i = 0; i< 20; i++){
        if(strcmp(account_list[i]->acc_name, name) == 0){
            if(account_list[i]->in_session == 1){ //account is already in session, ERROR!
                return -1;
            }
            account_list[i]->in_session = 1;
            location_index = i;
            return location_index;
        }
    }
    
    return -2; //account name does not exist, ERROR!
}








int credit(int location_index, float amount){
    if(location_index < 0){
        return 1; //client not in session
    }
    
    account_list[location_index]->balance = account_list[location_index]->balance + amount;
    return 0; //success
}





int debit(int location_index, float amount){ //returns 0 on successful debit, 1 if customer is not in session, and 2 if customer trying to debit more than balance
    if(location_index < 0){
        return 1; //client not in session
    }
    
    if(amount > account_list[location_index]->balance){
        return 2; //NOT ENOUGH AMOUNT TO WITHDRAW, LOW ON BALANCE ERROR!
    }
    account_list[location_index]->balance = account_list[location_index]->balance - amount;
    return 0; //success
}





float balance(char* name){
    int i;
    
    for(i = 0; i< 20; i++){
        if(strcmp(account_list[i]->acc_name, name) == 0){
            break;
        }
    }
    
    if(account_list[i]->in_session == 0){
        return 1; //not in session error!
    }
    
    return account_list[i]->balance;
}




int finish(char* name){
    int i;
    
    for(i = 0; i< 20; i++){
        if(strcmp(account_list[i]->acc_name, name) == 0){
            break;
        }
    }
    
    if(account_list[i]->in_session == 0){ //account already not in session, error!
        return 1;
    }
    
    account_list[i]->in_session = 1;
    return 0; //success
}



void server_print(){
    int i;
    
    for(i = 0; i < 20; i++){
        
        if(account_list[i]!=NULL){
            if(account_list[i]->isfree == 1){
                continue;
            }
            if(account_list[i]->in_session == 1){ //account is in session
                printf("%s%s\n%s\n\n", "Account Name: ", account_list[i]->acc_name, "IN SERVICE");
            }
            else{ //account is not in session
                printf("%s%s\n%s%f\n\n", "Account Name: ", account_list[i]->acc_name, "Balance: ", account_list[i]->balance);
            }
        }
    }
}







int main(int argc, char ** argv){
    
    int returnVal;//store return value of functions
    signal(SIGINT, shutDownHandler);//disconnects when user presses cnt c
    
    //STEP 1: CREATE THE SOCKET (2 way wire)
    socketNumba = socket(AF_INET, SOCK_STREAM,0);//create socket
    if(socketNumba<0){
        error("could not open server socket");
    }
    
    //STEP 2. DO STUFF WITH SERVER ADDRESS OBJECT
    struct sockaddr_in serverAddressInfo;
    bzero((char *) &serverAddressInfo, sizeof(serverAddressInfo)); //INITILIAZE
    serverAddressInfo.sin_port = htons(portNumber); //ADD PORT
    serverAddressInfo.sin_family = AF_INET; //INTERNET FAMILY
    serverAddressInfo.sin_addr.s_addr = INADDR_ANY; //TYPE OF IP CONNECTIONS WE CAN ADD
    
    //STEP 2: BIND THE wire/socket TO A PORT on SERVER
    returnVal =	bind(socketNumba, (struct sockaddr *) &serverAddressInfo, sizeof(serverAddressInfo));
    if(returnVal<0){
        error("binding error");
    }
    
    //STEP 3: LISTEN FOR INCOMING CONNECITONS ON SOCKET
    listen(socketNumba,10); //10 pending connection on queue
				
    //STEP 4: CREATE PRINTING THREAD BEFORE YOU SELECT/ACCEPT INCOMING CONNECTIONS
    pthread_create(&printThread, NULL, (void*)server_print, NULL);
    pthread_detach(printThread);
    
    
    //STEP 4: ACCEPT INCOMING CONNECTIONS
    int oppositeSocket;
    int clilen = -1;
    while(1){ //create select
        
        struct sockaddr_in clientAddressInfo;
        
        clilen = sizeof(clientAddressInfo);
        
        oppositeSocket = accept(socketNumba, (struct sockaddr *)&clientAddressInfo, &clilen);
        
        if(oppositeSocket==-1){
            continue;
        }
        
        pthread_create(&customerThread, NULL, method which deals with input/output, clientAddressInfo);
        pthread_detach(customerThread);
        
        if(head==NULL){
            head = constructor(oppositeSocket, NULL);
        }else{
            last->next = constructor(oppositeSocket, NULL);
            last = last->next;
        }
        
        
    }
    return 0;
}
