/*
ID: 2020A7PS1214P	Name: Darshan Abhaykumar
ID: 2020A7PS0970P	Name: Debjit Kar
ID:2020A7PS0986P	Name: Nidhish Parekh
*/
#ifndef TREEDEF_H
#define TREEDEF_H
#include "lexerDef.h"
#include "linkedlist.h"
#include "grammarDef.h"


struct TreeNode
{
    int tnt; // 0 is terminal and 1 is non terminal
    union {
        token t_val;
        NT nt_val;
    } val;
    int line_no;
    int rule_no;
    struct TreeNode* firstChild;
    struct TreeNode* nextSibling;    
    struct TreeNode* parent;    
};
typedef struct TreeNode* TREENODE;

#endif