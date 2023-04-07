#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "grammar.h"
#include "parser.h"
#include "tree.h"
#include "ast.h"
#include "stack_ll.h"
#include "linkedlist.h"
#include "Nlinkedlist.h"
#include "hashtable.h"


ASTNODE createAST(TREENODE parseTree)
{
    ASTNODE asTree = doRecursion(parseTree, NULL);
    asTree->tnt = 1;
    asTree->val.nt_val = program;
    asTree->line_no = -1; // CHECK
    
    return asTree;
}

void setASTChild(ASTNODE asTree, ASTNODE childNode) // child_count is 0-indexed
{
    childNode->parent = asTree;
    if(asTree->firstChild == NULL)
    {
        asTree->firstChild = childNode;
    }
    else
    {
        asTree = asTree->firstChild;
        while(asTree->nextSibling != NULL)
        {
            asTree = asTree->nextSibling;
        }
        asTree->nextSibling = childNode;
    }
}

ASTNODE getASTChild(ASTNODE asTree, int child_count) // child_count is 0-indexed
{
    asTree = asTree->firstChild;
    while(child_count-->0)
    {
        asTree = asTree->nextSibling;
    }
    return asTree;
}

TREENODE getParseChild(TREENODE parseTree, int child_count) // child_count is 0-indexed
{
    parseTree = parseTree->firstChild;
    while(child_count-->0)
    {
        parseTree = parseTree->nextSibling;
    }
    return parseTree;
}

ASTNODE convertToAST(TREENODE parseTree)
{
    ASTNODE asTree = (ASTNODE) malloc(sizeof(struct ASTNode));
    asTree->tnt = parseTree->tnt;
    if(parseTree->tnt == 0) asTree->val.t_val = parseTree->val.t_val;
    else asTree->val.nt_val = parseTree->val.nt_val;
    asTree->line_no = parseTree->line_no;
    asTree->firstChild = NULL;
    asTree->nextSibling = NULL;    
    asTree->parent = NULL;
    return asTree;
}

void printAST(ASTNODE node, int level)
{
    while (node != NULL)
    {
        for (int i = 0; i < level; i++) printf("\t");
        printf("Level %d Node -> ", level);
        if(node->tnt == 0) printT(node->val.t_val);
        else 
        {
            printNT(node->val.nt_val);
        }
        printf("\n");

        if (node->firstChild != NULL)
        {
            for (int i = 0; i < level; i++) printf("\t");
            printf("Children: \n");
            printAST(node->firstChild, level + 1);
        }
        node = node->nextSibling;
    }
}


ASTNODE doRecursion(TREENODE parseTree, ASTNODE asTree)
{
    int production_rule = parseTree->rule_no;
    printf("Production rule is: %d\n", production_rule);
    fflush(stdout);

    ASTNODE node = malloc(sizeof(struct ASTNode));
    node->tnt = parseTree->tnt;
    if(parseTree->tnt == 0)
    {   
        token t = parseTree->val.t_val;
        node->val.t_val = t;
        // if(t == ID || t == NUM || t == RNUM)
        // {
        //     node->lexeme = (char *) malloc(MAX_LEXEME);
        //     // ADD CODE TO ASSIGN CORRESPONDING LEXEME
        // }
    } 
    else node->val.nt_val = parseTree->val.nt_val;
    // node->line_no = -1;
    node->firstChild = NULL;
    node->nextSibling = NULL;    
    node->parent = NULL; 


    if(production_rule == 0)    
    {
        //<program> → <moduleDeclarations><otherModules><driverModule><otherModules>

        for(int i = 0; i<4; i++)
        {
            TREENODE childNode = getParseChild(parseTree, i);
            ASTNODE recurseNode = doRecursion(childNode, NULL);
            if(recurseNode != NULL) setASTChild(node, recurseNode);
        }
    }
    else if(production_rule == 1)
    {
        //<moduleDeclarations> → <moduleDeclaration><moduleDeclarations>

        free(node);
        TREENODE childNode1 = getParseChild(parseTree, 0);
        ASTNODE moduleNode = doRecursion(childNode1, NULL);
        TREENODE childNode2 = getParseChild(parseTree, 1);
        ASTNODE moduleNode2 = doRecursion(childNode2, NULL);
        if(moduleNode2 != NULL) setASTChild(moduleNode, moduleNode2);
        return moduleNode;

    }
    else if( production_rule == 2)
    {
        //<moduleDeclarations> → EPSILON

        free(node);
        return NULL;
    }
    else if( production_rule == 3)
    {
        //<moduleDeclaration> →  DECLARE MODULE ID SEMICOL
            
        TREENODE childNode = getParseChild(parseTree, 2);
        setASTChild(node, doRecursion(childNode, NULL));
    }
    else if( production_rule == 4)
    {
        //<otherModules> →  <module><otherModules>

        free(node);
        TREENODE childNode1 = getParseChild(parseTree, 0);
        ASTNODE moduleNode = doRecursion(childNode1, NULL);
        TREENODE childNode2 = getParseChild(parseTree, 1);
        ASTNODE moduleNode2 = doRecursion(childNode2, NULL);
        if(moduleNode2 != NULL) setASTChild(moduleNode, moduleNode2);
        return moduleNode;
    
    }
    else if( production_rule == 5)
    {
        //<otherModules> → EPSILON
            
        free(node);
        return NULL;
    }
    else if( production_rule == 6)
    {
        //<driverModule> → DRIVERDEF DRIVER PROGRAM DRIVERENDDEF <moduleDef>

        TREENODE childNode = getParseChild(parseTree, 4);
        setASTChild(node, doRecursion(childNode, NULL));
    
    }
    else if( production_rule == 7)
    {
        //<module> → DEF MODULE ID ENDDEF TAKES INPUT SQBO <input_plist> SQBC SEMICOL <ret> <moduleDef>
            
        TREENODE childNode1 = getParseChild(parseTree, 2);
        setASTChild(node, doRecursion(childNode1, NULL));
        TREENODE childNode2 = getParseChild(parseTree, 7);
        setASTChild(node, doRecursion(childNode2, NULL));
        TREENODE childNode3 = getParseChild(parseTree, 10);
        setASTChild(node, doRecursion(childNode3, NULL));
        TREENODE childNode4 = getParseChild(parseTree, 11);
        setASTChild(node, doRecursion(childNode4, NULL));
    
    }
    else if( production_rule == 8)
    {
        //<ret> → RETURNS SQBO <output_plist> SQBC SEMICOL
            
        TREENODE childNode = getParseChild(parseTree, 2);
        setASTChild(node, doRecursion(childNode, NULL));
    
    }
    else if( production_rule == 9)
    {
        //<ret> → EPSILON
            
        return node;
    
    }
    else if( production_rule == 10)
    {
        //<input_plist> → ID COLON <dataType> <input_plist2>

        free(node);
        TREENODE childNode0 = getParseChild(parseTree, 0);
        ASTNODE inputNode0 = doRecursion(childNode0, NULL);

        TREENODE childNode1 = getParseChild(parseTree, 2);
        ASTNODE inputNode = doRecursion(childNode1, NULL);

        ASTNODE idTypeN = (ASTNODE) malloc(sizeof(struct ASTNode));
        idTypeN->tnt = 1;
        idTypeN->val.nt_val = idType; // ASSIGNED A NEW CONSTRUCT
        setASTChild(idTypeN, inputNode0);
        setASTChild(idTypeN, inputNode);

        TREENODE childNode2 = getParseChild(parseTree, 3);
        ASTNODE inputNode2 = doRecursion(childNode2, NULL);
        if(inputNode2 != NULL) setASTChild(idTypeN, inputNode2);
        return idTypeN;
    
    }
    else if( production_rule == 11)
    {
        //<input_plist2> → COMMA ID COLON <dataType> <input_plist2>

        free(node);
        TREENODE childNode0 = getParseChild(parseTree, 1);
        ASTNODE inputNode0 = doRecursion(childNode0, NULL);

        TREENODE childNode1 = getParseChild(parseTree, 3);
        ASTNODE inputNode = doRecursion(childNode1, NULL);

        ASTNODE idTypeN = (ASTNODE) malloc(sizeof(struct ASTNode));
        // ASSIGN A NEW CONSTRUCT
        idTypeN->tnt = 1;
        idTypeN->val.nt_val = idType;
        setASTChild(idTypeN, inputNode0);
        setASTChild(idTypeN, inputNode);

        TREENODE childNode2 = getParseChild(parseTree, 4);
        ASTNODE inputNode2 = doRecursion(childNode2, NULL);
        if(inputNode2 != NULL) setASTChild(idTypeN, inputNode2);
        return idTypeN;
    
    }
    else if( production_rule == 12)
    {
        //<input_plist2> → EPSILON
            
        free(node);
        return NULL;
    
    }
    else if( production_rule == 13)
    {
        //<output_plist> → ID COLON <type> <output_plist2>
            
        free(node);
        TREENODE childNode0 = getParseChild(parseTree, 0);
        ASTNODE inputNode0 = doRecursion(childNode0, NULL);

        TREENODE childNode1 = getParseChild(parseTree, 2);
        ASTNODE inputNode = doRecursion(childNode1, NULL);

        ASTNODE idTypeN = (ASTNODE) malloc(sizeof(struct ASTNode));
        // ASSIGN A NEW CONSTRUCT
        idTypeN->tnt = 1;
        idTypeN->val.nt_val = idType; 
        setASTChild(idTypeN, inputNode0);
        setASTChild(idTypeN, inputNode);

        TREENODE childNode2 = getParseChild(parseTree, 3);
        ASTNODE inputNode2 = doRecursion(childNode2, NULL);
        if(inputNode2 != NULL) setASTChild(idTypeN, inputNode2);
        return idTypeN;
    
    }
    else if( production_rule == 14)
    {
        //<output_plist2> → COMMA ID COLON <type> <output_plist2>
            
        free(node);
        TREENODE childNode0 = getParseChild(parseTree, 1);
        ASTNODE inputNode0 = doRecursion(childNode0, NULL);

        TREENODE childNode1 = getParseChild(parseTree, 3);
        ASTNODE inputNode = doRecursion(childNode1, NULL);

        ASTNODE idTypeN = (ASTNODE) malloc(sizeof(struct ASTNode));
        // ASSIGN A NEW CONSTRUCT
        idTypeN->tnt = 1;
        idTypeN->val.nt_val = idType;
        setASTChild(idTypeN, inputNode0);
        setASTChild(idTypeN, inputNode);

        TREENODE childNode2 = getParseChild(parseTree, 4);
        ASTNODE inputNode2 = doRecursion(childNode2, NULL);
        if(inputNode2 != NULL) setASTChild(idTypeN, inputNode2);
        return idTypeN;
    
    }
    else if( production_rule == 15)
    {
        //<output_plist2> → EPSILON
            
        free(node);
        return NULL;
    
    }
    else if( production_rule == 16)
    {
        //<dataType> → INTEGER
            
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    
    }
    else if( production_rule == 17)
    {
        //<dataType> → REAL
            
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    
    }
    else if( production_rule == 18)
    {
        //<dataType> → BOOLEAN
            
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    
    }
    else if( production_rule == 19)
    {
        //<dataType> → ARRAY SQBO <arr_range> SQBC OF <type>
            
        free(node);
        TREENODE childNode1 = getParseChild(parseTree, 2);
        TREENODE childNode2 = getParseChild(parseTree, 5);
        ASTNODE inputNode0 = doRecursion(childNode1, NULL);
        ASTNODE inputNode = doRecursion(childNode2, NULL);
        ASTNODE arrayTypeN = (ASTNODE) malloc(sizeof(struct ASTNode));
        // ASSIGN A NEW CONSTRUCT
        arrayTypeN->tnt = 1;
        arrayTypeN->val.nt_val = arrType;
        
        setASTChild(arrayTypeN, inputNode0);
        setASTChild(arrayTypeN, inputNode);
        return arrayTypeN;
    }
    else if( production_rule == 20)
    {
        //<arr_range>→ <arr_index> RANGEOP <arr_index>
            
        free(node);
        TREENODE childNode1 = getParseChild(parseTree, 0);
        TREENODE childNode2 = getParseChild(parseTree, 2);
        ASTNODE inputNode0 = doRecursion(childNode1, NULL);
        ASTNODE inputNode = doRecursion(childNode2, NULL);
        ASTNODE arrayTypeN = (ASTNODE) malloc(sizeof(struct ASTNode));
        // ASSIGN A NEW CONSTRUCT
        arrayTypeN->tnt = 1;
        arrayTypeN->val.nt_val = arrRangeType;

        setASTChild(arrayTypeN, inputNode0);
        setASTChild(arrayTypeN, inputNode);
        return arrayTypeN;
    }
    else if( production_rule == 21)
    {
        //<type> → INTEGER
            
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    
    }
    else if( production_rule == 22)
    {
        //<type> → REAL
            
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    
    }
    else if( production_rule == 23)
    {
        //<type> → BOOLEAN
            
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    
    }
    else if( production_rule == 24)
    {
        //<moduleDef> → START <statements> END


        TREENODE childNode = getParseChild(parseTree, 1);
        return(doRecursion(childNode, NULL));

    }
    else if( production_rule == 25)
    {
        //<statements> → <statement> <statements>

        free(node);
        TREENODE childNode1 = getParseChild(parseTree, 0);
        ASTNODE statementNode = doRecursion(childNode1, NULL);
        TREENODE childNode2 = getParseChild(parseTree, 1);
        ASTNODE statementNode2 = doRecursion(childNode2, NULL);
        if(statementNode2 != NULL) setASTChild(statementNode, statementNode2);
        return statementNode;
    
    }
    else if( production_rule == 26)
    {
        //<statements> → EPSILON

        free(node);
        return NULL;
    }
    else if( production_rule == 27)
    {
        //<statement> → <ioStmt>
            
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return(doRecursion(childNode, NULL));
    
    }
    else if( production_rule == 28)
    {
        //<statement> → <simpleStmt>
            
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return(doRecursion(childNode, NULL));
    
    }
    else if( production_rule == 29)
    {
        //<statement> → <declareStmt>
            
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return(doRecursion(childNode, NULL));
    
    }
    else if( production_rule == 30)
    {
        //<statement> → <conditionalStmt>
            
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return(doRecursion(childNode, NULL));
    
    }
    else if( production_rule == 31)
    {
        //<statement> → <iterativeStmt>
            
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return(doRecursion(childNode, NULL));
    
    }
    else if( production_rule == 32)
    {
        //<ioStmt> → GET_VALUE BO ID BC SEMICOL
            
        free(node);
        TREENODE childNode = getParseChild(parseTree, 2);
        ASTNODE inputNode0 = doRecursion(childNode, NULL);
        ASTNODE getValN = (ASTNODE) malloc(sizeof(struct ASTNode));
        getValN->tnt = 1;
        getValN->val.nt_val = getValue;
        setASTChild(getValN, inputNode0);
        return getValN;
    }
    else if( production_rule == 33)
    {
        //<ioStmt> → PRINT BO <print_var> BC SEMICOL
            
        free(node);
        TREENODE childNode = getParseChild(parseTree, 2);
        ASTNODE inputNode0 = doRecursion(childNode, NULL);
        ASTNODE printValN = (ASTNODE) malloc(sizeof(struct ASTNode));
        printValN->tnt = 1;
        printValN->val.nt_val = printValue;
        setASTChild(printValN, inputNode0);
        return printValN;
    
    }
    else if( production_rule == 34)
    {
        //<boolVal> → TRUE 
            
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    
    }
    else if( production_rule == 35)
    {
        //<boolVal> → FALSE
            
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    
    }
    else if( production_rule == 36)
    {
        //<print_var> → ID <which_ID>
        free(node);
        TREENODE childNode1 = getParseChild(parseTree, 0);
        ASTNODE idNode = doRecursion(childNode1, NULL);
        TREENODE childNode2 = getParseChild(parseTree, 1);
        ASTNODE idNode2 = doRecursion(childNode2, idNode);
        return idNode2;
    
    }
    else if( production_rule == 37)
    {
        //<print_var> → NUM
            
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    
    }
    else if( production_rule == 38)
    {
        //<print_var> → RNUM
            
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    }
    else if( production_rule == 39)
    {
        //<print_var> → <boolVal>
        
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return(doRecursion(childNode, NULL));
    
    }
    else if( production_rule == 40)
    {
        //<which_ID> → SQBO <num_or_id> SQBC
        free(node);
        TREENODE childNode = getParseChild(parseTree, 1);
        ASTNODE inputNode0 = doRecursion(childNode, NULL);
        ASTNODE whichIDN = (ASTNODE) malloc(sizeof(struct ASTNode));
        whichIDN->tnt = 1;
        whichIDN->val.nt_val = arrElement;
        setASTChild(whichIDN, inputNode0);
        setASTChild(whichIDN, asTree);
        return whichIDN;
    }
    else if( production_rule == 41)
    {
        //<which_ID> → EPSILON
        free(node);
        return asTree;
    
    }
    else if( production_rule == 42)
    {
        //<simpleStmt> → <assignmentStmt>

        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return(doRecursion(childNode, NULL));
    
    }
    else if( production_rule == 43)
    {
        //<simpleStmt> → <moduleReuseStmt>

        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return(doRecursion(childNode, NULL));
    
    }
    else if( production_rule == 44)
    {
        //<assignmentStmt> → ID <whichStmt>
    
    
    }
    else if( production_rule == 45)
    {
        //<whichStmt> → <lvalueIDStmt>
    
    
    }
    else if( production_rule == 46)
    {
        //<whichStmt> → <lvalueARRStmt>
    
    
    }
    else if( production_rule == 47)
    {
        //<lvalueIDStmt> → ASSIGNOP <expression> SEMICOL
    
    
    }
    else if( production_rule == 48)
    {
        //<lvalueARRStmt> → SQBO <exprIndex> SQBC ASSIGNOP <expression> SEMICOL
    
    
    }
    else if( production_rule == 49)
    {
        //<arr_index> → <sign> <num_or_id>

        free(node);
        TREENODE childNode1 = getParseChild(parseTree, 0);
        TREENODE childNode2 = getParseChild(parseTree, 1);
        ASTNODE inputNode0 = doRecursion(childNode1, NULL);
        ASTNODE inputNode = doRecursion(childNode2, NULL);
        ASTNODE signedNumN = (ASTNODE) malloc(sizeof(struct ASTNode));
        // ASSIGN A NEW CONSTRUCT
        signedNumN->tnt = 1;
        signedNumN->val.nt_val = signedNum;

        setASTChild(signedNumN, inputNode0);
        setASTChild(signedNumN, inputNode);
        return signedNumN;
    
    }
    else if( production_rule == 50)
    {
        //<num_or_id> → NUM

        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    
    }
    else if( production_rule == 51)
    {
        //<num_or_id> → ID

        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    
    }
    else if( production_rule == 52)
    {
        //<sign> → PLUS

        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    
    }
    else if( production_rule == 53)
    {
        //<sign> → MINUS

        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    }
    else if( production_rule == 54)
    {
        //<sign> → ε

        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    
    }
    else if( production_rule == 55)
    {
        //<moduleReuseStmt> → <optional> USE MODULE ID WITH PARAMETERS <actual_para_list> SEMICOL
    
    
    }
    else if( production_rule == 56)
    {
        //<factor> → NUM
    
    
    }
    else if( production_rule == 57)
    {
        //<factor> → RNUM 
    
    
    }
    else if( production_rule == 58)
    {
        //<factor> → <boolVal>
    
    }
    else if( production_rule == 59)
    {
        //<factor> → ID <factor2>
    
    }
    else if( production_rule == 60)
    {
        //<factor2> → SQBO <exprIndex> SQBC
    
    }
    else if( production_rule == 61)
    {
        //<factor2> → EPSILON
    
    
    }
    else if( production_rule == 62)
    {
        //<optional> -> SQBO <idList> SQBC ASSIGNOP
    
    
    }
    else if( production_rule == 63)
    {
        //<optional> -> EPSILON
    
    
    }
    else if( production_rule == 64)
    {
        //<idList> -> ID <idList2>

        free(node);
        TREENODE childNode1 = getParseChild(parseTree, 0);
        ASTNODE idNode = doRecursion(childNode1, NULL);
        TREENODE childNode2 = getParseChild(parseTree, 1);
        ASTNODE idNode2 = doRecursion(childNode2, NULL);
        if(idNode2 != NULL) setASTChild(idNode, idNode2);
        return idNode;
    
    
    }
    else if( production_rule == 65)
    {
        //<idList2> -> COMMA ID <idList2>

        free(node);
        TREENODE childNode1 = getParseChild(parseTree, 1);
        ASTNODE idNode = doRecursion(childNode1, NULL);
        TREENODE childNode2 = getParseChild(parseTree, 2);
        ASTNODE idNode2 = doRecursion(childNode2, NULL);
        if(idNode2 != NULL) setASTChild(idNode, idNode2);
        return idNode;
    
    
    }
    else if( production_rule == 66)
    {
        //<idList2> -> EPSILON

        free(node);
        return NULL;
    
    }
    else if( production_rule == 67)
    {
        //<expression> → <abExpr>
    
    
    }
    else if( production_rule == 68)
    {
        //<abExpr> → <U>
    
    
    }
    else if( production_rule == 69)
    {
        //<U> → <uni_op><new_NT>
    
    
    }
    else if( production_rule == 70)
    {
        //<new_NT> → BO <arithmeticExpr> BC
    
    
    }
    else if( production_rule == 71)
    {
        //<new_NT> → <con_var>
    
    
    }
    else if( production_rule == 72)
    {
        //<uni_op> → PLUS
    
    
    }
    else if( production_rule == 73)
    {
        //<uni_op> → MINUS
    
    
    }
    else if( production_rule == 74)
    {
        //<abExpr> → <AnyTerm><Term7>
    
    
    }
    else if( production_rule == 75)
    {
        //<Term7> → <logicalOp><AnyTerm><Term7>
    
    
    }
    else if( production_rule == 76)
    {
        //<Term7> → EPSILON
    
    
    }
    else if( production_rule == 77)
    {
        //<AnyTerm> → <arithmeticExpr><Term8>
    
    
    }
    else if( production_rule == 78)
    {
        //<AnyTerm> → <boolVal>
    
    
    }
    else if( production_rule == 79)
    {
        //<Term8> → <relationalOp><arithmeticExpr>
    
    
    }
    else if( production_rule == 80)
    {
        //<Term8> → EPSILON
    
    
    }
    else if( production_rule == 81)
    {
        //<con_var> → ID
    
    
    }
    else if( production_rule == 82)
    {
        //<con_var> → NUM
    
    
    }
    else if( production_rule == 83)
    {
        //<con_var> → RNUM
    
    
    }
    else if( production_rule == 84)
    {
        //<factor> → BO <abExpr> BC
    
    
    }
    else if( production_rule == 85)
    {
        //<arithmeticExpr> → <term><arithmeticExpr2>
    
    
    }
    else if( production_rule == 86)
    {
        //<arithmeticExpr2> → <low_op><term><arithmeticExpr2>
    
    
    }
    else if( production_rule == 87)
    {
        //<arithmeticExpr2> → EPSILON
    
    
    }
    else if( production_rule == 88)
    {
        //<term> → <factor><term2>
    
    
    }
    else if( production_rule == 89)
    {
        //<term2> -> <high_op><factor><term2>
    
    
    }
    else if( production_rule == 90)
    {
        //<term2> → EPSILON
    
    
    }
    else if( production_rule == 91)
    {
        //<exprIndex> → <sign> <exprIndex2>
    
    
    }
    else if( production_rule == 92)
    {
        //<exprIndex> → <arrExpr>
    
    
    }
    else if( production_rule == 93)
    {
        //<exprIndex2> → <num_or_id>
    
    
    }
    else if( production_rule == 94)
    {
        //<exprIndex2> → BO <arrExpr> BC
    
    
    }
    else if( production_rule == 95)
    {
        //<arrExpr> → <arrTerm> <arrExpr2>
    
    
    }
    else if( production_rule == 96)
    {
        //<arrExpr2> → <low_op> <arrTerm> <arrExpr2>
    
    
    }
    else if( production_rule == 97)
    {
        //<arrExpr2> → EPSILON
    
    
    }
    else if( production_rule == 98)
    {
        //<arrTerm> → <arrFactor> <arrTerm2>
    
    
    }
    else if( production_rule == 99)
    {
        //<arrTerm2> → <high_op> <arrFactor> <arrTerm2>
    
    
    }
    else if( production_rule == 100)
    {
        //<arrTerm2> → EPSILON
    
    
    }
    else if( production_rule == 101)
    {
        //<arrFactor> → ID
    
    
    }
    else if( production_rule == 102)
    {
        //<arrFactor> → NUM
    
    
    }
    else if( production_rule == 103)
    {
        //<arrFactor> → <boolVal>
    
    
    }
    else if( production_rule == 104)
    {
        //<arrFactor> → BO <arrExpr> BC
    
    
    }
    else if( production_rule == 105)
    {
        //<low_op> → PLUS
    
    
    }
    else if( production_rule == 106)
    {
        //<low_op> → MINUS
    
    
    }
    else if( production_rule == 107)
    {
        //<high_op> → MUL
    
    
    }
    else if( production_rule == 108)
    {
        //<high_op> → DIV
    
    
    }
    else if( production_rule == 109)
    {
        //<logicalOp> → AND
    
    
    }
    else if( production_rule == 110)
    {
        //<logicalOp> → OR
    
    
    }
    else if( production_rule == 111)
    {
        //<relationalOp> →  LT
    
    
    }
    else if( production_rule == 112)
    {
        //<relationalOp> → LE
    
    
    }
    else if( production_rule == 113)
    {
        //<relationalOp> → GT
    
    
    }
    else if( production_rule == 114)
    {
        //<relationalOp> → GE
    
    
    }
    else if( production_rule == 115)
    {
        //<relationalOp> → EQ
    
    
    }
    else if( production_rule == 116)
    {
        //<relationalOp> → NE
    
    
    }
    else if( production_rule == 117)
    {
        //<declareStmt> →  DECLARE <idList> COLON <dataType> SEMICOL

        TREENODE childNode1 = getParseChild(parseTree, 1);
        setASTChild(node, doRecursion(childNode1, NULL));
        TREENODE childNode2 = getParseChild(parseTree, 3);
        setASTChild(node, doRecursion(childNode2, NULL));
    }
    else if( production_rule == 118)
    {
        //<conditionalStmt> → SWITCH BO ID BC START <caseStmts><default_stmt> END
    
    
    }
    else if( production_rule == 119)
    {
        //<caseStmts> → CASE <value> COLON <statements> BREAK SEMICOL <caseStmts2>
    
    
    }
    else if( production_rule == 120)
    {
        //<caseStmts2> → CASE <value> COLON <statements> BREAK SEMICOL <caseStmts2>
    
    
    }
    else if( production_rule == 121)
    {
        //<caseStmts2> → EPSILON
    
    
    }
    else if( production_rule == 122)
    {
        //<value> → NUM
    
    
    }
    else if( production_rule == 123)
    {
        //<value> → TRUE
    
    
    }
    else if( production_rule == 124)
    {
        //<value> → FALSE
    
    
    }
    else if( production_rule == 125)
    {
        //<default_stmt> → DEFAULT COLON <statements> BREAK SEMICOL
    
    
    }
    else if( production_rule == 126)
    {
        //<default_stmt> → EPSILON 
    
    
    }
    else if( production_rule == 127)
    {
        //<iterativeStmt> → FOR BO ID IN <for_range> BC START <statements> END
    
    
    }
    else if( production_rule == 128)
    {
        //<iterativeStmt> → WHILE BO <abExpr> BC START <statements> END
    
    
    }
    else if( production_rule == 129)
    {
        //<for_range> → <for_index> RANGEOP <for_index>
    
    
    }
    else if( production_rule == 130)
    {
        //<for_index> → <for_sign> <for_index2>
    
    
    }
    else if( production_rule == 131)
    {
        //<for_index2> → NUM
    
    
    }
    else if( production_rule == 132)
    {
        //<for_sign> → PLUS
    
    
    }
    else if( production_rule == 133)
    {
        //<for_sign> → MINUS
    
    
    }
    else if( production_rule == 134)
    {
        //<for_sign> → EPSILON


    }
    else if( production_rule == 135)
    {
        //<actual_para_list> → <sign> <K> <Term9>
    
    }
    else if( production_rule == 136)
    {
        //<Term9> → COMMA <sign> <K> <Term9>
    
    }
    else if( production_rule == 137)
    {
        //<Term9> → EPSILON
    
    }
    else if( production_rule == 138)
    {
        //<K> → NUM
    
    }
    else if( production_rule == 139)
    {
        //<K> → RNUM
    
    }
    else if( production_rule == 140)
    {
        //<K> → <boolVal>
    
    }
    else if( production_rule == 141)
    {
        //<K> → ID <actual_para_list2>
    
    }
    else if( production_rule == 142)
    {
        //<actual_para_list2> → SQBO <exprIndex> SQBC
    
    }
    else if( production_rule == 143)
    {
        //<actual_para_list2> → EPSILON
    
    }
    return node;
}


