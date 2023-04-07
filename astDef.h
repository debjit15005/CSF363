/*
Group No. : 42
ID: 2020A7PS1214P	Name: Darshan Abhaykumar
ID: 2020A7PS0970P	Name: Debjit Kar
ID:2020A7PS0986P	Name: Nidhish Parekh
*/
#ifndef ASTDEF_H
#define ASTDEF_H

struct ASTNode {
    int tnt; // 0 is terminal and 1 is non terminal
    union {
        token t_val;
        NT nt_val;
    } val;
    int line_no;
    char* lexeme;
    struct ASTNode* firstChild;
    struct ASTNode* nextSibling;    
    struct ASTNode* parent; 
};

typedef struct ASTNode * ASTNODE;
#endif