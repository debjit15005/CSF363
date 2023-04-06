#ifndef AST_H
#define AST_H
#include "astDef.h"

void createAST(TREENODE parseTree);
ASTNODE doRecursion(TREENODE parseTree, ASTNODE asTree);

#endif