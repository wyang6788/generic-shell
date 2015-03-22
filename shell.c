#include "shell.h"

struct Command* MakeCommand(){
	struct Command* command = malloc(sizeof(struct Command));
	command->fd = 0; //Not sure what to do with pipe file descriptors as of yet
	command->argcount = 0;
	command->next = NULL;
	return command;
}

void ReadLines(char* input){
	if(isatty(0)) printf("$\n");
	if(fgets(input,BUFFERSIZE,stdin)!=NULL) printf("%s",input);
}

struct Command* ParseCommandLine(char* input){
	struct Command* first;
	struct Command* command;
	int CommandCounter = 0;
	TokenizerT* tokenizer = TKCreate(" \n", input);
	char* token = TKGetNextToken(tokenizer);
	if(token==NULL){
		printf("There is absolutely nothing in the command line \n");
		return NULL;
	}	

	command = MakeCommand();
	first = command;
	while(token!=NULL){
		if(strcmp(token,"|")==0){
			command->next = MakeCommand();
			command = command->next;
			CommandCounter = 0;
		}
		else{
			command->args[CommandCounter] = strdup(token);
			command->argcount++;
			CommandCounter++;	
		}
	
		printf("token: %s\n",token);
		free(token);
		token = TKGetNextToken(tokenizer);
	}
	TKDestroy(tokenizer);
	return first;
}

void PrintCommandList(struct Command* command){
	struct Command* ptr = command;
	int i;
	while(ptr!=NULL){
		printf("Commands: ");
		for(i = 0; i < ptr->argcount; i++){
			printf("%s, ",ptr->args[i]);
		}	
		printf("\n");
		ptr = ptr->next;
	}
}

void DestroyCommandList(struct Command* command){
	struct Command* ptr = command;
	struct Command* next = NULL;
	int i;
	while(ptr!=NULL){
		next = ptr->next;
		for(i = 0; i<ptr->argcount; i++){
			free(ptr->args[i]);
		}
		free(ptr);
		ptr = next;
	}
}

int f_cd(struct Command* command){
	if(!command->args[1]){
		if(chdir(getenv("HOME"))==0){
			return 1;		
		}
		else{	
			printf("ERROR: cd failed\n");
			return 0;
		}
	}
	else if(command->args[2]){
		printf("ERROR: too many arguments\n");
		return 0;
	}
	else{
		if(chdir(command->args[1])==0){
			return 1;
		}
		else{
			printf("ERROR: cd failed\n");
			return 0;
		}
	}
}

int f_exit(struct Command* command){
	if(!command->args[1]){
		exit(0);
	}
	else{
		exit(atoi(command->args[1]));
	}
}

struct Builtins* MakeBuiltins(){
	struct Builtins* builtins = malloc(sizeof(struct Builtins));
	builtins->name = "cd";
	builtins->f = f_cd;
	builtins->next = malloc(sizeof(struct Builtins));
	builtins->next->name = "exit";
	builtins->next->f = f_exit;
	builtins->next->next = NULL;
	return builtins;
}