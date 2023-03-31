// #include "stack_ll.h"
#include "stackDef.h"
#include "Nlinkedlist.h"
#include "grammar.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

Stack* newStack()
{
    Stack* mainStack = (Stack*) malloc(sizeof(Stack));
    mainStack->list1 = createNewList();
    return mainStack;
}

void push(Stack* mainStack, NODE element)
{
    // printNT(element->val.nt_val);
    LIST temp = mainStack->list1;
    insertNodeIntoList(element, temp);
}

NODE top(Stack* mainStack) 
{
    LIST ll = mainStack->list1;
    NODE temp = ll->head;
    return temp;
}

void pop(Stack* mainStack)
{
    LIST ll = mainStack->list1;
    removeFirst(ll);
}

int reachEnd(Stack* mainStack)
{
    if(mainStack->list1->count == 0) return 0;
    else return 1;
}

void printStack(Stack* mainStack)
{
    LIST stack = mainStack->list1;
    printList(stack);
}

void pushDerivation(Stack* mainStack, RULE rhs)
{
    // printRule(rhs);
    // LIST list = duplicateRuleIntoList(rhs);
    fflush(stdout);
    pop(mainStack);
    NODE rhsptr = rhs->head->next; 
    NODE dup;
    // printList(list);
    Stack* temp = newStack();
    while(rhsptr != NULL)
    {   
        dup = duplicateNode(rhsptr);
        push(temp, dup);
        // printStack(temp);
        rhsptr = rhsptr->next;
    }
    // temp->list1->head = rhsptr;
    // printStack(temp);
    while(reachEnd(temp))
    {
        NODE ins = top(temp);
        pop(temp);
        push(mainStack, ins);
    }
}
