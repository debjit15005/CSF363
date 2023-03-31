#ifndef GRAMMAR_H
#define GRAMMAR_H
#include "grammarDef.h"
#include "linkedlist.h"
#include "stdio.h"

extern RULE** table;

void printNT(NT term);
void fprintNT(FILE* fp, NT term);
void runGrammar();
void buildGrammar();

#endif