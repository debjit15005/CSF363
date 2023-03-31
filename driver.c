#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "grammar.h"
#include "parser.h"
#include "tree.h"
#include "stack_ll.h"
#include "linkedlist.h"
#include "Nlinkedlist.h"
#include "hashtable.h"

int main(){
	// buildGrammar();
	// computeFirsts();
	// printf("Firsts done\n");
	// printFirsts();
	// printf("***********************************************\n");
	// computeFollows();
	// printf("Follows done\n");
	// // // printFollows();
	// printf("***********************************************\n");
	// createParseTable();
	// int** parseTable = getParseTable();
	// printf("ParseTable done\n");
	
	// // int** parseTable = getParseTable();
	// // printParseTable(); 
	// parseInputSourceCode("input.txt",parseTable);
	// tokenInfo* parserToken = runLexerForParser("input.txt", 10);
	// printToken(parserToken);

	
	// Stack* test = newStack();
	// NODE t1 = createNewTerm(0,0,1);
	// push(test, t1);
	// pushDerivation(test,table[0][0]); 
	// printStack(test);

	// printf("Test");

	// runLexer("input.txt",10);
	runGrammar();
	













	return 0; 

}