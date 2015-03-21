/*
 * tokenizer.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tokenizer.h"
#include <sys/stat.h>

/*
 * TKCreate creates a new TokenizerT object for a given set of separator
 * characters (given as a string) and a token stream (given as a string).
 * 
 * TKCreate should copy the two arguments so that it is not dependent on
 * them staying immutable after returning.  (In the future, this may change
 * to increase efficiency.)
 *
 * If the function succeeds, it returns a non-NULL TokenizerT.
 * Else it returns NULL.
 *
 * You need to fill in this function as part of your implementation.
 */

TokenizerT *TKCreate(char *separators, char *ts) {
	
	/*
	 * Description: creates a new tokenizer struct from the token stream and delimiters
	 * Parameters: set of delimiters, token stream
	 * Modifies: nothing
	 * Returns: a pointer to a tokenizer struct on success, a null pointer on failure
	 * 
	 */
	 
	if(separators == NULL || ts == NULL){
		return NULL;
	}
	
	TokenizerT* tokenizer = (TokenizerT*)malloc(sizeof(TokenizerT));
	
	if(tokenizer == NULL){
		return NULL;
	}


	tokenizer->delimiters = malloc(strlen(separators)*sizeof(char) +1);
    strcpy(tokenizer->delimiters,separators);
	tokenizer->copied_string = malloc(strlen(ts)*sizeof(char)+1);
    strcpy(tokenizer->copied_string,ts);
	tokenizer->current_position = tokenizer->copied_string;
	
    return tokenizer;
}

/*
 * TKDestroy destroys a TokenizerT object.  It should free all dynamically
 * allocated memory that is part of the object being destroyed.
 *
 * You need to fill in this function as part of your implementation.
 */

void TKDestroy(TokenizerT *tk) {	
	
	/*
	 * Description: destroys tokenizer struct and deallocates all memory
	 * Parameters: tokenizer to be destroyed
	 * Modifies: tokenizer struct: deallocates memory
	 * Returns: nothing 
	 */
	 
	free(tk->copied_string);
	free(tk->delimiters);
	free(tk);
	
	return;
}

char is_usable(char character, char* alphanumeric) {
	
	/*
	 * Description: determines if a particular character is a member of the set of usable chars 
	 * Parameters: character to be compared, string of usable chars
	 * Modifies: Nothing
	 * Returns: 1 if character is usable, 0 if it is not
	 */
	
	char* current = NULL;
	
	for(current = alphanumeric; current - alphanumeric< strlen(alphanumeric); current++) {
		if(character == *current) {
			return 1;
		}
	}
	return 0;
}

char is_delimiter(char character, char* delimiters) {
	
	/*
	 * Description: determines if a particular character is a member of the set of delimiters
	 * Parameters: character to be compared, string of delimiters
	 * Modifies: Nothing
	 * Returns: 1 if character is a delimiter, 0 if it is not
	 */
	
	char* current = NULL;
	
	for(current = delimiters; current - delimiters < strlen(delimiters); current++) {
		if(character == *current) {
			return 1;
		}
	}
	return 0;
}

/*
 * TKGetNextToken returns the next token from the token stream as a
 * character string.  Space for the returned token should be dynamically
 * allocated.  The caller is responsible for freeing the space once it is
 * no longer needed.
 *
 * If the function succeeds, it returns a C string (delimited by '\0')
 * containing the token.  Else it returns 0.
 *
 * You need to fill in this function as part of your implementation.
 */

char* TKGetNextToken(TokenizerT *tk) {

	/*
	 * Description: returns the next token from the token stream specified within the tokenizer
	 * Parameters: tokenizer from which to extract token
	 * Modifies: tokenizer->current_position: identifies starting point of next token; creates a new string with 
	 * Returns: token extracted as a char* on success, null on failure/end of string;
	 */
	
	char* token = NULL;
	char* token_start = NULL;
	int singlequote = 0;
	int doublequote = 0;

	while(tk->current_position - tk->copied_string < strlen(tk->copied_string)) {
		if(*tk->current_position == '\''){
			singlequote = 1;
			tk->current_position++;
			token_start = tk->current_position;
			break;
		}
		else if(*tk->current_position == '\"'){
			doublequote = 1;
			tk->current_position++;
			token_start = tk->current_position;
			break;
		}
		else if(*tk->current_position == '|'){
			tk->current_position++;
			if(tk->current_position == NULL){
				return NULL;
			}
			token = malloc(sizeof(char) * 2);
			token[0] = '|';
			token[1] = '\0';
			return token;
		}
		else if(!is_delimiter(*tk->current_position, tk->delimiters)) {
		
			token_start = tk->current_position;
			break;
		}
		tk->current_position++;
	}
	
	if(token_start == NULL) {
		return NULL;
	}

	if(singlequote == 1){
		while(tk->current_position - tk->copied_string < strlen(tk->copied_string)){
			if(*tk->current_position!='\''){
				tk->current_position++;
			}
			else{
				break;
			}
		}
		if(tk->current_position - tk->copied_string == strlen(tk->copied_string)){
			printf("Mismatched quote. Assuming quote is at the end.\n");
		}
		token = (char*)malloc(sizeof(char) * (tk->current_position - token_start + 1));
		strncpy(token, token_start, tk->current_position - token_start);
		token[(tk->current_position - token_start)] = '\0';

		tk->current_position++;
		return token;	
	}
	else if(doublequote == 1){
		while(tk->current_position - tk->copied_string < strlen(tk->copied_string)){
			if(*tk->current_position!='\"'){
				tk->current_position++;
			}
			else{
				break;
			}
		}
		if(tk->current_position - tk->copied_string == strlen(tk->copied_string)){
			printf("Mismatched quote. Assuming quote is at the end.\n");
		}
		token = (char*)malloc(sizeof(char) * (tk->current_position - token_start + 1));
		strncpy(token, token_start, tk->current_position - token_start);
		token[(tk->current_position - token_start)] = '\0';

		tk->current_position++;
		return token;	
	}
	else{
		while(tk->current_position - tk->copied_string < strlen(tk->copied_string)) {
			if(!is_delimiter(*tk->current_position, tk->delimiters) && *tk->current_position!='|') {
				tk->current_position++;
			}
			else{
				break;
			}
		}
		token = (char*)malloc(sizeof(char) * (tk->current_position - token_start + 1));
		strncpy(token, token_start, tk->current_position - token_start);
		token[(tk->current_position - token_start)] = '\0';
		return token;	
	}
}
