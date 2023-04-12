/*
Group No. : 42
ID: 2020A7PS1214P	Name: Darshan Abhaykumar
ID: 2020A7PS0970P	Name: Debjit Kar
ID:2020A7PS0986P	Name: Nidhish Parekh
*/

#ifndef PARSER_H
#define PARSER_H
#include "lexerDef.h"
#include "linkedlist.h"
#include "ast.h"
#include "parserDef.h"

extern tokenInfo** reqLexeme;
extern SLIM scopeStack;
extern int parseTreeNodeCount;
extern int printErrors;
extern int syntacticError;
extern TREENODE t1;

void automaticFirsts();
void automaticFollows();
void computeFirsts();
void computeFollows();

void createParseTable();
ASTNODE parseInputSourceCode(char *testcaseFile, int** parseTable, int printSyntacticErrors);
void printFirsts(void);
void printFollows(void);
void printParseTable();
NODE* getFirsts();
NODE* getFollows();
int** getParseTable();
void runParser();

#endif