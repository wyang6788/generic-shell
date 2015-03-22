#include <unistd.h>
#include "tokenizer.h"
#define BUFFERSIZE 1000

typedef struct Command{
	char* args[50];
	int fd;
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

void PrintCommandList(struct Command* command);

void DestroyCommandList(struct Command* command);

int f_cd(struct Command* command);

int f_exit(struct Command* command);