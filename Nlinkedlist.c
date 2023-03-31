/*
Group No. : 42
ID: 2020A7PS1214P	Name: Darshan Abhaykumar
ID: 2020A7PS0970P	Name: Debjit Kar
ID:2020A7PS0986P	Name: Nidhish Parekh
*/

#include "stack_ll.h"
// #include "linkedlist.h" 
#include "NlinkedlistDef.h"
#include "grammar.h"
#include "grammarDef.h"
#include "lexerDef.h"
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>

LIST createNewList()
{
    LIST ll = (LIST) malloc(sizeof(LL));
    ll->count = 0;
    ll->head = NULL;
    return ll;
}

NODE createNewNode(token t, NT nt, int tnt)
{
    NODE temp = (NODE) malloc(sizeof(node));
    if(tnt == 1)
    {
        temp->val.nt_val = nt;
    }
    else temp->val.t_val = t;
    temp->tnt = tnt;
    temp->next = NULL;
    return temp;
}

NODE duplicateNode(NODE node){
    NODE temp = (NODE) malloc(sizeof(node));
    if(node->tnt == 1)
    {
        temp->val.nt_val = node->val.nt_val;
    }
    else temp->val.t_val = node->val.t_val;
    temp->tnt = node->tnt;
    temp->next = NULL;
    return temp; 
}

void insertNodeIntoList(NODE node, LIST list)
{
    list->count++;
    if(list->head == NULL)
    {
        list->head = node;
        return;
    }
    else
    {
        NODE temp = list->head;
        list->head = node;
        node->next = temp;
    }
}

void removeFirst(LIST list)
{
    if(list->count == 1)
    {
        NODE temp = list->head;
        //free(temp);
        list->head = NULL;
    }
    else
    {
        NODE temp = list->head->next;
        NODE remove = list->head;
        //free(remove);
        list->head = temp;
    }
    list->count--;
}

void printList(LIST list){
    int count = list->count;
    NODE nodeptr = list->head;
    int i = 1;
    while(count>0){
        printf("%d ", i);
        if(nodeptr->tnt == 0) printT(nodeptr->val.t_val);
        else printNT(nodeptr->val.nt_val);
        printf("\n");
        nodeptr = nodeptr->next;
        count--;
        i++;
    }
}

LIST duplicateRuleIntoList(RULE list){
    LIST temp = createNewList();
    NODE ptr = list->head;
    while(ptr != NULL){
        insertNodeIntoList(duplicateNode(ptr),temp);
        ptr = ptr->next;
    }
    return temp;

}