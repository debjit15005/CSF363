#ifndef HTABLE_C
#define HTABLE_C
#include "lexer.h"
#include "hashtable.h"
/*
Group No. : 42
ID: 2020A7PS1214P	Name: Darshan Abhaykumar
ID: 2020A7PS0970P	Name: Debjit Kar
ID:2020A7PS0986P	Name: Nidhish Parekh
*/

/*
	This is the menu driven file. To begin executing the code 
	1> Type 'make'
	2> Type './runCompiler <input_file> <parseTreeOutput> size_of_buffer'
*/

// ------------------------------------------
// TODO: Include in lexer.h later
// ------------------------------------------

// int hashcode(char* key);
// token getToken(char *lexeme, Htable htable);
// void insertId(Htable htable, char* key, token sym, int hashed);
// void initTable(Htable htable);

// USING OPEN ADDRESSING FORM OF HASHING
int hashcode(char* key)
{
    int len = strlen(key);
    unsigned long hashed=7;
    for(int i=0; i<len; i++){
        hashed = hashed*31 + key[i];
    }
    return hashed%MODULO;
}

void insertId(Htable htable, char* key, token sym, int hashed)
{   
    if(hashed == -1) hashed = hashcode(key);
    if(htable[hashed].valid == 1) insertId(htable, key, sym, (++hashed)%MODULO);
    else
    {
        strcpy(htable[hashed].lexeme, key);
        htable[hashed].sym = sym;
        htable[hashed].valid = 1;
    }
}   

void initTable(Htable htable){
    insertId(htable,"integer", INTEGER, -1);
    insertId(htable,"real", REAL, -1);
    insertId(htable,"boolean", BOOLEAN, -1);
    insertId(htable,"of", OF, -1);
    insertId(htable,"array", ARRAY, -1);
    insertId(htable,"start", START, -1);
    insertId(htable,"end", END, -1);
    insertId(htable,"declare", DECLARE, -1);
    insertId(htable,"module", MODULE, -1);
    insertId(htable,"driver", DRIVER, -1);
    insertId(htable,"program", PROGRAM, -1);
    insertId(htable,"get_value", GET_VALUE, -1);
    insertId(htable,"print", PRINT, -1);
    insertId(htable,"use", USE, -1);
    insertId(htable,"with", WITH, -1);
    insertId(htable,"parameters", PARAMETERS, -1);
    insertId(htable,"takes", TAKES, -1);
    insertId(htable,"input", INPUT, -1);
    insertId(htable,"returns", RETURNS, -1);
    insertId(htable,"for", FOR, -1);
    insertId(htable,"in", IN, -1);
    insertId(htable,"switch", SWITCH, -1);
    insertId(htable,"case", CASE, -1);
    insertId(htable,"break", BREAK, -1);
    insertId(htable,"default", DEFAULT, -1);
    insertId(htable,"while", WHILE, -1);
    insertId(htable,"true", TRUE, -1);
    insertId(htable,"false", FALSE, -1);
    insertId(htable,"AND", AND, -1);
    insertId(htable,"OR", OR, -1);

}

token getToken(char *lexeme, Htable htable){
	int hashed = hashcode(lexeme);
	while (htable[hashed].valid == 1){
		if(strcmp(htable[hashed].lexeme, lexeme) == 0) return htable[hashed].sym;
		hashed = (++hashed) % MODULO;
	}
	return ID;
}

#endif