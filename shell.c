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

void ExecuteCommands(struct Command* command){
	struct Builtins* builtins = MakeBuiltins();

	if(!command==NULL) {
		while(!(strcmp(builtins->name,"end")==0)) {
			if(strcmp(builtins->name,command->args[0])==0) {
				int status = (*builtins->f)(command);
				printf("%d\n",status);
			}
			builtins = builtins->next;	
		}
	}
	
	//Check for builtins, execute,  otherwise execute nonbuiltin commands
	
	DestroyBuiltins(builtins);
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

int f_fork(struct Command* command) {
	int pid;

	switch(pid=fork()) {
		case 0:
			printf("Child Process");
			ExecuteCommands(command->next);
			exit(0);
		default:
			printf("Parent Process");
			ExecuteCommands(command->next);
			exit(0);
		case -1:
			perror("The fork broke");
			exit(1);	
	}
	return(pid);
}

void f_execve(struct Command* command) {
	char *args[] = {"ls","-aF","/",0};
	char *env[] = { 0 };
	if(!command->args[1]) {
		printf("Nothing to Execute");
		return;	
	} else {
		execve(command->args[1],args,env);
	}
	perror("execve");
	exit(1);
}

int f_pipe(struct Command* command) {
	if (pipe(command->fd) == 0)
	{
		//I feel like there should be more to pipe, such as writing to files
		//But I am not sure yet how to implement it.
		return 1;
	} else {
		printf("Pipe failed.");
		return 0;
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
	builtins->next->next = malloc(sizeof(struct Builtins));
	builtins->next->next->name = "end";
	return builtins;
}

void DestroyBuiltins(struct Builtins* builtins){
	free(builtins->next);
	free(builtins);
}
