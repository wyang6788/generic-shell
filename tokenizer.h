#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_HEX_CHARS 2
#define MAX_OCT_CHARS 3

struct TokenizerT_ {
	char* copied_string;
	char* delimiters;		
	char* current_position;
};

typedef struct TokenizerT_ TokenizerT;

TokenizerT *TKCreate(char *separators, char *ts); 

void TKDestroy(TokenizerT *tk); 

char is_usable(char character, char* alphanumeric); 

char is_delimiter(char character, char* delimiters);

char *TKGetNextToken(TokenizerT *tk);

