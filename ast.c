/*
Group No. : 42
ID: 2020A7PS1214P	Name: Darshan Abhaykumar
ID: 2020A7PS0970P	Name: Debjit Kar
ID:2020A7PS0986P	Name: Nidhish Parekh
*/
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

//char** reqLexeme;
int currIndex = 0;
int lineIndexS = 0;
int astNodeCount;

ASTNODE createAST(TREENODE parseTree, char** receivedLexeme)
{
    
    reqLexeme = receivedLexeme;
    // for(int i = 0; i<10; i++) printf("%s\n", reqLexeme[i]);
    ASTNODE asTree = doRecursion(parseTree, NULL);
    free(reqLexeme);
    return asTree;
}

void setASTChild(ASTNODE asTree, ASTNODE childNode)
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
    if(parseTree->tnt == 0) 
    {
        token t = parseTree->val.t_val;
        asTree->val.t_val = t;
        if(t == ID || t == NUM || t == RNUM)
        {
            asTree->lexeme = (char *) malloc(MAX_LEXEME);
            strcpy(asTree->lexeme, reqLexeme[currIndex++]);
        }
    }
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
        for (int i = 0; i < level; i++) printf("  ");
        printf("Level %d Node -> ", level);
        if(node->tnt == 0)
        {
            token t = node->val.t_val;
            printT(t);
            if(t == ID || t == NUM || t == RNUM) printf(" %s", node->lexeme);
        }
        else 
        {
            printNT(node->val.nt_val);
        }
        printf("\n");

        if (node->firstChild != NULL)
        {
            for (int i = 0; i < level; i++) printf("  ");
            printf("Children: \n");
            printAST(node->firstChild, level + 1);
        }
        node = node->nextSibling;
    }
}

/* This recurses to create ast node */
ASTNODE doRecursion(TREENODE parseTree, ASTNODE asTree)
{
    int production_rule = parseTree->rule_no;

    ASTNODE node = malloc(sizeof(struct ASTNode));
    node->tnt = parseTree->tnt;
    if(parseTree->tnt == 0)
    {   
        token t = parseTree->val.t_val;
        node->val.t_val = t;
        if(t == ID || t == NUM || t == RNUM)
        {
            node->lexeme = (char *) malloc(MAX_LEXEME);
            strcpy(node->lexeme, reqLexeme[currIndex++]);
        }
        else if(t == START)
        {
            node->line_no = scopeStack[lineIndexS].start;
        }
        else if(t == END)
        {
            node->line_no = scopeStack[lineIndexS++].end;
        }
    } 
    else node->val.nt_val = parseTree->val.nt_val;
    
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
        
        idTypeN->tnt = 1;
        idTypeN->val.nt_val = idType; // ASSIGNED A NEW CONSTRUCT
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
        idTypeN->tnt = 1;
        idTypeN->val.nt_val = idType; // ASSIGNED A NEW CONSTRUCT
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
        idTypeN->tnt = 1;
        idTypeN->val.nt_val = idType; // ASSIGNED A NEW CONSTRUCT
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
        arrayTypeN->tnt = 1; 
        arrayTypeN->val.nt_val = arrType; // ASSIGNED A NEW CONSTRUCT
        
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
        arrayTypeN->tnt = 1;
        arrayTypeN->val.nt_val = arrRangeType; // ASSIGNED A NEW CONSTRUCT

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
        TREENODE startNode = getParseChild(parseTree,0);
        TREENODE endNode = getParseChild(parseTree,2);
        ASTNODE statementsNode = doRecursion(childNode, NULL);
        ASTNODE startANode =  doRecursion(startNode, NULL);
        ASTNODE endANode = doRecursion(endNode, NULL);
        printf("%d %d\n", startANode->line_no, endANode->line_no);
        setASTChild(node,statementsNode);
        setASTChild(node,startANode);
        setASTChild(node,endANode);
        

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
        getValN->val.nt_val = getValue; // ASSIGNED A NEW CONSTRUCT
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
        printValN->val.nt_val = printValue; // ASSIGNED A NEW CONSTRUCT
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
        whichIDN->val.nt_val = arrElement; // ASSIGNED A NEW CONSTRUCT
        setASTChild(whichIDN, asTree); // ID
        setASTChild(whichIDN, inputNode0); // <num_or_id>.addr_syn
        return whichIDN;
    }
    else if( production_rule == 41)
    {
        //<which_ID> → EPSILON
        free(node);
        return asTree; // <which_ID>.addr_syn = <which_ID>.addr_inh
    
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

        free(node);
        TREENODE childNode1 = getParseChild(parseTree, 0);
        ASTNODE idNode = doRecursion(childNode1, NULL);
        TREENODE childNode2 = getParseChild(parseTree, 1);
        ASTNODE idNode2 = doRecursion(childNode2, idNode);
        return idNode2;
    
    }
    else if( production_rule == 45)
    {
        //<whichStmt> → <lvalueIDStmt>

        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return(doRecursion(childNode, asTree));
    
    }
    else if( production_rule == 46)
    {
        //<whichStmt> → <lvalueARRStmt>

        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return(doRecursion(childNode, asTree));
    
    }
    else if( production_rule == 47)
    {
        //<lvalueIDStmt> → ASSIGNOP <expression> SEMICOL

        free(node);
        TREENODE childNode = getParseChild(parseTree, 1);
        ASTNODE inputNode0 = doRecursion(childNode, NULL);
        ASTNODE assignN = (ASTNODE) malloc(sizeof(struct ASTNode));
        assignN->tnt = 1;
        assignN->val.nt_val = assignOp; // ASSIGNED A NEW CONSTRUCT
        setASTChild(assignN, asTree);
        setASTChild(assignN, inputNode0);
        return assignN;
    
    }
    else if( production_rule == 48)
    {
        //<lvalueARRStmt> → SQBO <exprIndex> SQBC ASSIGNOP <expression> SEMICOL

        free(node);
        TREENODE childNode = getParseChild(parseTree, 1);
        ASTNODE inputNode0 = doRecursion(childNode, NULL);
        TREENODE childNode1 = getParseChild(parseTree, 4);
        ASTNODE inputNode1 = doRecursion(childNode1, NULL);
        ASTNODE whichIDN = (ASTNODE) malloc(sizeof(struct ASTNode));
        whichIDN->tnt = 1;
        whichIDN->val.nt_val = arrElement; // ASSIGNED A NEW CONSTRUCT
        setASTChild(whichIDN, asTree);
        setASTChild(whichIDN, inputNode0);
        ASTNODE assignN = (ASTNODE) malloc(sizeof(struct ASTNode));
        assignN->tnt = 1;
        assignN->val.nt_val = assignOp; // ASSIGNED A NEW CONSTRUCT
        setASTChild(assignN, whichIDN);
        setASTChild(assignN, inputNode1);
        return assignN;
    
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
        signedNumN->tnt = 1;
        signedNumN->val.nt_val = signedNum; // ASSIGNED A NEW CONSTRUCT

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

        // <moduleReuseStmt>.addr_syn = createNode(label:USE, <optional>.addr_syn, ID.addr, <actual_para_list>.addr_syn)

        // CHECK

        TREENODE childNode = getParseChild(parseTree, 0);
        TREENODE childNode1 = getParseChild(parseTree, 3);
        TREENODE childNode2 = getParseChild(parseTree, 6);

        ASTNODE funCallN = (ASTNODE) malloc(sizeof(struct ASTNode));
        funCallN->tnt = 1;
        funCallN->val.nt_val = funCallOp;

        ASTNODE inputNode0 = doRecursion(childNode, NULL);
        ASTNODE inputNode1 = doRecursion(childNode1, NULL);
        ASTNODE inputNode2 = doRecursion(childNode2, NULL);

        setASTChild(funCallN, inputNode0);
        setASTChild(funCallN, inputNode1);
        setASTChild(funCallN, inputNode2);

        return funCallN;
    }
    else if( production_rule == 56)
    {
        //<factor> → NUM

        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);    
    
    }
    else if( production_rule == 57)
    {
        //<factor> → RNUM 
        
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    }
    else if( production_rule == 58)
    {
        //<factor> → <boolVal>
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return doRecursion(childNode, NULL);
    }
    else if( production_rule == 59)
    {
        //<factor> → ID <factor2>
    
        free(node);
        TREENODE childNode1 = getParseChild(parseTree, 0);
        ASTNODE idNode = doRecursion(childNode1, NULL);
        TREENODE childNode2 = getParseChild(parseTree, 1);
        ASTNODE idNode2 = doRecursion(childNode2, idNode);
        return idNode2;
    }
    else if( production_rule == 60)
    {
        //<factor2> → SQBO <exprIndex> SQBC

        free(node);
        TREENODE childNode = getParseChild(parseTree, 1);
        ASTNODE inputNode0 = doRecursion(childNode, NULL);
        ASTNODE exprIndexN = (ASTNODE) malloc(sizeof(struct ASTNode));
        exprIndexN->tnt = 1;
        exprIndexN->val.nt_val = arrElement; // ASSIGNED A NEW CONSTRUCT
        setASTChild(exprIndexN, asTree);
        setASTChild(exprIndexN, inputNode0);
        return exprIndexN;
    }
    else if( production_rule == 61)
    {
        //<factor2> → EPSILON
    
        free(node);
        return asTree;
    
    }
    else if( production_rule == 62)
    {
        //<optional> -> SQBO <idList> SQBC ASSIGNOP
    
        TREENODE childNode = getParseChild(parseTree, 1);
        setASTChild(node, doRecursion(childNode, NULL));
    }
    else if( production_rule == 63)
    {
        //<optional> -> EPSILON
        TREENODE childNode = getParseChild(parseTree, 0);
        setASTChild(node, doRecursion(childNode, NULL));
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
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return(doRecursion(childNode, NULL));
    
    }
    else if( production_rule == 68)
    {
        //<expression> → <U>
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return(doRecursion(childNode, NULL));
    }
    else if( production_rule == 69)
    {
        //<U> → <uni_op><new_NT>

        TREENODE childNode1 = getParseChild(parseTree, 0);
        ASTNODE inputNode0 = doRecursion(childNode1, NULL);
        TREENODE childNode2 = getParseChild(parseTree, 1);
        ASTNODE inputNode = doRecursion(childNode2, NULL);

        ASTNODE uniOpN = (ASTNODE) malloc(sizeof(struct ASTNode));
        uniOpN->tnt = 1;
        uniOpN->val.nt_val = uniOp; // ASSIGNED A NEW CONSTRUCT

        setASTChild(uniOpN, inputNode0);
        setASTChild(uniOpN, inputNode);
        return uniOpN;
    
    }
    else if( production_rule == 70)
    {
        //<new_NT> → BO <arithmeticExpr> BC
    
        TREENODE childNode = getParseChild(parseTree, 1);
        free(node);
        return(doRecursion(childNode, NULL));
    }
    else if( production_rule == 71)
    {
        //<new_NT> → <con_var>
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return(doRecursion(childNode, NULL));
    
    }
    else if( production_rule == 72)
    {
        //<uni_op> → PLUS
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    }
    else if( production_rule == 73)
    {
        //<uni_op> → MINUS
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    }
    else if( production_rule == 74)
    {
        //<abExpr> → <AnyTerm><Term7>

        free(node);
        TREENODE childNode1 = getParseChild(parseTree, 0);
        ASTNODE inputNode0 = doRecursion(childNode1, NULL);
        TREENODE childNode2 = getParseChild(parseTree, 1);
        ASTNODE inputNode1 = doRecursion(childNode2, inputNode0);
        return inputNode1;

    }
    else if( production_rule == 75)
    {
        //<Term7> → <logicalOp><AnyTerm><Term7>

        TREENODE childNode1 = getParseChild(parseTree, 0);  
        ASTNODE inputNode1 = doRecursion(childNode1, NULL); // logicalOp.addr_syn

        TREENODE childNode2 = getParseChild(parseTree, 1);
        ASTNODE inputNode2 = doRecursion(childNode2, NULL); // AnyTerm.addr_syn

        ASTNODE logicalN = (ASTNODE) malloc(sizeof(struct ASTNode));
        logicalN->tnt = 1;
        token t = inputNode1->val.t_val;
        if(t == AND) logicalN->val.nt_val = ANDOp;
        else if(t == OR) logicalN->val.nt_val = OROp;

        setASTChild(logicalN, asTree);
        setASTChild(logicalN, inputNode2);

        TREENODE childNode3 = getParseChild(parseTree, 2);
        ASTNODE inputNode3 = doRecursion(childNode3, logicalN); // Term7(2).addr_syn

        return inputNode3;

    }
    else if( production_rule == 76)
    {
        //<Term7> → EPSILON

        free(node);
        return asTree;
    
    }
    else if( production_rule == 77)
    {
        //<AnyTerm> → <arithmeticExpr><Term8>

        // <Term8>.addr_inh = <arithmeticExpr>.addr_syn
        // <AnyTerm>.addr_syn = <Term8>.addr_syn

        TREENODE childNode1 = getParseChild(parseTree, 0);   
        ASTNODE inputNode1 = doRecursion(childNode1, NULL); // <arithmeticExpr>.addr_syn

        TREENODE childNode2 = getParseChild(parseTree, 1);  
        ASTNODE inputNode2 = doRecursion(childNode2, inputNode1); // <Term8>.addr_syn

        return inputNode2;
    
    }
    else if( production_rule == 78)
    {
        //<AnyTerm> → <boolVal>
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return(doRecursion(childNode, NULL));
    
    }
    else if( production_rule == 79)
    {
        //<Term8> → <relationalOp><arithmeticExpr>
    
        //<Term8>.addr_syn = createNode(label: <relationalOp>.addr_syn, <Term8>.addr_inh, <arithmeticExpr>.addr_syn)

        TREENODE childNode1 = getParseChild(parseTree, 0);   
        ASTNODE inputNode1 = doRecursion(childNode1, NULL); // <relationalOp>.addr_syn

        TREENODE childNode2 = getParseChild(parseTree, 1);   
        ASTNODE inputNode2 = doRecursion(childNode2, NULL); // <arithmeticExpr>.addr_syn

        ASTNODE relationalN = (ASTNODE) malloc(sizeof(struct ASTNode));
        relationalN->tnt = 1;
        token t = inputNode1->val.t_val;
        if(t == LT) relationalN->val.nt_val = LTOp;
        else if (t == LE) relationalN->val.nt_val = LEOp;
        else if(t == GT) relationalN->val.nt_val = GTOp;
        else if(t == GE) relationalN->val.nt_val = GEOp;
        else if(t == EQ) relationalN->val.nt_val = EQOp;
        else if(t == NE) relationalN->val.nt_val = NEOp;

        setASTChild(relationalN, asTree);
        setASTChild(relationalN, inputNode2);

        return relationalN;
    }
    else if( production_rule == 80)
    {
        //<Term8> → EPSILON

        free(node);
        return asTree;
    
    }
    else if( production_rule == 81)
    {
        //<con_var> → ID

        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    
    }
    else if( production_rule == 82)
    {
        //<con_var> → NUM

        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    
    }
    else if( production_rule == 83)
    {
        //<con_var> → RNUM

        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    
    }
    else if( production_rule == 84)
    {
        //<factor> → BO <abExpr> BC

        TREENODE childNode = getParseChild(parseTree, 1);
        free(node);
        return doRecursion(childNode, NULL);
    
    }
    else if( production_rule == 85)
    {
        //<arithmeticExpr> → <term><arithmeticExpr2>

        // <arithmeticExpr2>.addr_inh = <term>.addr_syn
        // <arithmeticExpr>.addr_syn = <arithmeticExpr2>.addr_syn


        TREENODE childNode1 = getParseChild(parseTree, 0);   
        ASTNODE inputNode1 = doRecursion(childNode1, NULL); // <term>.addr_syn

        TREENODE childNode2 = getParseChild(parseTree, 1);  
        ASTNODE inputNode2 = doRecursion(childNode2, inputNode1); // <arithmeticExpr2>.addr_syn

        return inputNode2;
    
    }
    else if( production_rule == 86)
    {
        //<arithmeticExpr2> → <low_op><term><arithmeticExpr2>

        TREENODE childNode1 = getParseChild(parseTree, 0);  
        ASTNODE inputNode1 = doRecursion(childNode1, NULL); // low_op.addr_syn

        TREENODE childNode2 = getParseChild(parseTree, 1);
        ASTNODE inputNode2 = doRecursion(childNode2, NULL); // term.addr_syn

        ASTNODE lowOPN = (ASTNODE) malloc(sizeof(struct ASTNode));
        lowOPN->tnt = 1;
        token t = inputNode1->val.t_val;
        if(t == PLUS) lowOPN->val.nt_val = PLUSOp;
        else if(t == MINUS) lowOPN->val.nt_val = MINUSOp;

        setASTChild(lowOPN, asTree);
        setASTChild(lowOPN, inputNode2);

        TREENODE childNode3 = getParseChild(parseTree, 2);
        ASTNODE inputNode3 = doRecursion(childNode3, lowOPN); // arithmeticExpr2(2).addr_syn

        return inputNode3;
    
    }
    else if( production_rule == 87)
    {
        //<arithmeticExpr2> → EPSILON

        free(node);
        return asTree;
    
    }
    else if( production_rule == 88)
    {
        //<term> → <factor><term2>
    
        TREENODE childNode1 = getParseChild(parseTree, 0);   
        ASTNODE inputNode1 = doRecursion(childNode1, NULL); // <factor>.addr_syn

        TREENODE childNode2 = getParseChild(parseTree, 1);  
        ASTNODE inputNode2 = doRecursion(childNode2, inputNode1); // <term2>.addr_syn

        return inputNode2;
    }
    else if( production_rule == 89)
    {
        //<term2> -> <high_op><factor><term2>
    
        TREENODE childNode1 = getParseChild(parseTree, 0);  
        ASTNODE inputNode1 = doRecursion(childNode1, NULL); // high_op.addr_syn

        TREENODE childNode2 = getParseChild(parseTree, 1);
        ASTNODE inputNode2 = doRecursion(childNode2, NULL); // factor.addr_syn

        ASTNODE lowOPN = (ASTNODE) malloc(sizeof(struct ASTNode));
        lowOPN->tnt = 1;
        token t = inputNode1->val.t_val;
        if(t == MUL) lowOPN->val.nt_val = MULOp;
        else if(t == DIV) lowOPN->val.nt_val = DIVOp;

        setASTChild(lowOPN, asTree);
        setASTChild(lowOPN, inputNode2);

        TREENODE childNode3 = getParseChild(parseTree, 2);
        ASTNODE inputNode3 = doRecursion(childNode3, lowOPN); // term2(2).addr_syn

        return inputNode3;
    }
    else if( production_rule == 90)
    {
        //<term2> → EPSILON
        
        free(node);
        return asTree;
    
    }
    else if( production_rule == 91)
    {
        //<exprIndex> → <sign> <exprIndex2>
    
        free(node);
        TREENODE childNode1 = getParseChild(parseTree, 0);
        TREENODE childNode2 = getParseChild(parseTree, 1);
        ASTNODE inputNode0 = doRecursion(childNode1, NULL);
        ASTNODE inputNode = doRecursion(childNode2, NULL);
        ASTNODE signedNumN = (ASTNODE) malloc(sizeof(struct ASTNode));
        signedNumN->tnt = 1;
        signedNumN->val.nt_val = signedNum; // ASSIGNED A NEW CONSTRUCT

        setASTChild(signedNumN, inputNode0);
        setASTChild(signedNumN, inputNode);
        return signedNumN;
    }
    else if( production_rule == 92)
    {
        //<exprIndex> → <arrExpr>

        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return doRecursion(childNode, NULL);
    }
    else if( production_rule == 93)
    {
        //<exprIndex2> → <num_or_id>
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return doRecursion(childNode, NULL);
    }
    else if( production_rule == 94)
    {
        //<exprIndex2> → BO <arrExpr> BC
    
        TREENODE childNode = getParseChild(parseTree, 1);
        free(node);
        return doRecursion(childNode, NULL);
    }
    else if( production_rule == 95)
    {
        //<arrExpr> → <arrTerm> <arrExpr2>
    
        TREENODE childNode1 = getParseChild(parseTree, 0);   
        ASTNODE inputNode1 = doRecursion(childNode1, NULL); // <arrTerm>.addr_syn

        TREENODE childNode2 = getParseChild(parseTree, 1);  
        ASTNODE inputNode2 = doRecursion(childNode2, inputNode1); // <arrExpr2>.addr_syn

        return inputNode2;
    }
    else if( production_rule == 96)
    {
        //<arrExpr2> → <low_op> <arrTerm> <arrExpr2>
    
        TREENODE childNode1 = getParseChild(parseTree, 0);  
        ASTNODE inputNode1 = doRecursion(childNode1, NULL); // low_op.addr_syn

        TREENODE childNode2 = getParseChild(parseTree, 1);
        ASTNODE inputNode2 = doRecursion(childNode2, NULL); // arrTerm.addr_syn

        ASTNODE lowOPN = (ASTNODE) malloc(sizeof(struct ASTNode));
        lowOPN->tnt = 1;
        token t = inputNode1->val.t_val;
        if(t == PLUS) lowOPN->val.nt_val = PLUSOp;
        else if(t == MINUS) lowOPN->val.nt_val = MINUSOp;

        setASTChild(lowOPN, asTree);
        setASTChild(lowOPN, inputNode2);

        TREENODE childNode3 = getParseChild(parseTree, 2);
        ASTNODE inputNode3 = doRecursion(childNode3, lowOPN); // arrExpr2(2).addr_syn

        return inputNode3;
    }
    else if( production_rule == 97)
    {
        //<arrExpr2> → EPSILON

        free(node);
        return asTree;
    }
    else if( production_rule == 98)
    {
        //<arrTerm> → <arrFactor> <arrTerm2>
    
        TREENODE childNode1 = getParseChild(parseTree, 0);   
        ASTNODE inputNode1 = doRecursion(childNode1, NULL); // <arrFactor>.addr_syn

        TREENODE childNode2 = getParseChild(parseTree, 1);  
        ASTNODE inputNode2 = doRecursion(childNode2, inputNode1); // <arrTerm2>.addr_syn

        return inputNode2;
    }
    else if( production_rule == 99)
    {
        //<arrTerm2> → <high_op> <arrFactor> <arrTerm2>

         TREENODE childNode1 = getParseChild(parseTree, 0);  
        ASTNODE inputNode1 = doRecursion(childNode1, NULL); // high_op.addr_syn

        TREENODE childNode2 = getParseChild(parseTree, 1);
        ASTNODE inputNode2 = doRecursion(childNode2, NULL); // arrFactor.addr_syn

        ASTNODE lowOPN = (ASTNODE) malloc(sizeof(struct ASTNode));
        lowOPN->tnt = 1;
        token t = inputNode1->val.t_val;
        if(t == MUL) lowOPN->val.nt_val = MULOp;
        else if(t == DIV) lowOPN->val.nt_val = DIVOp;

        setASTChild(lowOPN, asTree);
        setASTChild(lowOPN, inputNode2);

        TREENODE childNode3 = getParseChild(parseTree, 2);
        ASTNODE inputNode3 = doRecursion(childNode3, lowOPN); // arrTerm2(2).addr_syn

        return inputNode3;
    
    }
    else if( production_rule == 100)
    {
        //<arrTerm2> → EPSILON
    
        free(node);
        return asTree;
    }
    else if( production_rule == 101)
    {
        //<arrFactor> → ID
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    }
    else if( production_rule == 102)
    {
        //<arrFactor> → NUM
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    }
    else if( production_rule == 103)
    {
        //<arrFactor> → <boolVal>
    
        TREENODE childNode = getParseChild(parseTree, 0
        );
        free(node);
        return doRecursion(childNode, NULL);
    }
    else if( production_rule == 104)
    {
        //<arrFactor> → BO <arrExpr> BC
    
        TREENODE childNode = getParseChild(parseTree, 1);
        free(node);
        return doRecursion(childNode, NULL);
    }
    else if( production_rule == 105)
    {
        //<low_op> → PLUS
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    
    }
    else if( production_rule == 106)
    {
        //<low_op> → MINUS
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    
    }
    else if( production_rule == 107)
    {
        //<high_op> → MUL
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    
    }
    else if( production_rule == 108)
    {
        //<high_op> → DIV

        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    
    }
    else if( production_rule == 109)
    {
        //<logicalOp> → AND
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    
    }
    else if( production_rule == 110)
    {
        //<logicalOp> → OR
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    
    }
    else if( production_rule == 111)
    {
        //<relationalOp> →  LT
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    
    }
    else if( production_rule == 112)
    {
        //<relationalOp> → LE
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    
    }
    else if( production_rule == 113)
    {
        //<relationalOp> → GT
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);

    }
    else if( production_rule == 114)
    {
        //<relationalOp> → GE
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);

    }
    else if( production_rule == 115)
    {
        //<relationalOp> → EQ
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    
    }
    else if( production_rule == 116)
    {
        //<relationalOp> → NE
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);

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

        // <caseStmts>.addr_inh = NULL
        // <conditionalStmt>.addr_syn = createNode(label:SWITCH, <caseStmts>.addr_syn, <default_stmt>.addr_syn, ID.addr)
        
        TREENODE childNode1 = getParseChild(parseTree, 2); // ID
        TREENODE childNode2 = getParseChild(parseTree, 5); // <caseStmts>.addr_syn
        TREENODE childNode3 = getParseChild(parseTree, 6); // <default_stmt>.addr_syn
        ASTNODE idN = doRecursion(childNode1, NULL);
        ASTNODE caseN = (ASTNODE) malloc(sizeof(struct ASTNode));
        caseN->tnt = 1;
        caseN->val.nt_val = cases;
        setASTChild(caseN, doRecursion(childNode2, NULL));
        setASTChild(caseN, doRecursion(childNode3, NULL));


        ASTNODE switchN = (ASTNODE) malloc(sizeof(struct ASTNode));
        switchN->tnt = 1;
        switchN->val.nt_val = switchOp;
        setASTChild(switchN, idN);
        setASTChild(switchN, caseN);

        return switchN;
    }
    else if( production_rule == 119)
    {
        //<caseStmts> → CASE <value> COLON <statements> BREAK SEMICOL <caseStmts2>


        // <caseStmts2>.addr_inh = insertAtLast(<caseStmts>.addr_inh, <caseStmts>.addr)
        // <caseStmts>.addr = createNode(label:CASE, <value>.addr_syn, <statements>.addr_syn)
        // <caseStmts>.addr_syn = <caseStmts2>.addr_syn

        TREENODE childNode1 = getParseChild(parseTree, 1); // <value>.addr_syn
        TREENODE childNode2 = getParseChild(parseTree, 3); // <statements>.addr_syn
        TREENODE childNode3 = getParseChild(parseTree, 6); // <caseStmts2>.addr_syn
        
        ASTNODE caseN = (ASTNODE) malloc(sizeof(struct ASTNode));
        caseN->tnt = 1;
        caseN->val.nt_val = case1;
        setASTChild(caseN, doRecursion(childNode1, NULL));
        setASTChild(caseN, doRecursion(childNode2, NULL));


        ASTNODE caseTemp = doRecursion(childNode3, caseN);
        if(caseTemp != NULL) setASTChild(caseN, caseTemp);
        return caseN;

    }
    else if( production_rule == 120)
    {
        //<caseStmts2> → CASE <value> COLON <statements> BREAK SEMICOL <caseStmts2>
    
        TREENODE childNode1 = getParseChild(parseTree, 1); // <value>.addr_syn
        TREENODE childNode2 = getParseChild(parseTree, 3); // <statements>.addr_syn
        TREENODE childNode3 = getParseChild(parseTree, 6); // <caseStmts2>.addr_syn
        
        ASTNODE caseN = (ASTNODE) malloc(sizeof(struct ASTNode));
        caseN->tnt = 1;
        caseN->val.nt_val = case1;
        setASTChild(caseN, doRecursion(childNode1, NULL));
        setASTChild(caseN, doRecursion(childNode2, NULL));


        ASTNODE caseTemp = doRecursion(childNode3, caseN);
        if(caseTemp != NULL) setASTChild(caseN, caseTemp);
        return caseN;
    }
    else if( production_rule == 121)
    {
        //<caseStmts2> → EPSILON
        free(node);
        return NULL;
    
    }
    else if( production_rule == 122)
    {
        //<value> → NUM
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    }
    else if( production_rule == 123)
    {
        //<value> → TRUE
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    }
    else if( production_rule == 124)
    {
        //<value> → FALSE

        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    }
    else if( production_rule == 125)
    {
        //<default_stmt> → DEFAULT COLON <statements> BREAK SEMICOL

        TREENODE childNode = getParseChild(parseTree, 2);
        free(node);
        return doRecursion(childNode, NULL);

    }
    else if( production_rule == 126)
    {
        //<default_stmt> → EPSILON 
        TREENODE childNode = getParseChild(parseTree, 0);
        setASTChild(node, convertToAST(childNode));
    
    }
    else if( production_rule == 127)
    {
        //<iterativeStmt> → FOR BO ID IN <for_range> BC START <statements> END
        
        free(node);
        TREENODE childNode1 = getParseChild(parseTree, 2);
        TREENODE childNode2 = getParseChild(parseTree, 4);
        TREENODE childNode3 = getParseChild(parseTree, 7);
        TREENODE childNode4 = getParseChild(parseTree, 6);
        TREENODE childNode5 = getParseChild(parseTree, 8);

        ASTNODE inputNode0 =  doRecursion(childNode1, NULL); // ID
        ASTNODE inputNode1 =  doRecursion(childNode2, NULL); // <for_range>.addr_syn
        ASTNODE inputNode2 =  doRecursion(childNode3, NULL); // <statements>.addr_syn
        ASTNODE inputNode3 =  doRecursion(childNode4, NULL); // START
        ASTNODE inputNode4 =  doRecursion(childNode5, NULL); // END
        printf("%d %d\n", inputNode3->line_no, inputNode4->line_no);
        ASTNODE forIDnRangeN = (ASTNODE) malloc(sizeof(struct ASTNode));
        forIDnRangeN->tnt = 1;
        forIDnRangeN->val.nt_val = forIDnRangeOp;

        setASTChild(forIDnRangeN, inputNode0);
        setASTChild(forIDnRangeN, inputNode1);

        ASTNODE forN = (ASTNODE) malloc(sizeof(struct ASTNode));
        forN->tnt = 1;
        forN->val.nt_val = forOp;

        setASTChild(forN, forIDnRangeN);
        setASTChild(forN, inputNode2);

        setASTChild(forN, inputNode3);
        setASTChild(forN, inputNode4);
        return forN;
    }
    else if( production_rule == 128)
    {
        //<iterativeStmt> → WHILE BO <abExpr> BC START <statements> END
    
        free(node);
        TREENODE childNode1 = getParseChild(parseTree, 2);
        TREENODE childNode2 = getParseChild(parseTree, 5);
        TREENODE childNode3 = getParseChild(parseTree, 4);
        TREENODE childNode4 = getParseChild(parseTree, 6);

        ASTNODE inputNode0 =  doRecursion(childNode1, NULL); // <abExpr>.addr_syn
        ASTNODE inputNode1 =  doRecursion(childNode2, NULL); // <statements>.addr_syn
        ASTNODE inputNode2 =  doRecursion(childNode3, NULL); // START
        ASTNODE inputNode3 =  doRecursion(childNode4, NULL); // END

        ASTNODE whileN = (ASTNODE) malloc(sizeof(struct ASTNode));
        whileN->tnt = 1;
        whileN->val.nt_val = whileOp;

        setASTChild(whileN, inputNode0);
        setASTChild(whileN, inputNode1);
        setASTChild(whileN, inputNode2);
        setASTChild(whileN, inputNode3);

        return whileN;
    }
    else if( production_rule == 129)
    {
        //<for_range> → <for_index> RANGEOP <for_index>
    
        free(node);
        TREENODE childNode1 = getParseChild(parseTree, 0);
        TREENODE childNode2 = getParseChild(parseTree, 2);
        ASTNODE inputNode0 = doRecursion(childNode1, NULL);
        ASTNODE inputNode = doRecursion(childNode2, NULL);
        ASTNODE arrayTypeN = (ASTNODE) malloc(sizeof(struct ASTNode));
        arrayTypeN->tnt = 1;
        arrayTypeN->val.nt_val = arrRangeType; // ASSIGNED A NEW CONSTRUCT

        setASTChild(arrayTypeN, inputNode0);
        setASTChild(arrayTypeN, inputNode);
        return arrayTypeN;
    }
    else if( production_rule == 130)
    {
        //<for_index> → <for_sign> <for_index2>
    
        free(node);
        TREENODE childNode1 = getParseChild(parseTree, 0);
        TREENODE childNode2 = getParseChild(parseTree, 1);
        ASTNODE inputNode0 = doRecursion(childNode1, NULL);
        ASTNODE inputNode = doRecursion(childNode2, NULL);
        ASTNODE signedNumN = (ASTNODE) malloc(sizeof(struct ASTNode));
        signedNumN->tnt = 1;
        signedNumN->val.nt_val = signedNum; // ASSIGNED A NEW CONSTRUCT

        setASTChild(signedNumN, inputNode0);
        setASTChild(signedNumN, inputNode);
        return signedNumN;
    }
    else if( production_rule == 131)
    {
        //<for_index2> → NUM
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    }
    else if( production_rule == 132)
    {
        //<for_sign> → PLUS
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    }
    else if( production_rule == 133)
    {
        //<for_sign> → MINUS
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    }
    else if( production_rule == 134)
    {
        //<for_sign> → EPSILON

        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    }
    else if( production_rule == 135)
    {
        //<actual_para_list> → <sign> <K> <Term9>

        // <K>.addr_inh = <sign>.addr_syn
        // <Term9>.addr_inh = <K>.addr_syn
        // <actual_para_list>.addr_syn = insertAtLast(<Term9>.addr_syn, createNode(label:signed_num, <sign>.addr_syn, <K>.addr_syn))
        
        free(node);
        TREENODE childNode = getParseChild(parseTree, 0); // <sign>.addr_syn
        TREENODE childNode1 = getParseChild(parseTree, 1); // <K>.addr_syn
        TREENODE childNode2 = getParseChild(parseTree, 2); // <Term9>.addr_syn

        ASTNODE inputNode0 = doRecursion(childNode, NULL);
        ASTNODE inputNode1 = doRecursion(childNode1, NULL);

        ASTNODE signedNumN = (ASTNODE) malloc(sizeof(struct ASTNode));
        signedNumN->tnt = 1;
        signedNumN->val.nt_val = signedNum; // ASSIGNED A NEW CONSTRUCT

        setASTChild(signedNumN, inputNode0);
        setASTChild(signedNumN, inputNode1);

        ASTNODE inputNode2 = doRecursion(childNode2, NULL);
        if(inputNode2 != NULL) setASTChild(signedNumN, inputNode2);

        return signedNumN;

    }
    else if( production_rule == 136)
    {
        //<Term9> → COMMA <sign> <K> <Term9>
    
        // <K>.addr_inh = <sign>.addr_syn
        // <Term92>.addr_inh = <Term91>.addr_inh

        // Bottom Up:
        // <Term91>.addr_syn = insertAtLast(<Term92>.addr_syn, createNode(label:signed_num, <K>.addr_syn, <sign>.addr_syn))

        free(node);
        TREENODE childNode = getParseChild(parseTree, 1); // <sign>.addr_syn
        TREENODE childNode1 = getParseChild(parseTree, 2); // <K>.addr_syn
        TREENODE childNode2 = getParseChild(parseTree, 3); // <Term9>(2).addr_syn

        ASTNODE inputNode0 = doRecursion(childNode, NULL);
        ASTNODE inputNode1 = doRecursion(childNode1, NULL);

        ASTNODE signedNumN = (ASTNODE) malloc(sizeof(struct ASTNode));
        signedNumN->tnt = 1;
        signedNumN->val.nt_val = signedNum; // ASSIGNED A NEW CONSTRUCT

        setASTChild(signedNumN, inputNode0);
        setASTChild(signedNumN, inputNode1);

        ASTNODE inputNode2 = doRecursion(childNode2, NULL);
        if(inputNode2 != NULL) setASTChild(signedNumN, inputNode2);

        return signedNumN;
    }
    else if( production_rule == 137)
    {
        //<Term9> → EPSILON
        free(node);
        return asTree;
    }
    else if( production_rule == 138)
    {
        //<K> → NUM
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    }
    else if( production_rule == 139)
    {
        //<K> → RNUM
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    }
    else if( production_rule == 140)
    {
        //<K> → <boolVal>
    
        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return doRecursion(childNode, NULL);
    }
    else if( production_rule == 141)
    {
        //<K> → ID <actual_para_list2>

        free(node);
        TREENODE childNode1 = getParseChild(parseTree, 0);
        ASTNODE idNode = doRecursion(childNode1, NULL);
        TREENODE childNode2 = getParseChild(parseTree, 1);
        ASTNODE idNode2 = doRecursion(childNode2, idNode);
        return idNode2;
    }
    else if( production_rule == 142)
    {
        //<actual_para_list2> → SQBO <exprIndex> SQBC

        free(node);
        TREENODE childNode = getParseChild(parseTree, 1);
        ASTNODE inputNode0 = doRecursion(childNode, NULL);
        ASTNODE whichIDN = (ASTNODE) malloc(sizeof(struct ASTNode));
        whichIDN->tnt = 1;
        whichIDN->val.nt_val = arrElement; // ASSIGNED A NEW CONSTRUCT
        setASTChild(whichIDN, asTree);
        setASTChild(whichIDN, inputNode0);
        return whichIDN;
    
    }
    else if( production_rule == 143)
    {
        //<actual_para_list2> → EPSILON

        free(node);
        return asTree;
    }
    return node;
}

int countNodes(ASTNODE node){
    while (node != NULL)
    {
        astNodeCount ++;
        if (node->firstChild != NULL)
        {
            
            countNodes(node->firstChild);
        }
        node = node->nextSibling;
    }

    return (int) astNodeCount*0.8;
}
