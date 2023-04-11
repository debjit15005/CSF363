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
#include "symbolTable.h"

// TODO: ADD LINE NUMBER TO ERRORS
// TODO: CHECK IF LHS OF RANGE <= RHS OF RANGE FOR ANY TYPE OF RANGE CONSTRUCT
// TODO: WIDTH AND OFFSET CHECKING FOR DYNAMIC ARRAYS
// TODO: if WHILE condition variables can change from get_value

GlobalSymTable gSymTable[MODULO]; // Creating a global symbol table with 'MODULO' entries

ASTNODE searchFor(ASTNODE asTree, NT key);
void printLocalTables(SYMTABLE htable);
RECURSESTRUCT populateChildTable(ASTNODE asTree, SYMTABLE currTable, int curroffset);
void markAllID(ASTNODE asTree, SYMTABLE currTable);

int getHashIndex(SYMTABLE htable, char* key, int hashed)
{
    if(hashed == -1) hashed = hashcode(key);
    SymTableEntry e1 = htable->entries[hashed];
    if(e1.valid == 1)
    {
        if(strcmp(e1.lexeme, key) == 0)
        {
            return hashed; // Found entry
        }
        return getHashIndex(htable, key, (++hashed)%MODULO);
    }
    else return -1; // Entry doesn't exist in table
}

void setSYMTABLEChild(SYMTABLE currTable, SYMTABLE childNode)
{
    childNode->parent = currTable;
    if(currTable->firstChild == NULL)
    {
        currTable->firstChild = childNode;
    }
    else
    {
        currTable = currTable->firstChild;
        while(currTable->nextSibling != NULL)
        {
            currTable = currTable->nextSibling;
        }
        currTable->nextSibling = childNode;
    }
}

void initSymTable()
{
    for(int i=0;i<MODULO;i++) gSymTable[i].valid = 0;
}

void insertGlobalSym(GLOBALSYMTABLE htable, char* key, int hashed) // TODO: ADD I/P AND O/P PARA LIST LATER
{   
    if(hashed == -1) hashed = hashcode(key); // hashcode() called from HTable.c
    if(htable[hashed].valid == 1)
    {
        if(strcmp(htable[hashed].lexeme, key) == 0)
        {
            // RETURN ERROR AS MODULE NAME ALREADY EXISTS (NO OVERLOADING ALLOWED)
            printf("\033[0;31mERROR %s MODULE NAME ALREADY EXISTS \033[0m \n", key);
            return;
        }
        insertGlobalSym(htable, key, (++hashed)%MODULO);
    }
    else
    {
        strcpy(htable[hashed].lexeme, key);
        htable[hashed].firstChild = (SYMTABLE) malloc(sizeof(struct SymTable));
        SYMTABLE tempTable = htable[hashed].firstChild;
        for(int i=0;i<MODULO;i++) tempTable->entries[i].valid = 0;
        tempTable->nesting = 0; // Initialise nesting level to 0
        htable[hashed].valid = 1;
    }
} 


int insertLocalSym(SYMTABLE htable, char* key, int hashed) // Checks if a key already exists in the table
{   
    if(hashed == -1) hashed = hashcode(key); // hashcode() called from HTable.c
    SymTableEntry e1 = htable->entries[hashed];
    if(e1.valid == 1)
    {
        if(strcmp(e1.lexeme, key) == 0)
        {
            // TODO: ADD CHECK FOR INPUT PARAMETER LIST
            return -1;
        }
        return insertLocalSym(htable, key, (++hashed)%MODULO);
    }
    else return hashed;
}   

SYMTABLE getfromGlobal(char *lexeme, GLOBALSYMTABLE htable){
	int hashed = hashcode(lexeme);
	while (htable[hashed].valid == 1){
		if(strcmp(htable[hashed].lexeme, lexeme) == 0) return htable[hashed].firstChild;
		hashed = (++hashed) % MODULO;
	}
    printf("\033[0;31mERROR NO SYMTABLE WITH THIS MODULE NAME \033[0m \n");
	return NULL; // REPORT ERROR IF YOU GET NO SYMTABLE WITH THIS MODULE NAME
}

SymTableEntry getEntryFromTable(char* key, SYMTABLE htable)
{
    int hashed = hashcode(key);
    SymTableEntry e1 = htable->entries[hashed];
	while (e1.valid == 1){
		if(strcmp(e1.lexeme, key) == 0)
        {
            return e1;
        }
		hashed = (++hashed) % MODULO;
        e1 = htable->entries[hashed];
	}
    return e1;
}


void createSymTable(ASTNODE asTree)
{
    ASTNODE tempChild = getASTChild(asTree, 0);
    ASTNODE searchModDecl = searchFor(tempChild, moduleDeclaration);
    printf("\n\n");
    
    // ADDING ALL MODULE DECLARATIONS
    while(searchModDecl != NULL)
    {
        insertGlobalSym(gSymTable, searchModDecl->firstChild->lexeme, -1);
        SYMTABLE childTable = getfromGlobal(searchModDecl->firstChild->lexeme, gSymTable);
        strcpy(childTable->moduleName, searchModDecl->firstChild->lexeme);
        searchModDecl = searchFor(searchModDecl->firstChild, moduleDeclaration);
    }

    // ADDING ALL OTHERMODULES 1 NAMES
    ASTNODE searchOtherMod = searchFor(tempChild, module);
    while(searchOtherMod != NULL) // TODO: TAKE CARE OF CASE WHEN MODULE DECLARATION AND DEFINITION BEFORE A CALL
    {
        searchOtherMod = searchOtherMod->firstChild;
        char modName[MAX_LEXEME];
        strcpy(modName, searchOtherMod->lexeme);
        insertGlobalSym(gSymTable, modName, -1);
        SYMTABLE childTable = getfromGlobal(modName, gSymTable);
        strcpy(childTable->moduleName, modName);

        // // CREATE I/P PARA LIST
        // SYMTABLE ipParaTable = (SYMTABLE) malloc(sizeof(struct SymTable)); 
        // for(int i = 0; i<MODULO; i++) ipParaTable->entries[i].valid = 0;
        // ipParaTable->nesting = -1; // Setting nesting as -1 to show that it is a I/P or O/P table

        // int current_offset = 0;
        // ASTNODE ipNode = searchOtherMod->nextSibling;
        // while(ipNode != NULL)
        // {
        //     ASTNODE keyNode = ipNode->firstChild;
        //     if(insertLocalSym(ipParaTable, keyNode->lexeme,-1) != -1)
        //     {
        //         ASTNODE type = keyNode->nextSibling;
        //         int isArray = 0, width = 0, isStatic = 0, mult = 0;
        //         char* r1 = (char *) malloc(10*sizeof(char));
        //         char* r2 = (char *) malloc(10*sizeof(char));
        //         if(type->val.nt_val == arrType)
        //         {
        //             isArray = 1;
        //             ASTNODE num1 = type->firstChild->firstChild;
        //             ASTNODE num2 = num1->nextSibling;
        //             type = type->firstChild->nextSibling;
        //             if(num1->firstChild->val.t_val == EPSILON) strcpy(r1, num1->firstChild->nextSibling->lexeme); // TODO: Check the variable names in arrRange
        //             else if(num1->firstChild->val.t_val == PLUS)
        //             {
        //                 strcpy(r1, "+");
        //                 strcat(r1,num1->firstChild->nextSibling->lexeme);
        //             }
        //             else
        //             {
        //                 strcpy(r1, "-");
        //                 strcat(r1,num1->firstChild->nextSibling->lexeme);
        //             }
        //             if(num2->firstChild->val.t_val == EPSILON) strcpy(r2, num2->firstChild->nextSibling->lexeme);
        //             else if(num2->firstChild->val.t_val == PLUS)
        //             {
        //                 strcpy(r2, "+");
        //                 strcat(r2,num2->firstChild->nextSibling->lexeme);
        //             }
        //             else
        //             {
        //                 strcpy(r2, "-");
        //                 strcat(r2,num2->firstChild->nextSibling->lexeme);
        //             }
        //             if(num1->firstChild->nextSibling->val.t_val == ID || num2->firstChild->nextSibling->val.t_val == ID) isStatic = 0;
        //             else isStatic = 1;
        //         }
        //         if(type->val.t_val == INTEGER) mult = 2;
        //         else if(type->val.t_val == REAL) mult = 4;
        //         else if(type->val.t_val == BOOLEAN) mult = 1;

        //         int entryIndex = getHashIndex(ipParaTable, keyNode->lexeme, -1);
        //         ipParaTable->entries[entryIndex].offset = current_offset;
        //         if(isArray == 1) // IF ARRAY TYPE
        //         {
        //             width = (atoi(r2)-atoi(r1)+1)*mult + 1;
        //             ipParaTable->entries[entryIndex].width = width;
        //             strcpy(ipParaTable->entries[entryIndex].r1, r1);
        //             strcpy(ipParaTable->entries[entryIndex].r2, r2);
        //             ipParaTable->entries[entryIndex].isStatic = isStatic;
        //             ipParaTable->entries[entryIndex].isArray = 1;
                    
        //         }
        //         else // IF NOT ARRAY TYPE
        //         {
        //             ipParaTable->entries[entryIndex].width = mult;
        //             ipParaTable->entries[entryIndex].isStatic = 0;
        //             ipParaTable->entries[entryIndex].isArray = 0;
        //         }
        //         ipParaTable->entries[entryIndex].valid = 1;
        //         ipParaTable->entries[entryIndex].type = type->val.t_val;
        //         strcpy(ipParaTable->entries[entryIndex].lexeme, keyNode->lexeme);
        //         current_offset += ipParaTable->entries[entryIndex].width;
        //     }
        //     else 
        //     {
        //         printf("\033[0;31mERROR %s is being redeclared \033[0m \n", keyNode->lexeme); // If a variable is being redeclared
        //     }
        //     ipNode = getASTChild(ipNode, 2);
        // }
        populateChildTable(searchOtherMod->nextSibling->nextSibling->nextSibling, childTable, 0); // TODO: ADD I/P AND O/P PARA LIST INSTEAD OF 3 NEXTSIBLINGS
        searchOtherMod = searchFor(searchOtherMod, module);
    }

    // ADDING ALL DRIVER 

    // ADDING ALL OTHERMODULES 2 NAMES

}

// If type = -1 returned then we received error
RECURSESTRUCT populateChildTable(ASTNODE asTree, SYMTABLE currTable, int curroffset)
{
    if(asTree == NULL)
    {
        printf("------SUB TREE IS NULL------\n");
        fflush(stdout);
        return NULL; 
    }
    RECURSESTRUCT node = (RECURSESTRUCT) malloc(sizeof(struct recurseStruct));
    if(asTree->tnt == 0)
    {
        node->type = asTree->val.t_val;
        if(asTree->val.t_val == NUM)
        {
            node->type = INTEGER;
            node->numval = atoi(asTree->lexeme);
        } 
        else if(asTree->val.t_val == RNUM) node->type = REAL;
        else if(asTree->val.t_val == TRUE || asTree->val.t_val == FALSE) node->type = BOOLEAN;
        if(asTree->val.t_val == ID)
        {
            SymTableEntry e1 = getEntryFromTable(asTree->lexeme, currTable);
            SYMTABLE recurseTable = currTable;
            while(recurseTable != NULL && (e1.valid != 1 || strcmp(asTree->lexeme, e1.lexeme) != 0))
            {
                recurseTable = recurseTable->parent;
                if(recurseTable != NULL) e1 = getEntryFromTable(asTree->lexeme, recurseTable);
            }
            if(recurseTable != NULL && strcmp(asTree->lexeme, e1.lexeme) == 0)
            {
                node->type = e1.type;
                node->isArray = e1.isArray;
                node->isFor = e1.isFor;
                if(e1.isWhile == 1) recurseTable->entries[getHashIndex(recurseTable, asTree->lexeme, -1)].changed = 1;
                if(e1.isArray == 1)
                {
                    node->isStatic = e1.isStatic;
                    strcpy(node->r1,e1.r1);
                    strcpy(node->r2,e1.r2);
                }
            } 
            else
            {
                printf("\033[0;31mERROR VARIABLE %s NOT DECLARED \033[0m \n", asTree->lexeme);
                node->type = -1;
            } 
        }
        return node;
    }
    
    ASTNODE currNode = asTree;
    int current_offset = curroffset;
    NT currNT = currNode->val.nt_val;
    if(currNT == declareStmt) // declare Statement
    {
        ASTNODE varID = currNode->firstChild;
        ASTNODE type = varID->nextSibling;
        int isArray = 0, width = 0, isStatic = 0, mult = 0;
        char* r1 = (char *) malloc(10*sizeof(char));
        char* r2 = (char *) malloc(10*sizeof(char));
        if(type->val.nt_val == arrType)
        {
            isArray = 1;
            ASTNODE num1 = type->firstChild->firstChild;
            ASTNODE num2 = num1->nextSibling;
            type = type->firstChild->nextSibling;
            if(num1->firstChild->val.t_val == EPSILON) strcpy(r1, num1->firstChild->nextSibling->lexeme); // TODO: Check the variable names in arrRange
            else if(num1->firstChild->val.t_val == PLUS)
            {
                strcpy(r1, "+");
                strcat(r1,num1->firstChild->nextSibling->lexeme);
            }
            else
            {
                strcpy(r1, "-");
                strcat(r1,num1->firstChild->nextSibling->lexeme);
            }
            if(num2->firstChild->val.t_val == EPSILON) strcpy(r2, num2->firstChild->nextSibling->lexeme);
            else if(num2->firstChild->val.t_val == PLUS)
            {
                strcpy(r2, "+");
                strcat(r2,num2->firstChild->nextSibling->lexeme);
            }
            else
            {
                strcpy(r2, "-");
                strcat(r2,num2->firstChild->nextSibling->lexeme);
            }
            if(num1->firstChild->nextSibling->val.t_val == ID || num2->firstChild->nextSibling->val.t_val == ID) isStatic = 0;
            else isStatic = 1;
        }
        if(type->val.t_val == INTEGER) mult = 2;
        else if(type->val.t_val == REAL) mult = 4;
        else if(type->val.t_val == BOOLEAN) mult = 1;
        while(varID != NULL)
        {
            int entryIndex = insertLocalSym(currTable,varID->lexeme,-1);
            if(entryIndex == -1) // IF THE ID ALREADY EXISTS
            {
                printf("\033[0;31mERROR %s is being redeclared \033[0m \n", varID->lexeme); // If a variable is being redeclared
                varID = varID->firstChild;
                continue; 
            }
            
            currTable->entries[entryIndex].offset = current_offset;
            if(isArray == 1) // IF ARRAY TYPE
            {
                width = (atoi(r2)-atoi(r1)+1)*mult + 1;
                currTable->entries[entryIndex].width = width;
                strcpy(currTable->entries[entryIndex].r1, r1);
                strcpy(currTable->entries[entryIndex].r2, r2);
                currTable->entries[entryIndex].isStatic = isStatic;
                currTable->entries[entryIndex].isArray = 1;
                
            }
            else // IF NOT ARRAY TYPE
            {
                currTable->entries[entryIndex].width = mult;
                currTable->entries[entryIndex].isStatic = 0;
                currTable->entries[entryIndex].isArray = 0;
            }
            currTable->entries[entryIndex].valid = 1;
            currTable->entries[entryIndex].type = type->val.t_val;
            strcpy(currTable->entries[entryIndex].lexeme, varID->lexeme);
            current_offset += currTable->entries[entryIndex].width;
           
            varID = varID->firstChild;
        }
        // printf("offset is %d\n", current_offset);
        populateChildTable(getASTChild(asTree, 2), currTable, current_offset);
    }
    else if(currNT == assignOp) // Assignment statement
    {
        ASTNODE child1 = getASTChild(asTree, 0);
        ASTNODE child2 = getASTChild(asTree, 1);
        RECURSESTRUCT type1 = populateChildTable(child1, currTable, current_offset);
        RECURSESTRUCT type2 = populateChildTable(child2, currTable, current_offset);
        if(type1->type != -1 && type2->type != -1)
        {
            if(type1->type != type2->type)
            {
                printf("ERROR Type Mismatch\n"); // If LHS and RHS are not of same type / same array type
                node->type = -1;
            }
            else if(type1->isFor == 1)
            {
                printf("\033[0;31mCannot reassign for loop variable \033[0m \n\n"); // If LHS and RHS are not of same type / same array type
                node->type = -1;
            }
            else if((type1->isArray ^ type2->isArray == 1) && (child1->val.nt_val != arrElement && child2->val.nt_val != arrElement ))
            {
                printf("ERROR Type Mismatch\n"); // If of the form A := B where A is integer and B is Array of integer
                node->type = -1;
            }
            else if(type1->isArray == 1 && type2->isArray == 1)
            {
                if(child1->val.nt_val != arrElement && child2->val.nt_val != arrElement) // If of the from A := B where A and B are both array of the same type
                {
                    if((atoi(type1->r2)-atoi(type1->r1)) != (atoi(type2->r2)-atoi(type2->r1))) // Check for bounds matching
                    {
                        printf("ERROR Type Mismatch\n");
                        node->type = -1;
                    }
                }
            }
        }
        else node->type = -1;
        populateChildTable(getASTChild(asTree, 2), currTable, current_offset);

    }
    else if(currNT == arrElement)
    {
        RECURSESTRUCT type1 = populateChildTable(getASTChild(asTree, 0), currTable, current_offset);
        RECURSESTRUCT type2 = populateChildTable(getASTChild(asTree, 1), currTable, current_offset);
        // printf("%d %d %d\n", atoi(type1->r1), type2->numval, atoi(type1->r2));
        // printf("%d\n", type1->isStatic);
        if(type1->type == -1 || type2->type == -1) node->type = -1;
        else if( type2->type != INTEGER) // CAN BE REMOVED
        {
            node->type = -1;
            printf("ERROR index needs to be an integer\n");
        }
        else
        {
            if(type1->isStatic == 1 && (atoi(type1->r1) <=  type2->numval && type2->numval <= atoi(type1->r2)))
            {
                // printf("Valid\n");
            }
            else
            {
                type1->type = -1;
                printf("ERROR index not within bounds\n");
            }
            // TODO: CHECK BOUNDS OF STATIC ARRAY ( DONE )
            // TODO: CHECK BOUNDS OF DYNAMIC ARRAY ????
            return type1;
        }
        
    }
    else if(currNT == getValue || currNT == printValue)
    {
        populateChildTable(getASTChild(asTree, 0), currTable, current_offset);
        populateChildTable(getASTChild(asTree, 1), currTable, current_offset);
    }
    else if(currNT == PLUSOp || currNT == MINUSOp || currNT == MULOp || currNT == DIVOp)
    {
        ASTNODE child1 = getASTChild(asTree, 0);
        ASTNODE child2 = getASTChild(asTree, 1);
        RECURSESTRUCT type1 = populateChildTable(child1, currTable, current_offset);
        RECURSESTRUCT type2 = populateChildTable(child2, currTable, current_offset);
        if(type1->type == -1 || type2->type == -1)
        {
            node->type = -1;
        }
        else
        {
            if(currNT == DIVOp) // If division operation then check divide by 0
            {
                if(type2->type == INTEGER && type2->numval == 0)
                {
                    printf("ERROR Cannot divide by 0\n");
                    node->type = -1;
                }
            }
            else
            {
                 if((type1->isArray == 1 && child1->val.nt_val != arrElement) || (type2->isArray == 1 && child2->val.nt_val != arrElement))
                {
                    printf("ERROR CANNOT PERFORM ARITHMETIC OPERATIONS ON ARRAY VARIABLES\n");
                }
                else if(type1->type != type2->type)
                {
                    printf("ERROR TYPE MISMATCH\n");
                }
            }
        }
    }
    else if(currNT == uniOp)
    {
        RECURSESTRUCT type2 = populateChildTable(getASTChild(asTree, 1), currTable, current_offset);
        if(type2->type == -1) node->type = -1;
        else node->type = type2->type;
    }
    else if(currNT == ANDOp || currNT == OROp)
    {   
        RECURSESTRUCT type1 = populateChildTable(getASTChild(asTree, 0), currTable, current_offset);
        RECURSESTRUCT type2 = populateChildTable(getASTChild(asTree, 1), currTable, current_offset);
        if(type1->type == -1 || type2->type == -1) node->type = -1;
        else
        {
            if(type1->type == BOOLEAN)
            {
                if(type2->type == BOOLEAN) node->type = BOOLEAN;
                else node->type = -1;
            }
        }
        
    }
    else if(currNT == LTOp || currNT == LEOp || currNT == GTOp || currNT == GEOp || currNT == EQOp || currNT == NEOp)
    {
        RECURSESTRUCT type1 = populateChildTable(getASTChild(asTree, 0), currTable, current_offset);
        RECURSESTRUCT type2 = populateChildTable(getASTChild(asTree, 1), currTable, current_offset);
        if(type1->type == -1 || type2->type == -1) node->type = -1;
        if(type1->type != type2->type)
        {
            printf("\033[0;31mERROR Type Mismatch \033[0m \n");
            node->type = -1;
        }
        else
        {
            if((currNT == LTOp || currNT == LEOp || currNT == GTOp || currNT == GEOp)&&(type1->type == BOOLEAN))
            {
                printf("\033[0;31mERROR Cannot compare boolean operands \033[0m \n");
                node->type = -1;
            }
        }
    }
    else if(currNT == forOp)
    {
        // MALLOC A NEW SYM TABLE, INITIALISE PARENT, CHILD, NESTING AND VALID 
        SYMTABLE childTable = (SYMTABLE) malloc(sizeof(struct SymTable)); 
        for(int i = 0; i<MODULO; i++) childTable->entries[i].valid = 0;
        childTable->nesting = currTable->nesting+1;
        setSYMTABLEChild(currTable, childTable);
        strcpy(childTable->moduleName, currTable->moduleName);

        RECURSESTRUCT type1 = populateChildTable(getASTChild(asTree, 0)->firstChild, currTable, current_offset);
        if(type1->type == -1 || type1->type != INTEGER)
        {
            if(type1->type != -1) printf("\033[0;31mERROR For loop variable must be integer \033[0m \n");
            node->type = -1;
        } 
        else
        {
            currTable->entries[getHashIndex(currTable, getASTChild(asTree, 0)->firstChild->lexeme, -1)].isFor = 1;
            populateChildTable(getASTChild(asTree, 1), childTable, current_offset); // USE THIS TO RECURSIVELY CREATE TREE OF HASHTABLES
        }
        
        populateChildTable(getASTChild(asTree, 2), currTable, current_offset); // To continue after the FOR block is done
    }
    else if(currNT == switchOp)
    {
        ASTNODE childNode = getASTChild(asTree, 0); // ID
        RECURSESTRUCT type1 = populateChildTable(childNode, currTable, current_offset);

        if(type1->type == -1) node->type = -1;
        else
        {
            if((type1->isArray == 1 && childNode->val.nt_val != arrElement)||(type1->type == REAL))
            {
                printf("\033[0;31mERROR invalid switch variable\033[0m \n");
                node->type = -1;
            }
            else if(type1->type == BOOLEAN || type1->type == INTEGER)
            {
                ASTNODE caseNode = getASTChild(asTree, 1)->firstChild; // CONSTRUCT: case
                ASTNODE caseIter = caseNode;
                while(caseIter != NULL)
                {
                    token switchVal = getASTChild(caseIter, 0)->val.t_val;
                    if(switchVal != NUM)
                    {
                        printf("\033[0;31mERROR case statement value doesn't match type\033[0m \n");
                    node->type = -1;
                    }

                    // MALLOC A NEW SYM TABLE, INITIALISE PARENT, CHILD, NESTING AND VALID 
                    SYMTABLE childTable = (SYMTABLE) malloc(sizeof(struct SymTable)); 
                    for(int i = 0; i<MODULO; i++) childTable->entries[i].valid = 0;
                    childTable->nesting = currTable->nesting+1;
                    setSYMTABLEChild(currTable, childTable);
                    strcpy(childTable->moduleName, currTable->moduleName);
                    populateChildTable(getASTChild(caseIter, 1), childTable, current_offset);
                    caseIter = getASTChild(caseIter, 2);
                }
                if(type1->type == INTEGER && caseNode->nextSibling->firstChild->val.t_val == EPSILON) // no default statement
                {
                    printf("\033[0;31mERROR default statement required\033[0m \n");
                    node->type = -1;
                }
                else if(type1->type == BOOLEAN && caseNode->nextSibling->firstChild->val.t_val != EPSILON)
                {
                    printf("\033[0;31mERROR default statement not required\033[0m \n");
                    node->type = -1;
                }
                else // default statement
                {
                    SYMTABLE childTable = (SYMTABLE) malloc(sizeof(struct SymTable)); 
                    for(int i = 0; i<MODULO; i++) childTable->entries[i].valid = 0;
                    childTable->nesting = currTable->nesting+1;
                    setSYMTABLEChild(currTable, childTable);
                    strcpy(childTable->moduleName, currTable->moduleName);
                    populateChildTable(caseNode->nextSibling, childTable, current_offset);
                } 
            }
        }
        populateChildTable(getASTChild(asTree, 2), currTable, current_offset); // To continue after the SWITCH block is done
    }
    else if(currNT == whileOp)
    {
        RECURSESTRUCT type1 = populateChildTable(getASTChild(asTree, 0), currTable, current_offset);
        markAllID(getASTChild(asTree, 0), currTable);


        SYMTABLE childTable = (SYMTABLE) malloc(sizeof(struct SymTable)); // For the WHILE block
        for(int i = 0; i<MODULO; i++) childTable->entries[i].valid = 0;
        childTable->nesting = currTable->nesting+1;
        setSYMTABLEChild(currTable, childTable);
        strcpy(childTable->moduleName, currTable->moduleName);
        populateChildTable(getASTChild(currNode, 1), childTable, current_offset);

        int conditionVariableChanged = 0;
        // Check if condition variable got changed
        for(int i = 0; i<MODULO; i++)
        {
            if(currTable->entries[i].valid == 1)
            {
                if(currTable->entries[i].isWhile == 1 && currTable->entries[i].changed == 1) conditionVariableChanged = 1;
            }
        }
        if(conditionVariableChanged == 0)
        {
            printf("\033[0;31mERROR condition variable not changed\033[0m \n");
            node->type = -1;
        }


        populateChildTable(getASTChild(asTree, 2), currTable, current_offset); // To continue after the WHILE block is done
    }
    return node; 
}

ASTNODE searchFor(ASTNODE asTree, NT key)
{
    if(asTree == NULL) return NULL;
    ASTNODE currLevel = asTree;
    while(asTree != NULL && asTree->val.nt_val != key)
    {
        asTree = asTree->nextSibling;
    }
    if(asTree == NULL) return NULL;
    return asTree;
}

void printGlobalTable(GLOBALSYMTABLE htable)
{
    printf("%-10s %-10s %-10s %-10s %-10s %-10s %-10s %-10s %-10s %-10s\n","var","module","scope","type","isArray","isStatic","ArrayRange","width","offset","nesting");
    for(int i = 0; i<MODULO; i++)
    {
        if(htable[i].valid == 1)
        {
            printLocalTables(htable[i].firstChild);
        }
    }
}

void printLocalTables(SYMTABLE htable)
{
    if(htable == NULL) return;
    printLocalTables(htable->firstChild);
    
    for(int i = 0; i<MODULO; i++)
    {
        if(htable->entries[i].valid == 1)
        {
            SymTableEntry currEntry = htable->entries[i];
            
            printf("%-10s ", currEntry.lexeme);
            printf("%-10s ", htable->moduleName);
            // printf("%-10s ", currEntry.lexeme); // TODO: ADD scope line
            printf("%-10s ", "PLACEHOLDER");
            printf("%-10d ", currEntry.type); // type
            if(currEntry.isArray == 1)
            {
                printf("%-10s ", "yes"); // isArray
                if(currEntry.isStatic == 1) printf("%-10s ", "static"); // isStatic == 1
                else printf("%-10s ", "dynamic"); // isStatic == 0
                printf("[%-3s,%-3s]  ", currEntry.r1, currEntry.r2);
            }
            else
            {
                printf("%-10s ", "no"); // isArray
                printf("%-10s ", "*");; // isStatic
                printf("%-10s ", "*"); // ARRAY RANGE
            } 
            printf("%-10d ", currEntry.width);
            printf("%-10d ", currEntry.offset);
            printf("%-10d ", htable->nesting);
            // printf("%-10d ", currEntry.isWhile); // FOR TESTING
            // printf("%-10d ", currEntry.changed); // FOR TESTING
            printf("\n");
        }
    }
    printLocalTables(htable->nextSibling);
}

void markAllID(ASTNODE asTree, SYMTABLE currTable)
{
    if(asTree == NULL) return;

    markAllID(asTree->firstChild, currTable);
    if(asTree->val.t_val == ID)
    {
        SymTableEntry e1 = getEntryFromTable(asTree->lexeme, currTable);
        SYMTABLE recurseTable = currTable;
        while(recurseTable != NULL && (e1.valid != 1 || strcmp(asTree->lexeme, e1.lexeme) != 0))
        {
            recurseTable = recurseTable->parent;
            if(recurseTable != NULL) e1 = getEntryFromTable(asTree->lexeme, recurseTable);
        }
        if(recurseTable != NULL && strcmp(asTree->lexeme, e1.lexeme) == 0) // TODO: ADD CHECKING IN PARENT TABLE IF NOT FOUND IN CURRENT TABLE (DONE)
        {
            int entryIndex = getHashIndex(recurseTable, asTree->lexeme, -1);
            recurseTable->entries[entryIndex].isWhile = 1;
            recurseTable->entries[entryIndex].changed = 0;
        }
    }
    markAllID(asTree->nextSibling, currTable);
}
