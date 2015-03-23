#include "shell.h"

struct Command* MakeCommand(){
	struct Command* command = malloc(sizeof(struct Command));
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

void ExecuteCommands(struct Command* command){
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

	PipeFirst(command);		
	
	DestroyBuiltins(builtins);
}

int PipeFirst(struct Command* command){
	if(command->next!=NULL){
		pipe(command->next->fd);
	}
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

int PipeRest(struct Command* command){
	if(command->next!=NULL){
		pipe(command->next->fd);	
	}
	int pid;
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
	if(!command->args[0]) {
		printf("Nothing to Execute");
		return;	
	} else {
		execve(command->args[0],args,env);
	}
	perror("execve");
	exit(1);
}

int f_pipe(struct Command* command) {
	int pid;
	if (pipe(command->fd) == 0)
	{
		switch(pid=fork()) {
		case 0:
			dup2(command->fd[0], 0);
			close(command->fd[1]);
			f_execve(command);
			exit(0);
		default:
			dup2(command->fd[1], 1);
			close(command->fd[0]);
			f_execve(command->next);
			exit(0);
		case -1:
			perror("The fork broke");
			exit(1);	
		}
		return 1;
	} else {
		printf("Pipe failed.");
		return 0;
	}
}

int f_cd(struct Command* command){
	if(command->argcount==1){
		if(chdir(getenv("HOME"))==0){
			return 1;		
		}
		else{	
			printf("ERROR: cd failed, couldn't return to home directory\n");
			return 0;
		}
	}
	else if(command->argcount>2){
		printf("ERROR: too many arguments\n");
		return 0;
	}
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

void DestroyBuiltins(struct Builtins* builtins){
	free(builtins->next);
	free(builtins);
}
