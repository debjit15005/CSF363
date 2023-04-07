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

void setASTChild(ASTNODE asTree, int child_count, ASTNODE childNode) // child_count is 0-indexed
{
    childNode->parent = asTree;
    if(child_count == 0)
    {
        asTree->firstChild = childNode;
    }
    else
    {
        asTree = asTree->firstChild;
        while(child_count-->1)
        {
            asTree = asTree->nextSibling;
        }
        asTree->nextSibling = childNode;
    }
}

TREENODE getASTChild(ASTNODE asTree, int child_count) // child_count is 0-indexed
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
    asTree->parent = NULL; // NEED TO FILL THIS
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
    if(parseTree->tnt == 0) node->val.t_val = parseTree->val.t_val;
    else node->val.nt_val = parseTree->val.nt_val;
    node->line_no = -1;
    node->firstChild = NULL;
    node->nextSibling = NULL;    
    node->parent = NULL; 


    if(production_rule == 0)    
    {
        //<program> → <moduleDeclarations><otherModules><driverModule><otherModules>

        for(int i = 0; i<4; i++)
        {
            printf("%d\n", i);
            fflush(stdout);
            TREENODE childNode = getParseChild(parseTree, i);
            setASTChild(node, i, doRecursion(childNode, NULL)); // CHECK IF THIS ASSIGNMENT WILL WORK
            // free(childNode); // CHECK IF THIS REMOVES THE INTERNAL NODE OR 'childNode'
        }
    }
    else if(production_rule == 1)
    {
        //<moduleDeclarations> → <moduleDeclaration><moduleDeclarations>


    }
    else if( production_rule == 2)
    {
        //<moduleDeclarations> → EPSILON

        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    }
    else if( production_rule == 3)
    {
        //<moduleDeclaration> →  DECLARE MODULE ID SEMICOL 
    
    
    }
    else if( production_rule == 4)
    {
        //<otherModules> →  <module><otherModules>
    
    
    }
    else if( production_rule == 5)
    {
        //<otherModules> → EPSILON

        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
    }
    else if( production_rule == 6)
    {
        //<driverModule> → DRIVERDEF DRIVER PROGRAM DRIVERENDDEF <moduleDef>

        for(int i = 0; i<4; i++) free(getParseChild(parseTree, i));
        TREENODE childNode = getParseChild(parseTree, 4);
        setASTChild(node, 0, doRecursion(childNode, NULL));
    
    }
    else if( production_rule == 7)
    {
        //<module> → DEF MODULE ID ENDDEF TAKES INPUT SQBO <input_plist> SQBC SEMICOL <ret><moduleDef>
    
    
    }
    else if( production_rule == 8)
    {
        //<ret> → RETURNS SQBO <output_plist> SQBC SEMICOL
    
    
    }
    else if( production_rule == 9)
    {
        //<ret> → EPSILON
    
    
    }
    else if( production_rule == 10)
    {
        //<input_plist> → ID COLON <dataType> <input_plist2>
    
    
    }
    else if( production_rule == 11)
    {
        //<input_plist2> → COMMA ID COLON <dataType> <input_plist2>
    
    
    }
    else if( production_rule == 12)
    {
        //<input_plist2> → EPSILON
    
    
    }
    else if( production_rule == 13)
    {
        //<output_plist> → ID COLON <type> <output_plist2>
    
    
    }
    else if( production_rule == 14)
    {
        //<output_plist2> → COMMA ID COLON <type> <output_plist2>
    
    
    }
    else if( production_rule == 15)
    {
        //<output_plist2> → EPSILON
    
    
    }
    else if( production_rule == 16)
    {
        //<dataType> → INTEGER
    
    
    }
    else if( production_rule == 17)
    {
        //<dataType> → REAL
    
    
    }
    else if( production_rule == 18)
    {
        //<dataType> → BOOLEAN
    
    
    }
    else if( production_rule == 19)
    {
        //<dataType> → ARRAY SQBO <arr_range> SQBC OF <type>
    
    
    }
    else if( production_rule == 20)
    {
        //<arr_range>→ <arr_index> RANGEOP <arr_index>
    
    
    }
    else if( production_rule == 21)
    {
        //<type> → INTEGER
    
    
    }
    else if( production_rule == 22)
    {
        //<type> → REAL
    
    
    }
    else if( production_rule == 23)
    {
        //<type> → BOOLEAN
    
    
    }
    else if( production_rule == 24)
    {
        //<moduleDef> → START <statements> END

        free(getParseChild(parseTree, 0));
        free(getParseChild(parseTree, 2));
        TREENODE childNode = getParseChild(parseTree, 1);
        free(node);
        return(doRecursion(childNode, NULL));
        // printf("here\n");
        // fflush(stdout);

        // getASTChild(asTree, 0) = doRecursion(childNode, NULL);
        // free(childNode);
    }
    else if( production_rule == 25)
    {
        //<statements> → <statement> <statements>
    
    
    }
    else if( production_rule == 26)
    {
        //<statements> → EPSILON

        TREENODE childNode = getParseChild(parseTree, 0);
        free(node);
        return convertToAST(childNode);
        // free(childNode);
    
    }
    else if( production_rule == 27)
    {
        //<statement> → <ioStmt>
    
    
    }
    else if( production_rule == 28)
    {
        //<statement> → <simpleStmt>
    
    
    }
    else if( production_rule == 29)
    {
        //<statement> → <declareStmt>
    
    
    }
    else if( production_rule == 30)
    {
        //<statement> → <conditionalStmt>
    
    
    }
    else if( production_rule == 31)
    {
        //<statement> → <iterativeStmt>
    
    
    }
    else if( production_rule == 32)
    {
        //<ioStmt> → GET_VALUE BO ID BC SEMICOL
    
    
    }
    else if( production_rule == 33)
    {
        //<ioStmt> → PRINT BO <print_var> BC SEMICOL
    
    
    }
    else if( production_rule == 34)
    {
        //<boolVal> → TRUE 
    
    
    }
    else if( production_rule == 35)
    {
        //<boolVal> → FALSE
    
    
    }
    else if( production_rule == 36)
    {
        //<print_var> → ID <which_ID>
    
    
    }
    else if( production_rule == 37)
    {
        //<print_var> → NUM
    
    
    }
    else if( production_rule == 38)
    {
        //<print_var> → RNUM
    
    
    }
    else if( production_rule == 39)
    {
        //<print_var> → <boolVal>
    
    
    }
    else if( production_rule == 40)
    {
        //<which_ID> → SQBO <num_or_id> SQBC
    
    
    }
    else if( production_rule == 41)
    {
        //<which_ID> → EPSILON
    
    
    }
    else if( production_rule == 42)
    {
        //<simpleStmt> → <assignmentStmt>
    
    
    }
    else if( production_rule == 43)
    {
        //<simpleStmt> → <moduleReuseStmt>
    
    
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
    
    
    }
    else if( production_rule == 50)
    {
        //<num_or_id> → NUM
    
    
    }
    else if( production_rule == 51)
    {
        //<num_or_id> → ID
    
    
    }
    else if( production_rule == 52)
    {
        //<sign> → PLUS
    
    
    }
    else if( production_rule == 53)
    {
        //<sign> → MINUS
    
    
    }
    else if( production_rule == 54)
    {
        //<sign> → ε
    
    
    }
    else if( production_rule == 55)
    {
        //<moduleReuseStmt> → <optional> USE MODULE ID WITH PARAMETERS <actual_para_list> SEMICOL
    
    
    }
    else if( production_rule == 56)
    {
        //<actual_para_list> → <sign> <K> <Term9>
    
    
    }
    else if( production_rule == 57)
    {
        //<Term9> → COMMA <sign> <K> <Term9>
    
    
    }
    else if( production_rule == 58)
    {
        //<Term9> → EPSILON
    
    
    }
    else if( production_rule == 59)
    {
        //<K> → NUM
    
    
    }
    else if( production_rule == 60)
    {
        //<K> → RNUM
    
    
    }
    else if( production_rule == 61)
    {
        //<K> → <boolVal>
    
    
    }
    else if( production_rule == 62)
    {
        //<K> → ID <actual_para_list2>
    
    
    }
    else if( production_rule == 63)
    {
        //<actual_para_list2> → SQBO <exprIndex> SQBC 
    
    
    }
    else if( production_rule == 64)
    {
        //<actual_para_list2> → EPSILON
    
    
    }
    else if( production_rule == 65)
    {
        //<optional> → SQBO <idList> SQBC ASSIGNOP
    
    
    }
    else if( production_rule == 66)
    {
        //<optional> → EPSILON 
    
    
    }
    else if( production_rule == 67)
    {
        //<idList> -> ID <idList2>
    
    
    }
    else if( production_rule == 68)
    {
        //<idList2> -> COMMA ID <idList2>
    
    
    }
    else if( production_rule == 69)
    {
        //<idList2> -> EPSILON
    
    
    }
    else if( production_rule == 70)
    {
        //<expression> -> <abExpr>
    
    
    }
    else if( production_rule == 71)
    {
        //<expression> -> <U>
    
    
    }
    else if( production_rule == 72)
    {
        //<U> -> <uni_op> <new_NT>
    
    
    }
    else if( production_rule == 73)
    {
        //<new_NT> -> BO <arithmeticExpr> BC
    
    
    }
    else if( production_rule == 74)
    {
        //<new_NT> -> <con_var>
    
    
    }
    else if( production_rule == 75)
    {
        //<uni_op> → PLUS
    
    
    }
    else if( production_rule == 76)
    {
        //<uni_op> → MINUS 
    
    
    }
    else if( production_rule == 77)
    {
        //<abExpr> → <AnyTerm> <Term7>
    
    
    }
    else if( production_rule == 78)
    {
        //<Term7> → <logicalOp><AnyTerm><Term7>
    
    
    }
    else if( production_rule == 79)
    {
        //<Term7> → EPSILON
    
    
    }
    else if( production_rule == 80)
    {
        //<AnyTerm> → <arithmeticExpr> <Term8>
    
    
    }
    else if( production_rule == 81)
    {
        //<AnyTerm> → <boolVal>
    
    
    }
    else if( production_rule == 82)
    {
        //<Term8> → <relationalOp> <arithmeticExpr>
    
    
    }
    else if( production_rule == 83)
    {
        //<Term8> → EPSILON
    
    
    }
    else if( production_rule == 84)
    {
        //<con_var> → ID
    
    
    }
    else if( production_rule == 85)
    {
        //<con_var> → NUM
    
    
    }
    else if( production_rule == 86)
    {
        //<con_var> → RNUM
    
    
    }
    else if( production_rule == 87)
    {
        //<arithmeticExpr> → <term> <arithmeticExpr2>
    
    
    }
    else if( production_rule == 88)
    {
        //<arithmeticExpr2> → <low_op> <term>  <arithmeticExpr2>
    
    
    }
    else if( production_rule == 89)
    {
        //<arithmeticExpr2> → EPSILON
    
    
    }
    else if( production_rule == 90)
    {
        //<term> → <factor> <term2>
    
    
    }
    else if( production_rule == 91)
    {
        //<term2> → <high_op> <factor> <term2>
    
    
    }
    else if( production_rule == 92)
    {
        //<term2> → EPSILON
    
    
    }
    else if( production_rule == 93)
    {
        //<factor> → BO <abExpr> BC
    
    
    }
    else if( production_rule == 94)
    {
        //<factor> → NUM
    
    
    }
    else if( production_rule == 95)
    {
        //<factor> → RNUM
    
    
    }
    else if( production_rule == 96)
    {
        //<factor> → <boolVal>
    
    
    }
    else if( production_rule == 97)
    {
        //<factor> → ID <factor2>
    
    
    }
    else if( production_rule == 98)
    {
        //<factor2> → SQBO <exprIndex> SQBC
    
    
    }
    else if( production_rule == 99)
    {
        //<factor2> → EPSILON
    
    
    }
    else if( production_rule == 100)
    {
        //<exprIndex> → <sign> <exprIndex2>
    
    
    }
    else if( production_rule == 101)
    {
        //<exprIndex> → <arrExpr>
    
    
    }
    else if( production_rule == 102)
    {
        //<exprIndex2> → <num_or_id>
    
    
    }
    else if( production_rule == 103)
    {
        //<exprIndex2> → BO <arrExpr> BC
    
    
    }
    else if( production_rule == 104)
    {
        //<arrExpr> → <arrTerm> <arrExpr2>
    
    
    }
    else if( production_rule == 105)
    {
        //<arrExpr2> → <low_op> <arrTerm> <arrExpr2>
    
    
    }
    else if( production_rule == 106)
    {
        //<arrExpr2> → EPSILON
    
    
    }
    else if( production_rule == 107)
    {
        //<arrTerm> → <arrFactor> <arrTerm2>
    
    
    }
    else if( production_rule == 108)
    {
        //<arrTerm2> → <high_op> <arrFactor> <arrTerm2>
    
    
    }
    else if( production_rule == 109)
    {
        //<arrTerm2> → EPSILON
    
    
    }
    else if( production_rule == 110)
    {
        //<arrFactor> → ID
    
    
    }
    else if( production_rule == 111)
    {
        //<arrFactor> → NUM
    
    
    }
    else if( production_rule == 112)
    {
        //<arrFactor> → <boolVal>
    
    
    }
    else if( production_rule == 113)
    {
        //<arrFactor> → BO <arrExpr> BC
    
    
    }
    else if( production_rule == 114)
    {
        //<low_op> → PLUS
    
    
    }
    else if( production_rule == 115)
    {
        //<low_op> → MINUS
    
    
    }
    else if( production_rule == 116)
    {
        //<high_op> → MUL
    
    
    }
    else if( production_rule == 117)
    {
        //<high_op> → DIV
    
    
    }
    else if( production_rule == 118)
    {
        //<logicalOp> → AND
    
    
    }
    else if( production_rule == 119)
    {
        //<logicalOp> → OR
    
    
    }
    else if( production_rule == 120)
    {
        //<relationalOp> →  LT
    
    
    }
    else if( production_rule == 121)
    {
        //<relationalOp> → LE
    
    
    }
    else if( production_rule == 122)
    {
        //<relationalOp> → GT
    
    
    }
    else if( production_rule == 123)
    {
        //<relationalOp> → GE
    
    
    }
    else if( production_rule == 124)
    {
        //<relationalOp> → EQ
    
    
    }
    else if( production_rule == 125)
    {
        //<relationalOp> → NE
    
    
    }
    else if( production_rule == 126)
    {
        //<declareStmt> →  DECLARE <idList> COLON <dataType> SEMICOL
    
    
    }
    else if( production_rule == 127)
    {
        //<conditionalStmt> → SWITCH BO ID BC START <caseStmts><default_stmt> END
    
    
    }
    else if( production_rule == 128)
    {
        //<caseStmts> → CASE <value> COLON <statements> BREAK SEMICOL <caseStmts2>
    
    
    }
    else if( production_rule == 129)
    {
        //<caseStmts2> → CASE <value> COLON <statements> BREAK SEMICOL <caseStmts2>
    
    
    }
    else if( production_rule == 130)
    {
        //<caseStmts2> → EPSILON
    
    
    }
    else if( production_rule == 131)
    {
        //<value> → NUM
    
    
    }
    else if( production_rule == 132)
    {
        //<value> → TRUE
    
    
    }
    else if( production_rule == 133)
    {
        //<value> → FALSE
    
    
    }
    else if( production_rule == 134)
    {
        //<default_stmt> → DEFAULT COLON <statements> BREAK SEMICOL
    
    
    }
    else if( production_rule == 135)
    {
        //<default_stmt> → EPSILON 
    
    
    }
    else if( production_rule == 136)
    {
        //<iterativeStmt> → FOR BO ID IN <for_range> BC START <statements> END
    
    
    }
    else if( production_rule == 137)
    {
        //<iterativeStmt> → WHILE BO <abExpr> BC START <statements> END
    
    
    }
    else if( production_rule == 138)
    {
        //<for_range> → <for_index> RANGEOP <for_index>
    
    
    }
    else if( production_rule == 139)
    {
        //<for_index> → <for_sign> <for_index2>
    
    
    }
    else if( production_rule == 140)
    {
        //<for_index2> → NUM
    
    
    }
    else if( production_rule == 141)
    {
        //<for_sign> → PLUS
    
    
    }
    else if( production_rule == 142)
    {
        //<for_sign> → MINUS
    
    
    }
    else if( production_rule == 143)
    {
        //<for_sign> → EPSILON


    }
    return node;
}


