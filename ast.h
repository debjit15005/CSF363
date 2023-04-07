#ifndef AST_H
#define AST_H
#include "astDef.h"

ASTNODE createAST(TREENODE parseTree);
void printAST(ASTNODE node, int level);
ASTNODE doRecursion(TREENODE parseTree, ASTNODE asTree);

#endif