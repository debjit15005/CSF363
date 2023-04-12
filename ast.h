#ifndef AST_H
#define AST_H
#include "astDef.h"
#include "tree.h"


extern int astNodeCount;



ASTNODE createAST(TREENODE parseTree, tokenInfo** reqLexeme);

void printAST(ASTNODE node, int level);
ASTNODE doRecursion(TREENODE parseTree, ASTNODE asTree);
ASTNODE getASTChild(ASTNODE asTree, int child_count);
int countNodes(ASTNODE node);

#endif