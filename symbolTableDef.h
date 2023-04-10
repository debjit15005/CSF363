/*
Group No. : 42
ID: 2020A7PS1214P	Name: Darshan Abhaykumar
ID: 2020A7PS0970P	Name: Debjit Kar
ID:2020A7PS0986P	Name: Nidhish Parekh
*/
#ifndef SYMBOLTABLEDEF_H
#define SYMBOLTABLEDEF_H
#include "tree.h"
#include "ast.h"
#include "lexer.h"

typedef struct {
    int valid;
    int line; // Line number for ERROR generation
    int offset; // Offset
    int width; // Width
    char r1[10]; // Array Low Range
    char r2[10]; // Array High Range
    int isStatic; // Static or Dynamic Array
    int isArray; // Array or not
    int type; // type of element
    int scope[2]; // scope line
    // char scopeName[MAX_LEXEME]; 
    char lexeme[MAX_LEXEME]; // variable name
} SymTableEntry;

struct SymTable{
    struct SymTable* firstChild;
    struct SymTable* nextSibling;
    struct SymTable* parent;
    int nesting; // Nesting level
    char moduleName[MAX_LEXEME]; // scope name
    SymTableEntry entries[MODULO]; // Array of entries
};

typedef struct SymTable* SYMTABLE;


typedef struct {
    SYMTABLE firstChild;
    int valid;
    char lexeme[MAX_LEXEME]; // module name
    TREENODE ip_list; /* CHECK IF THESE ARE NEEDED */
    TREENODE op_list; /* CHECK IF THESE ARE NEEDED */
} GlobalSymTable;

typedef GlobalSymTable* GLOBALSYMTABLE;

struct recurseStruct {
    int line; // Line number for ERROR generation
    int offset; // Offset
    int width; // Width
    char r1[10]; // Array Low Range
    char r2[10]; // Array High Range
    int isStatic; // Static or Dynamic Array
    int isArray; // Array or not
    token type; // type of element
    int scope[2]; // scope line
    int numval; // numeric value if any
};

typedef struct recurseStruct* RECURSESTRUCT;


#endif