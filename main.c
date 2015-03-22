#include "shell.h"

int main(){
	char input[BUFFERSIZE];
	struct Builtins* builtins = MakeBuiltins();
	while(1){
		ReadLines(input);
		struct Command* commandList = ParseCommandLine(input);
		PrintCommandList(commandList);

		//execute commands somewhere around here

		DestroyCommandList(commandList);
	}
}