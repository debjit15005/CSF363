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
#include "ast.h"
#include "symbolTable.h"

int main(){
	initializeLexerOnce();
	buildGrammar();
	automaticFirsts();
	// printFirsts();
	automaticFollows();
	// printf("\n");
	// printFollows();
	createParseTable();
	int** parseTable = getParseTable();
	// printf("ParseTable done\n");
	// printParseTable(); 
	parseInputSourceCode("input.txt",parseTable);
	
	// Stack* test = newStack();
	// NODE t1 = createNewTerm(0,0,1);
	// push(test, t1);
	// pushDerivation(test,table[0][0]); 
	// printStack(test);

	// printf("Test");

	// runLexer("input.txt",10);
	// runGrammar();
	

	return 0; 
}