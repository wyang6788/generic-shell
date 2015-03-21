#include <unistd.h>
#include "tokenizer.h"
#define BUFFERSIZE 1000

typedef struct Command{
	char* args[50];
	int fd;
	int argcount;
	struct Command* nextCommand;
} Command_t;

struct Command* MakeCommand();

void ReadLines(char* input);

struct Command* ParseCommandLine(char* input);

void PrintCommandList(struct Command* command);

void DestroyCommandList(struct Command* command);
