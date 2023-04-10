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

GlobalSymTable gSymTable[MODULO]; // Creating a global symbol table with 'MODULO' entries

ASTNODE searchFor(ASTNODE asTree, NT key);
void printLocalTables(SYMTABLE htable);
RECURSESTRUCT populateChildTable(ASTNODE asTree, SYMTABLE currTable, int curroffset);

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

int insertLocalSym(SYMTABLE htable, char* key, int hashed)
{   
    if(hashed == -1) hashed = hashcode(key); // hashcode() called from HTable.c
    SymTableEntry e1 = htable->entries[hashed];
    if(e1.valid == 1)
    {
        if(strcmp(e1.lexeme, key) == 0)
        {
            // TODO: ADD CHECK FOR INPUT PARAMETER LIST
            printf("\033[0;31mERROR %s is being redeclared \033[0m \n", key); // If a variable is being redeclared
            return 0;
        }
        return insertLocalSym(htable, key, (++hashed)%MODULO);
    }
    else
    {
        return 1;
    } 
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
    printf("\033[0;31mERROR VARIABLE %s NOT DECLARED \033[0m \n", key);
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
        populateChildTable(searchOtherMod->nextSibling->nextSibling->nextSibling, childTable, 0); // TODO: ADD I/P AND O/P PARA LIST INSTEAD OF 3 NEXTSIBLINGS
        searchOtherMod = searchFor(searchOtherMod, module);
    }

    // ADDING ALL DRIVER 

    // ADDING ALL OTHERMODULES 2 NAMES

}

// If valid = -1 returned then we received error
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
            // printf("%s\n", asTree->lexeme);
            if(strcmp(e1.lexeme, asTree->lexeme) == 0) // TODO: ADD CHECKING IN PARENT TABLE IF NOT FOUND IN CURRENT TABLE
            {
                node->type = e1.type;
                node->isArray = e1.isArray;
                if(e1.isArray == 1)
                {
                    node->isStatic = e1.isStatic;
                    strcpy(node->r1,e1.r1);
                    strcpy(node->r2,e1.r2);
                }
            } 
            else node->type = -1;
            return node;
        }
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
            if(insertLocalSym(currTable,varID->lexeme,-1) == 0) // IF THE ID ALREADY EXISTS
            {
                varID = varID->firstChild;
                continue; 
            }
            int entryIndex = hashcode(varID->lexeme);
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
            else if((type1->isArray ^ type2->isArray == 1) && (child1->val.nt_val != arrElement && child2->val.nt_val != arrElement ))
            {
                printf("ERROR Type Mismatch\n"); // If of the form A := B where A is integer and B is Array of integer
                node->type = -1;
            }
            else if(type1->isArray == 1 && type2->isArray == 1)
            {
                if(child1->val.nt_val != arrElement && child2->val.nt_val != arrElement) // IF not of type A[5] := B[5]
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
            printf("\033[0;31m Type Mismatch \033[0m \n");
            node->type = -1;
        }
        else
        {
            if((currNT == LTOp || currNT == LEOp || currNT == GTOp || currNT == GEOp)&&(type1->type == BOOLEAN))
            {
                printf("\033[0;31m Cannot compare boolean operands \033[0m \n");
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
        childTable->parent = currTable;

        RECURSESTRUCT type1 = populateChildTable(getASTChild(asTree, 0)->firstChild, currTable, current_offset);
        if(type1 == -1) node->type = -1;
        else
        {

            populateChildTable(getASTChild(asTree, 1), childTable, 0); // USE THIS TO RECURSIVELY CREATE TREE OF HASHTABLES
        }
        


        populateChildTable(getASTChild(asTree, 2), currTable, current_offset); // To continue after the FOR block is done
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
            // printf("%-s\t ", currEntry.lexeme); // TODO: ADD scope line
            // printf("\t ");
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
            printf("\n");
        }
    }
    printLocalTables(htable->nextSibling);
}

