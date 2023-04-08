/*
ID: 2020A7PS1214P	Name: Darshan Abhaykumar
ID: 2020A7PS0970P	Name: Debjit Kar
ID:2020A7PS0986P	Name: Nidhish Parekh
*/
#ifndef TREE_H
#define TREE_H
#include "treeDef.h"

TREENODE initTree();
TREENODE createEmptyNode();
void leftmostDerive(NODE deriv, TREENODE t1, int line_no, int rule_no);
void runTree(void);
void printTree(TREENODE t1, int level);

#endif