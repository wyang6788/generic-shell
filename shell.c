// Group Names: Peter Zhang, William Yang, Ellis Pomirchy, Joseph Pestonit, Philip Beauchea
#include "shell.h"

/**
*	Make Command - Generages an empty command.
*	Param - None
*	Return - Empty command
*
*/
struct Command* MakeCommand(){
	struct Command* command = malloc(sizeof(struct Command));
	command->argcount = 0;
	command->next = NULL;
	return command;
}

/**
*	ReadLines - Read console input
*	Param - Console input
*	Return - None
*
*/
void ReadLines(char* input){
	if(isatty(0)) printf("$\n");
	if(fgets(input,BUFFERSIZE,stdin)!=NULL) printf("%s",input);
}

/**
*	Parse Command Line - Tokenizes console input, and generates linked list of commands.
*	Param - Console input
*	Return - Linked list of commands with reference to the first command
*
*/

struct Command* ParseCommandLine(char* input){
	struct Command* first;
	struct Command* command;
	int CommandCounter = 0;

	// Tokenize
	TokenizerT* tokenizer = TKCreate(" \n", input);
	char* token = TKGetNextToken(tokenizer);
	if(token==NULL){
		return NULL;
	}	
	
	// Generate commands based on tokens
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
			if(command->argcount>50){
				fprintf(stderr,"Too many arguments\n");
				return NULL;
			}	
		}
	
		free(token);
		token = TKGetNextToken(tokenizer);
	}
	TKDestroy(tokenizer);
	return first;
}

/**
*	Execute Commands - Checks commands for builtins, then runs any commands through a pipeline
*	Param - First command to execute (with rest of linked list of commands)
*	Return - None
*
*/
void ExecuteCommands(struct Command* command){
	// Check for builtins
	struct Builtins* builtins = MakeBuiltins();
	struct Builtins* ptr = builtins;
	if(command!=NULL) {
		while(ptr!=NULL) {
			if(strcmp(ptr->name,command->args[0])==0) {
				(*ptr->f)(command);
				break;
			}
			ptr = ptr->next;	
		}
		
	}

	// Run Pipeline
	PipeFirst(command);		
	
	DestroyBuiltins(builtins);
}

/**
*	Pipe First - Runs first command in pipe, then runs the rest of commands in pipe
*	Param - Linked list of commands
*	Return - 1 after completion
*
*/
int PipeFirst(struct Command* command){
	if(command->next!=NULL){
		pipe(command->next->fd);
	}
	// Do first pipe
	int pid, status;
	switch(pid=fork()){
		case 0:
			if(command->next!=NULL){
				dup2(command->next->fd[1],1);
				close(command->next->fd[0]);
			}
			execvp(command->args[0],command->args);
		default:
			break;
	}
	// Pipe rest of commands
	command = command->next;
	while(command!=NULL){
		PipeRest(command);
		close(command->fd[0]); close(command->fd[1]);
		command = command->next;
	}
	
	while((pid=wait(&status))!=-1)
		fprintf(stderr,"process %d exits with %d\n", pid, WEXITSTATUS(status));
	
	return 1;
}

/**
*	Pipe Rest - Pipes the rest of the commands through
*	Param - Linked list of commands
*	Return - 0 when complete
*
*/
int PipeRest(struct Command* command){
	if(command->next!=NULL){
		pipe(command->next->fd);	
	}
	int pid;
	// Fork and execute	
	switch(pid=fork()){
		case 0:
			if(command->next!=NULL){
				dup2(command->next->fd[1],1);
			}
			dup2(command->fd[0],0);
			close(command->fd[1]);
			execvp(command->args[0],command->args);
		default:
			break;
	}
	return 0;
}

/**
*	Print Command List - Prints list of commands for easy debugging
*	Param - Linked list of commands
*	Return - None (prints list of commands)
*
*/
void PrintCommandList(struct Command* command){
	struct Command* ptr = command;
	int i;
	while(ptr!=NULL){
		for(i = 0; i < ptr->argcount; i++){
			printf("%s, ",ptr->args[i]);
		}	
		ptr = ptr->next;
	}
}

/**
*	Destroy Command List - Frees command list to free memory
*	Param - Linked list of commands
*	Return - None
*
*/
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

/**
*	Built in change directory - Changes directory to directory listed in the first command
*	Param - Linked List of commands
*	Return - Return 1 if no error. 0 otherwise
*
*/
int f_cd(struct Command* command){
	// Given no second argument, go home
	if(command->argcount==1){
		if(chdir(getenv("HOME"))==0){
			return 1;		
		}
		else{	
			printf("ERROR: cd failed, couldn't return to home directory\n");
			return 0;
		}
	}
	// If there are too many arguments
	else if(command->argcount>2){
		printf("ERROR: too many arguments\n");
		return 0;
	}
	// Otherwise, go to specified directory
	else{
		if(chdir(command->args[1])==0){
			return 1;
		}
		else{
			printf("ERROR: cd failed, directory doesn't exist\n");
			return 0;
		}
	}
}

/**
*	Built in Exit - Exits out of shell display the second argument of exit command.
*	Param - Linked list of commands
*	Return - None (Exits)
*
*/
int f_exit(struct Command* command){
	if(!command->args[1]){
		exit(0);
	}
	else{
		exit(atoi(command->args[1]));
	}
}

/**
*	Make builtins - Set list of builtins to check commands against
*	Param - None
*	Return - List of builtins in our shell
*/
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

/**
*	Destroy Builtins - Frees builtins, freeing memory
*	Param - Linked List of builtins
*	Return - None
*/
void DestroyBuiltins(struct Builtins* builtins){
	free(builtins->next);
	free(builtins);
}
