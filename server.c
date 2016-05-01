#include "client&server.h"
//GLOBALS------------------------------------------------------------------------
int socketNumba = -1;
pthread_t printThread;
pthread_t customerThread;
pthread_mutex_t openAccountlock;
pthread_mutex_t startAccountlock;

//STRUCTERS------------------------------------------------------------------------
struct acc_info{
    int is_free; //0 means not free, 1 means free space
    char acc_name[100];
    float balance;
    int in_session; //0 means not in use, 1 means in use
};

struct acc_info account_list[20]; //array of strings consisting of the account names
int numused = 0; //number of accounts currently in bank


struct socketNode{
    int socketNumber;
    struct socketNode *next;
};
struct socketNode * head = NULL;
struct socketNode * last = NULL;


struct socketNode* constructor(int socketNumber, struct socketNode * next){
    struct socketNode * nn = (struct socketNode *) malloc(1*(sizeof(struct socketNode)));
    nn->next = next;
    nn->socketNumber = socketNumber;
    return nn;
}

struct client{
    struct sockaddr address;
    int socketNumber;
};

struct client* constructor2(){
    struct client * nn = (struct client*)malloc(1*(sizeof(struct client)));
    return nn;
}


//HELPER METHODS----------------------------------------------------------------------------
void error(char *msg){
    perror(msg);
    exit(1);
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

//convert account name to index
//returns -1 if not found
//-2 if in use

int accountToIndex(char * name){
    int i;
    for(i = 0; i< 20; i++){
        if(strcmp(account_list[i].acc_name, name) == 0){
	    		   	 if(account_list[i].in_session == 1){
                         return -2;
                     }
            return i;
        }
    }
    return -1;
}



//INPUT/OUTPUT TO CLIENT---------------------------------------------------------------------

char * open_account(char* name){
    
    int i;
    pthread_mutex_lock(&openAccountlock);
    
    char retstr[100];
    bzero(retstr,100);
    
    char * r;
    if(numused == 20){
        strcpy(retstr, "ERROR! SORRY, NO SPACE IN BANK");
        r = retstr;
        
        return r; //ERROR! no space in bank
    }
    
    for(i = 0; i< 20; i++){
        if(account_list[i].is_free == 1){ //this location account space is free
            continue;
        }
        if(strcmp(name, account_list[i].acc_name) == 0){
            strcpy(retstr, "ERROR! ACCOUNT WITH SAME NAME ALREADY EXISTS. PLEASE CHOOSE A DIFFERENT NAME");
            r = retstr;
            return r; //ERROR! ACCOUNT WITH SAME NAME ALREADY EXISTS
        }
    }
    
    //CHECK IF CLIENT IS ALREADY IN CUSTOMER SESSION, IF YES, CAUSE ERROR
    if(strlen(name) == 0 || strlen(name) > 100){
        strcpy(retstr, "ERROR! ACCOUNT NAME LENGTH IS INAPPROPRIATE");
        r = retstr;
        return r; //ERROR! ACCOUNT LENGTH IS INAPPROPRIATE. IT CANNOT BE ZERO OR GREATER THAN 100
    }
    
    //AT THIS POINT, IT IS SAFE TO CREATE THE NEW ACCOUNT
    for(i = 0; i< 20; i++){
        if(account_list[i].is_free == 1){
            break;
        }
    }
    //At this point, we are at the first available free location in array
    account_list[i].is_free = 0;
    account_list[i].balance = 0;
    strcpy(account_list[i].acc_name, name);
    account_list[i].in_session = 0;
    numused++;
    //Account successfully created, and added to the account_list array
    strcpy(retstr, "WOOT! ACCOUNT SUCCESSFULLY CREATED.");
    r = retstr;
    pthread_mutex_unlock(&openAccountlock);
    
    return r;
    
}




char * start_account(char* name){ //updates location_index of account in account_list array
    
    pthread_mutex_lock(&startAccountlock);
	   
    int i;
    char retstr[100];
    bzero(retstr,100);
    char *r;
    
    for(i = 0; i< 20; i++){
        if(strcmp(account_list[i].acc_name, name) == 0){
		          
            if(account_list[i].in_session == 1){ //account is already in session, ERROR!
                strcpy(retstr, "Error! Account already in Session");
                r = retstr;
                return r;
            }
            
            account_list[i].in_session = 1;
            strcpy(retstr, "Account is now open");
            r = retstr;
            return r;
        }
    }
    
    strcpy(retstr, "Error! Account name does not exist.");
    
    pthread_mutex_unlock(&startAccountlock);
    r = retstr;
    return r; //account name does not exist, ERROR!
}


char * credit(int location_index, float amount){
	   
	   
    char retstr[100];
    bzero(retstr,100);
    char *r;
    if(location_index < 0){
        strcpy(retstr, "Client cannot credit amount when not in session");
        r = retstr;
        return r; //client not in session
    }
    
    account_list[location_index].balance = account_list[location_index].balance + amount;
    strcpy(retstr, "Successfully credited amount");
    
    r = retstr;
    return r; //success
}


char* debit(int location_index, float amount){
    char retstr[100];
    bzero(retstr,100);
    char * r;
    if(location_index < 0){
        strcpy(retstr, "Client cannot debit amount when not in session");
        r = retstr;
        return r; //client not in session
    }
    
    if(amount > account_list[location_index].balance){
        strcpy(retstr, "NOT ENOUGH AMOUNT TO WITHDRAW, LOW ON BALANCE ERROR!");
        r = retstr;
        return r; //NOT ENOUGH AMOUNT TO WITHDRAW, LOW ON BALANCE ERROR!
    }
    account_list[location_index].balance = account_list[location_index].balance - amount;
    strcpy(retstr, "Successfully debited amount");
    r = retstr;
    return r; //success
}

float balance(char* name){
    int i;
    
    for(i = 0; i< 20; i++){
        if(strcmp(account_list[i].acc_name, name) == 0){
            break;
        }
    }
    
    if(account_list[i].in_session == 0){
        return -1; //not in session error!
    }
    
    return account_list[i].balance;
}


char* finish(char* name){
    char retstr[100];
    bzero(retstr,100);
    int i;
    char * r;
    
    for(i = 0; i< 20; i++){
        if(strcmp(account_list[i].acc_name, name) == 0){
            break;
        }
    }
    
    if(account_list[i].in_session == 0){ //account already not in session, error!
        strcpy(retstr, "account already not in use, error!");
        r = retstr;
        return r;
    }
    
    account_list[i].in_session = 1;
    strcpy(retstr, "Successfully ended session");
    r = retstr;
    return r; //success
}



void serverexec(void * socketinfo){
    
    struct client * wire = (struct client *)(socketinfo);
    
    
    char string[256]; //reads the user input
    bzero(string, 256);
    
    
    char* token; //token for input
    
    char * returnstring; //points to a string returned by function
    bzero(returnstring, 256);
    
    int clientSocket = wire->socketNumber;
    int location_index = -1;//-1 means no account is open
    
    while(1){
        
        long retval = read(clientSocket, string, strlen(string));
        
        if(retval < 0){
            error("error reading from socket");
        }
        
        if(strncmp("open", string, 4) == 0){
            
            token = strtok(string, " ");
            token = strtok(NULL, " ");
            
            returnstring = open_account(token);
            write(clientSocket, returnstring, strlen(returnstring));
            
        }
        else if(strncmp("start", string, 5) == 0){
            
            token = strtok(string, " ");
            token = strtok(NULL, " ");
            
            returnstring = open_account(token);
            location_index = accountToIndex(token);
            
            write(clientSocket, returnstring, strlen(returnstring));
        }
        else if(strncmp("credit", string, 6)){
            
            token = strtok(string, " ");
            token = strtok(NULL, " ");
            
            returnstring = credit(location_index, (float)atof(token));
            
            write(clientSocket, returnstring, strlen(returnstring));
        }
        else if(strncmp("debit", string, 5)){
            token = strtok(string, " ");
            token = strtok(NULL, " ");
            
            returnstring = debit(location_index, (float)atof(token));
            write(clientSocket, returnstring, strlen(returnstring));
        }
        else if(strncmp("balance", string, 7)){
            
            retval =  balance(account_list[location_index].acc_name);
            
            if(retval == -1){
                write(clientSocket,"Not in customer session",24);
                continue;
            }
            sprintf(returnstring, "Balance in account: %f", balance(account_list[location_index].acc_name));
            
            write(clientSocket, returnstring, strlen(returnstring));
            
            
        }
        else if(strncmp("finish", string, 6)){
            
            returnstring = finish(account_list[location_index].acc_name);
            write(clientSocket, returnstring , strlen(returnstring));
        }
        
        else if(strncmp("exit", string, 4)){
            
            close(clientSocket);
            free(socketinfo);
            return;
        }
        else{
            strcpy(returnstring, "Invalid input. Choose from one of the options above.");
            write(clientSocket, returnstring, strlen(returnstring));
        }
        
        bzero(string, 256);
        bzero(returnstring, 256);
        
        
    }
    
    return;
}


void server_print(){
    int i;
    
    while(1){
        
        for(i = 0; i < 20; i++){
                if(account_list[i].is_free == 1){
                    continue;
                }
                if(account_list[i].in_session == 1){ //account is in session
                    printf("%s%s\n%s\n\n", "Account Name: ", account_list[i].acc_name, "IN SERVICE");
                }
                else{ //account is not in session
                    printf("%s%s\n%s%f\n\n", "Account Name: ", account_list[i].acc_name, "Balance: ", account_list[i].balance);
                }
            }
        sleep(20);
    }
}



void accountInit(){
    int i = 0;
    for(;i<20;i++){
        bzero(account_list[i].acc_name,100);
        account_list[i].in_session = 0;
        account_list[i].is_free = 1;
        account_list[i].balance = 0;
    }
    
}




int main(int argc, char ** argv){
    int returnVal;//store return value of functions
    
    accountInit();//initiliaze account data structure
    
    signal(SIGINT, shutDownHandler);//disconnects when user presses cnt c
    signal(SIGHUP, shutDownHandler);
    
    pthread_mutex_init(&openAccountlock,NULL);
    pthread_mutex_init(&startAccountlock,NULL);
    
    
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
    
    
    printf("Server is now accepting incoming connections");
    //STEP 4: ACCEPT INCOMING CONNECTIONS
    int oppositeSocket;
    while(1){ //create select
        
        struct client * wire = constructor2();
        
        socklen_t bytesToRead = sizeof(wire->address);
        
        oppositeSocket = accept(socketNumba, &wire->address, &bytesToRead); //from socketNumba, fill in the clientAddressInfo Struct and the size to read in
        
        wire->socketNumber = oppositeSocket;
        
        if(oppositeSocket==-1){
            free(wire);
            continue;
        }
        
        printf("connection added");
        pthread_create(&customerThread, NULL, (void*)serverexec, wire);
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
