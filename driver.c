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
	ASTNODE asTree = parseInputSourceCode("input.txt", parseTable);
	// printAST(asTree, 0);
	initSymTable();
	createSymTable(asTree);
	printGlobalTable(gSymTable);

	return 0; 
}