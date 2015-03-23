// Group Names: Peter Zhang, William Yang, Ellis Pomirchy, Joseph Pestonit, Philip Beauchea

#include "shell.h"

int main(){
	char input[BUFFERSIZE];
	while(1){
		ReadLines(input);
		struct Command* commandList = ParseCommandLine(input);
		if(commandList!=NULL){
			ExecuteCommands(commandList);
			DestroyCommandList(commandList);
		}
	}
}
