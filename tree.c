/*
ID: 2020A7PS1214P	Name: Darshan Abhaykumar
ID: 2020A7PS0970P	Name: Debjit Kar
ID:2020A7PS0986P	Name: Nidhish Parekh
*/
#include "treeDef.h"
#include "lexer.h"
#include "grammar.h"
#include <stdlib.h>


TREENODE initTree()
{
    TREENODE temp = (TREENODE) malloc(sizeof(struct TreeNode));
    temp->firstChild = NULL;
    temp->nextSibling = NULL;   
    temp->parent = NULL;
    temp->tnt = 1; 
    temp->line_no = -1;
    temp->val.nt_val = program;
    temp->rule_no = 0;
    return temp;    
}

TREENODE createEmptyNode()
{
    TREENODE temp = (TREENODE) malloc(sizeof(struct TreeNode));
    temp->firstChild = NULL;
    temp->nextSibling = NULL;   
    temp->parent = NULL;
    temp->tnt = -1; // CHECK
    temp->rule_no = -1;
    temp->line_no = -1;
    return temp; 
}


void leftmostDerive(NODE deriv, TREENODE t1, int line_no, int rule_no)
{
    TREENODE curr = t1;
    TREENODE parent = curr->parent;
    TREENODE parents;
    if(deriv != NULL)
    {
        while(curr->firstChild != NULL) // LOOP WHILE CHILD EXISTS
        {   
            parent = curr;
            curr = curr->firstChild;
        } 
        
        if(curr->tnt == 0) // LAST CHILD IS TERMINAL
        {
            
            while(curr->nextSibling != NULL && curr->tnt == 0) // LOOP WHILE SIBLING IS TERMINAL AND NOT THE END
            {
                curr = curr->nextSibling;
            }  
            if(curr->tnt == 0)
            {
                while(parent->nextSibling == NULL) parent = parent->parent;
                leftmostDerive(deriv, parent->nextSibling, line_no, rule_no); // LAST CHILD IS TERMINAL THEN LOOK AT UNCLE
                return;
            } 
            else 
            {
                leftmostDerive(deriv, curr, line_no, rule_no); // RECURSIVELY ADD DERIVATION TO CURR
                return;
            }
        }
        else
        {
            curr->rule_no = rule_no;
            parents = curr;
            TREENODE temp = createEmptyNode();
            temp->parent = curr;
            temp->line_no = line_no;
            temp->tnt = deriv->tnt;
            if(temp->tnt == 0) temp->val.t_val = deriv->val.t_val;
            else temp->val.nt_val = deriv->val.nt_val;
            
            curr->firstChild = temp;
            curr = curr->firstChild;
            deriv = deriv->next;
        }
        
    }
    while(deriv!=NULL)
    {
        TREENODE temp = createEmptyNode();
        temp->parent = parents;
        temp->tnt = deriv->tnt;
        temp->line_no = line_no;
        if(temp->tnt == 0) temp->val.t_val = deriv->val.t_val;
        else temp->val.nt_val = deriv->val.nt_val;
        curr->nextSibling = temp;
        curr = curr->nextSibling;
        deriv = deriv->next;
    }
}

void printTree(TREENODE node, int level)
{
    while (node != NULL)
    {
        for (int i = 0; i < level; i++) printf("  ");
        printf("Level %d Node -> ", level);
        if(node->tnt == 0) printT(node->val.t_val);
        else 
        {
            printNT(node->val.nt_val);
            printf(" Rule: %d", node->rule_no);
        }
        printf("\n");

        if (node->firstChild != NULL)
        {
            for (int i = 0; i < level; i++) printf("  ");
            printf("Children: \n");
            printTree(node->firstChild, level + 1);
        }
        node = node->nextSibling;
    }
}

void runTree(void)
{
    // TREENODE t1 = initTree();
    // RULE temp = createNewRule(program, 0);
    // addTermToRule(temp, 0, 0, 1);
    // addTermToRule(temp, 0, 0, 0);
    // addTermToRule(temp, 0, 1, 1);
    // RULE temp2 = createNewRule(moduleDeclarations, 1);
    // addTermToRule(temp2, 0, 0, 1);
    // addTermToRule(temp2, 0, 0, 0);
    // addTermToRule(temp2, 0, 0, 0);
    // RULE temp3 = createNewRule(moduleDeclaration, 2);
    // addTermToRule(temp3, 0, 0, 0);
    // addTermToRule(temp3, 0, 0, 0);
    // addTermToRule(temp3, 0, 0, 0);
    // leftmostDerive(temp->head->next, t1);
    // leftmostDerive(temp2->head->next, t1);
    // leftmostDerive(temp3->head->next, t1);
    // printTree(t1, 0);
}

void freeTree(TREENODE node)
{
    if(node->tnt == 0) free(node);
    else {
        if (node->firstChild != NULL){
            freeTree(node->firstChild);
        }
        if(node->nextSibling != NULL){
            freeTree(node->nextSibling);
        }
        free(node);
    } 
    
}
