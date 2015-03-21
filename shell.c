#include "shell.h"

struct Command* MakeCommand(){
	struct Command* command = malloc(sizeof(struct Command));
	command->fd = 0; //Not sure what to do with pipe file descriptors as of yet
	command->argcount = 0;
	command->nextCommand = NULL;
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
			command->nextCommand = MakeCommand();
			command = command->nextCommand;
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
		ptr = ptr->nextCommand;
	}
}
