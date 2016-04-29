struct acc_info{
	int isfree; //0 means not free, 1 means free space
	char accountname[100];
	int balance;
	int in_use; //0 means not in use, 1 means in use
};

acc_info* account_list[20] = (acc_info*)malloc(20 * sizeof(struct acc_info)); //array of strings consisting of the account names
//int[20] freelist; //tells if a particular position in the account list array is free
int numused = 0; //number of accounts currently in bank

int open_account(char* name){
	int i;
	
	if(numused == 20){
		return 1; //ERROR! no space in bank
	}
	
	for(i = 0; i< 20; i++){
		if(acc_info[i]->isfree == 1){ //free account space
			continue;
		}
		if(strcmp(name, account_list[i]) == 0){
			return 2; //ERROR! ACCOUNT WITH SAME NAME ALREADY EXISTS
		}
	}
	
	//CHECK IF CLIENT IS ALREADY IN CUSTOMER SESSION, IF YES, CAUSE ERROR
	
	
	if(strlen(name) == 0 || strlen(name) > 100){
		return 3; //ERROR! ACCOUNT LENGTH IS INAPPROPRIATE. IT CANNOT BE ZERO OR GREATER THAN 100
	}
	
	//AT THIS POINT, IT IS SAFE TO CREATE THE NEW ACCOUNT
	
}
