#include <unistd.h>
#include "tokenizer.h"
#include <sys/wait.h>
#define BUFFERSIZE 1000

typedef struct Command{
	char* args[50];
	int fd[2];
	int argcount;
	struct Command* next;
} Command_t;

typedef struct Builtins{
	char* name;
	int (*f)();
	struct Builtins* next;
} Builtins_t;

struct Builtins* MakeBuiltins();

struct Command* MakeCommand();

void ReadLines(char* input);

struct Command* ParseCommandLine(char* input);

void ExecuteCommands(struct Command* command);

void PrintCommandList(struct Command* command);

void DestroyCommandList(struct Command* command);

void DestroyBuiltins(struct Builtins* builtins);

int PipeFirst(struct Command* command);

int PipeRest(struct Command* command);

int f_fork(struct Command* command);

void f_execve(struct Command* command);

int f_cd(struct Command* command);

int f_exit(struct Command* command);
