/*
Group No. : 42
ID: 2020A7PS1214P	Name: Darshan Abhaykumar
ID: 2020A7PS0970P	Name: Debjit Kar
ID:2020A7PS0986P	Name: Nidhish Parekh
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

GlobalSymTable gSymTable[MODULO]; // Creating a global symbol table with 'MODULO' entries
int gOffset = 0;
ASTNODE searchFor(ASTNODE asTree, NT key);
void printLocalTables(SYMTABLE htable);
RECURSESTRUCT populateChildTable(ASTNODE asTree, SYMTABLE currTable);
void markAllID(ASTNODE asTree, SYMTABLE currTable);
void doOtherModules(ASTNODE searchOtherMod);
void printParaList(SymTableEntry list[], char* modName, int size);

int forArray[10] = {0};
int forArrayIndex = -1;

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

int getHashIndexfromGlobal(GLOBALSYMTABLE htable, char* key, int hashed) // -1 if entry doesn't exist else returns entry index
{
    if(hashed == -1) hashed = hashcode(key);
    if(htable[hashed].valid == 1)
    {
        if(strcmp(htable[hashed].lexeme, key) == 0)
        {
            return hashed;
        }
        return getHashIndexfromGlobal(htable, key, (++hashed)%MODULO);
    }
    else return -1;
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
int insertGlobalSym(GLOBALSYMTABLE htable, char key[], int hashed) // -1 if entry already exists, 0 if successful
{   
    if(hashed == -1) hashed = hashcode(key); // hashcode() called from HTable.c
    if(htable[hashed].valid == 1)
    {
        if(strcmp(htable[hashed].lexeme, key) == 0)
        {
            return -1;
        }
        return insertGlobalSym(htable, key, (++hashed)%MODULO);
    }
    else
    {
        htable[hashed].valid = 1;
        strcpy(htable[hashed].lexeme, key);
        htable[hashed].firstChild = (SYMTABLE) malloc(sizeof(struct SymTable));
        SYMTABLE tempTable = htable[hashed].firstChild;
        for(int i=0;i<MODULO;i++) tempTable->entries[i].valid = 0;
        tempTable->nesting = 0; // Initialise nesting level to 0
        return 0;
    }
} 


int insertLocalSym(SYMTABLE htable, char* key, int hashed) // Checks if a key already exists in the table
{   
    if(hashed == -1) hashed = hashcode(key); // hashcode() called from HTable.c
    SymTableEntry e1 = htable->entries[hashed];
    if(e1.valid == 1)
    {
        if(strcmp(e1.lexeme, key) == 0) return -1; // RETURNS -1 IF IT EXISTS
        return insertLocalSym(htable, key, (++hashed)%MODULO);
    }
    else return hashed; // RETURNS INDEX AT WHICH WE CAN INSERT
}   

SYMTABLE getfromGlobal(char* key, GLOBALSYMTABLE htable){
	int hashed = hashcode(key);
	while (htable[hashed].valid == 1){
		if(strcmp(htable[hashed].lexeme, key) == 0) return htable[hashed].firstChild;
		hashed = (++hashed) % MODULO;
	}
    if(printErrors)printf("\033[0;31mERROR NO SYMTABLE WITH THIS MODULE NAME \033[0m \n");
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
    ASTNODE driverMod = searchFor(tempChild, driverModule);
    ASTNODE searchOtherMod2 = searchFor(driverMod, module);
    printf("\n\n");
    
    // ADDING ALL MODULE DECLARATIONS
    while(searchModDecl != NULL)
    {
        int gindex = insertGlobalSym(gSymTable, searchModDecl->firstChild->lexeme, -1);
        if(gindex == -1)
        {
            if(printErrors) printf("\033[0;31mLINE %d ERROR %s MODULE NAME ALREADY EXISTS \033[0m \n", searchModDecl->firstChild->line_no, searchModDecl->firstChild->lexeme);
            searchModDecl = searchFor(searchModDecl->firstChild, moduleDeclaration);
            continue;
        }
        gSymTable[getHashIndexfromGlobal(gSymTable, searchModDecl->firstChild->lexeme, -1)].isDec = 1;
        SYMTABLE childTable = getfromGlobal(searchModDecl->firstChild->lexeme, gSymTable);
        strcpy(childTable->moduleName, searchModDecl->firstChild->lexeme);
        searchModDecl = searchFor(searchModDecl->firstChild, moduleDeclaration);
    }

    // ADDING ALL OTHERMODULES 1 NAMES
    ASTNODE searchOtherMod = searchFor(tempChild, module);
    if(searchOtherMod != NULL && searchOtherMod->val.nt_val == module)
    {
        doOtherModules(searchOtherMod);
    }
    gOffset = 0;

    // ADDING ALL DRIVER
    if(driverMod->val.nt_val == driverModule)
    {
        insertGlobalSym(gSymTable, "driver", -1);
        SYMTABLE childTable = getfromGlobal("driver", gSymTable);
        strcpy(childTable->moduleName,"driver");
        int start = getASTChild(driverMod->firstChild, 1)->line_no;
        int end = getASTChild(driverMod->firstChild, 2)->line_no;
        childTable->symScope[0] = start;
        childTable->symScope[1] = end;
        populateChildTable(driverMod->firstChild->firstChild, childTable);
    }

    gOffset = 0;
    // ADDING ALL OTHERMODULES 2 NAMES
    if(searchOtherMod2 != NULL && searchOtherMod2->val.nt_val == module)
    {
        doOtherModules(searchOtherMod2);
    }
}

int searchList(SymTableEntry list[], char* key) // -1 if not present else gives index of found element
{
    for(int i = 0; i<50; i++)
    {
        if(list[i].valid == 1)
        {
            if(strcmp(list[i].lexeme, key) == 0) return i;
        }
    }
    return -1;
}

// If type = -1 returned then we received error
RECURSESTRUCT populateChildTable(ASTNODE asTree, SYMTABLE currTable)
{
    if(asTree == NULL)
    {
        return NULL; 
    }
    RECURSESTRUCT node = (RECURSESTRUCT) malloc(sizeof(struct recurseStruct));
    if(asTree->tnt == 0)
    {
        node->line = asTree->line_no;
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
            int foundInPara = 0;
            while(recurseTable != NULL && (e1.valid != 1 || strcmp(asTree->lexeme, e1.lexeme) != 0))
            {
                recurseTable = recurseTable->parent;
                if(recurseTable != NULL) e1 = getEntryFromTable(asTree->lexeme, recurseTable);
            }
            
            if(recurseTable == NULL)
            {
                int index2 = getHashIndexfromGlobal(gSymTable, currTable->moduleName, -1);
                foundInPara = searchList(gSymTable[index2].ip_list, asTree->lexeme);
                if(foundInPara == -1) // returns -1 if not found or else returns index
                {
                    foundInPara = searchList(gSymTable[index2].op_list, asTree->lexeme);
                    if(foundInPara != -1) e1 = gSymTable[index2].op_list[foundInPara];
                }
                else e1 = gSymTable[index2].ip_list[foundInPara];
            }
            if(foundInPara != -1 || (recurseTable != NULL && strcmp(asTree->lexeme, e1.lexeme) == 0))
            {
                node->type = e1.type;
                node->isArray = e1.isArray;
                
                if(e1.isFor == 1)
                {
                    node->isFor = e1.isFor;
                }
                if(e1.isOp == 1)
                {
                    SymTableEntry* OPLIST = gSymTable[getHashIndexfromGlobal(gSymTable, currTable->moduleName, -1)].op_list;
                    OPLIST[searchList(OPLIST, asTree->lexeme)].isOp = -1;
                    memcpy(gSymTable[getHashIndexfromGlobal(gSymTable, currTable->moduleName, -1)].op_list, OPLIST, sizeof(OPLIST));
                }
                // if(e1.isWhile == 1) recurseTable->entries[getHashIndex(recurseTable, asTree->lexeme, -1)].changed = 1;
                if(e1.isArray == 1)
                {
                    node->isStatic = e1.isStatic;
                    strcpy(node->r1,e1.r1);
                    strcpy(node->r2,e1.r2);
                }
            }
            else
            {
                if(printErrors)printf("\033[0;31mLINE %d ERROR VARIABLE %s NOT DECLARED \033[0m \n", asTree->line_no, asTree->lexeme);
                node->type = -1;
            } 
        }
        return node;
    }
    
    ASTNODE currNode = asTree;
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
            if(num1->firstChild->val.t_val == EPSILON) strcpy(r1, num1->firstChild->nextSibling->lexeme);
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
                if(printErrors)printf("\033[0;31mLINE %d ERROR %s is being redeclared \033[0m \n", varID->line_no, varID->lexeme); // If a variable is being redeclared
                varID = varID->firstChild;
                continue; 
            }
            
            currTable->entries[entryIndex].offset = gOffset;
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
            currTable->entries[entryIndex].scope[0] = currTable->symScope[0];
            currTable->entries[entryIndex].scope[1] = currTable->symScope[1];

            currTable->entries[entryIndex].valid = 1;
            currTable->entries[entryIndex].type = type->val.t_val;
            strcpy(currTable->entries[entryIndex].lexeme, varID->lexeme);
            gOffset += currTable->entries[entryIndex].width;
           
            varID = varID->firstChild;
        }
        populateChildTable(getASTChild(asTree, 2), currTable);
    }
    else if(currNT == assignOp) // Assignment statement
    {
        ASTNODE child1 = getASTChild(asTree, 0);
        ASTNODE child2 = getASTChild(asTree, 1);
        RECURSESTRUCT type1 = populateChildTable(child1, currTable);
        RECURSESTRUCT type2 = populateChildTable(child2, currTable);
        if(type1->type != -1 && type2->type != -1)
        {
            if(type1->isFor == 1)
            {
                forArray[forArrayIndex] = 1;
            }
            else if(type1->type != type2->type)
            {
                if(printErrors)printf("\033[0;31mLINE %d ERROR Type Mismatch \033[0m \n", child1->line_no); // If LHS and RHS are not of same type / same array type
                node->type = -1;
                node->line = child1->line_no;
            }
            else if((type1->isArray ^ type2->isArray == 1) && (child1->val.nt_val != arrElement && child2->val.nt_val != arrElement ))
            {
                if(printErrors)printf("\033[0;31mLINE %d ERROR Type Mismatch \033[0m \n", child1->line_no); // If of the form A := B where A is an integer and B is an Array of integer
                node->type = -1;
                node->line = child1->line_no;
            }
            else if(type1->isArray == 1 && type2->isArray == 1)
            {
                if(child1->val.nt_val != arrElement && child2->val.nt_val != arrElement) // If of the from A := B where A and B are both arrays of the same type
                {
                    if((atoi(type1->r2)-atoi(type1->r1)) != (atoi(type2->r2)-atoi(type2->r1))) // Check for bounds matching
                    {
                        
                        if(printErrors) printf("\033[0;31mLINE %d ERRO Type Mismatch \033[0m \n", child1->line_no);
                        node->type = -1;
                        node->line = child1->line_no;
                    }
                }
            }
        }
        else node->type = -1;
        populateChildTable(getASTChild(asTree, 2), currTable);

    }
    else if(currNT == arrElement)
    {
        // printT(getASTChild(asTree, 1)->val.t_val);
        RECURSESTRUCT type1 = populateChildTable(getASTChild(asTree, 0), currTable);
        RECURSESTRUCT type2 = populateChildTable(getASTChild(asTree, 1), currTable);

        if(getASTChild(asTree, 1)->val.nt_val == signedNum)
        {
            ASTNODE num2 = getASTChild(getASTChild(asTree, 1), 0);
            if(num2->val.t_val == PLUS)
            {
                type2->numval = atoi(num2->nextSibling->lexeme);
            }
            else if(num2->val.t_val == MINUS)
            {
                type2->numval = -1*atoi(num2->nextSibling->lexeme);
            }
        }
        

        if(type1->type == -1 || type2->type == -1) node->type = -1;
        else
        {
            if(type1->isStatic == 1 && (atoi(type1->r1) >  type2->numval || type2->numval > atoi(type1->r2)))
            {
                
                node->type = -1;
                node->line = type1->line;
                if(printErrors)printf("\033[0;31mLINE %d ERROR Index not in bounds \033[0m \n", type1->line);
            }
            else node->type = type1->type;
        }
        
    }
    else if(currNT == getValue || currNT == printValue)
    {
        RECURSESTRUCT type1 = populateChildTable(getASTChild(asTree, 0), currTable);
        RECURSESTRUCT type2 = populateChildTable(getASTChild(asTree, 1), currTable);
    }
    else if(currNT == PLUSOp || currNT == MINUSOp || currNT == MULOp || currNT == DIVOp)
    {
        ASTNODE child1 = getASTChild(asTree, 0);
        ASTNODE child2 = getASTChild(asTree, 1);
        RECURSESTRUCT type1 = populateChildTable(child1, currTable);
        RECURSESTRUCT type2 = populateChildTable(child2, currTable);
        if(type1->type == -1 || type2->type == -1)
        {
            node->type = -1;
            if(type1->line != 0) node->line = type1->line;
            else if(type2->line != 0) node->line = type2->line;
        }
        else
        {
            if((type1->isArray == 1 && child1->val.nt_val != arrElement) || (type2->isArray == 1 && child2->val.nt_val != arrElement))
            {
                if(printErrors)printf("\033[0;31mLINE %d ERROR Types of operands are different \033[0m \n", type2->line);
                node->type = -1;
                node->line = type2->line;
            }
            else if(type1->type != type2->type)
            {
                if(type2->line == 0) 
                {
                    if(printErrors)printf("\033[0;31mLINE %d ERROR Type Mismatch \033[0m \n", type1->line);
                }
                else if(type1->line == 0) 
                {
                    if(printErrors)printf("\033[0;31mLINE %d ERROR Type Mismatch \033[0m \n", type2->line);
                }
                node->type = -1;
                node->line = type2->line;
            }
            else
            {
                if(currNT == DIVOp)
                {
                    node->type == REAL;
                    node->line = type2->line;
                } 
                else node->type = type1->type;
            } 
        }
    }
    else if(currNT == uniOp)
    {
        RECURSESTRUCT type2 = populateChildTable(getASTChild(asTree, 1), currTable);
        if(type2->type == -1)
        {
            node->type = -1;
            node->line = type2->line;
        } 
        else node->type = type2->type;
    }
    else if(currNT == ANDOp || currNT == OROp)
    {   
        RECURSESTRUCT type1 = populateChildTable(getASTChild(asTree, 0), currTable);
        RECURSESTRUCT type2 = populateChildTable(getASTChild(asTree, 1), currTable);
        if(type1->type == -1 || type2->type == -1)
        {
            node->type = -1;
            if(type1->line != 0) node->line = type1->line;
            else if(type2->line != 0) node->line = type2->line;
        } 
        else
        {
            if(type1->type == BOOLEAN)
            {
                if(type2->type == BOOLEAN)
                {
                    node->type = BOOLEAN;
                    node->line = type2->line;
                } 
                else
                {
                    node->type = -1;
                    if(type1->line != 0) node->line = type1->line;
                    else if(type2->line != 0) node->line = type2->line;
                } 
            }
        }
        
    }
    else if(currNT == LTOp || currNT == LEOp || currNT == GTOp || currNT == GEOp || currNT == EQOp || currNT == NEOp)
    {
        RECURSESTRUCT type1 = populateChildTable(getASTChild(asTree, 0), currTable);
        RECURSESTRUCT type2 = populateChildTable(getASTChild(asTree, 1), currTable);
        if(type1->type == -1 || type2->type == -1)
        {
            node->type = -1;
            if(type1->line != 0) node->line = type1->line;
            else if(type2->line != 0) node->line = type2->line;
        } 
        
        if(type1->type != type2->type)
        {
            if(type1->line == 0) 
            {
                if(printErrors)printf("\033[0;31mLINE %d ERROR Type Mismatch \033[0m \n", type2->line);
            }
            else if(type2->line == 0)
            {   
                if(printErrors)printf("\033[0;31mLINE %d ERROR Type Mismatch \033[0m \n", type1->line);
            } 
            node->type = -1;
            if(type1->line != 0) node->line = type1->line;
            else if(type2->line != 0) node->line = type2->line;
        }
        else
        {
            if((currNT == LTOp || currNT == LEOp || currNT == GTOp || currNT == GEOp)&&(type1->type == BOOLEAN))
            {
                if(printErrors)printf("\033[0;31mLINE %d ERROR Cannot compare boolean operands \033[0m \n", type1->line);
                node->type = -1;
                node->line = type1->line;
            }
            else
            {
                node->type = BOOLEAN;
                node->line = type1->line;
                return node;
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
        int start = getASTChild(currNode, 2)->line_no;
        int end = getASTChild(currNode, 3)->line_no;

        childTable->symScope[0] = start;
        childTable->symScope[1] = end;
        RECURSESTRUCT type1 = populateChildTable(getASTChild(asTree, 0)->firstChild, currTable);
        if(type1->type == -1 || type1->type != INTEGER)
        {
            if(type1->type != -1) if(printErrors)printf("\033[0;31mLINE [%d,%d] ERROR For loop variable must be integer \033[0m \n", start, end);
            node->type = -1;
            node->line = type1->line;
        } 
        else
        {
            SymTableEntry* IpList = gSymTable[getHashIndexfromGlobal(gSymTable, currTable->moduleName, -1)].ip_list;
            SymTableEntry* OpList = gSymTable[getHashIndexfromGlobal(gSymTable, currTable->moduleName, -1)].op_list;
            int indx = getHashIndex(currTable, getASTChild(asTree, 0)->firstChild->lexeme, -1);
            if(indx == -1) // If not found in Tree of Symbol Tables
            {
                indx = searchList(IpList, getASTChild(asTree, 0)->firstChild->lexeme); // Search in I/P List
                if(indx != -1)
                {
                    IpList[indx].isFor = 1;
                    memcpy(gSymTable[getHashIndexfromGlobal(gSymTable, currTable->moduleName, -1)].ip_list, IpList, sizeof(IpList));
                }
                else // If not found in I/P List
                {
                    indx = searchList(OpList, getASTChild(asTree, 0)->firstChild->lexeme); // Search in O/P List
                    if(indx != -1)
                    {
                        OpList[indx].isFor = 1;
                        memcpy(gSymTable[getHashIndexfromGlobal(gSymTable, currTable->moduleName, -1)].op_list, OpList, sizeof(OpList));
                    }
                }
            }
            else currTable->entries[indx].isFor = 1;
            forArray[++forArrayIndex] = 0; // If it gets set to 1 after call then we know for variable got changed
            RECURSESTRUCT type2 = populateChildTable(getASTChild(asTree, 1), childTable); // USE THIS TO RECURSIVELY CREATE TREE OF HASHTABLES
            
            if(forArray[forArrayIndex--] == 1) if(printErrors)printf("\033[0;31mLINE [%d,%d] ERROR Cannot reassign FOR loop variable \033[0m \n", start, end);
        }
        populateChildTable(getASTChild(asTree, 4), currTable); // To continue after the FOR block is done
    }
    else if(currNT == switchOp)
    {
        ASTNODE childNode = getASTChild(asTree, 0); // ID
        RECURSESTRUCT type1 = populateChildTable(childNode, currTable);
        int startSwitch = childNode->nextSibling->nextSibling->line_no;
        int endSwitch = childNode->nextSibling->nextSibling->nextSibling->line_no;
        if((type1->isArray == 1 && childNode->val.nt_val != arrElement)||(type1->type == REAL))
        {
            if(printErrors)printf("\033[0;31mLINE [%d,%d] ERROR invalid switch variable\033[0m \n", startSwitch, endSwitch);
            node->type = -1;
            node->line = type1->line;
        }
        else
        {
            ASTNODE caseNode = getASTChild(asTree, 1)->firstChild; // CONSTRUCT: case
            ASTNODE caseIter = caseNode;
            int caseStart = 0;
            int caseEnd = 0;
            while(caseIter != NULL) // CONSTRUCT: case
            {
                caseStart = getASTChild(caseIter, 2)->line_no;
                
                if(caseNode->nextSibling->firstChild->val.t_val == EPSILON)
                {
                    caseEnd = getASTChild(caseNode->nextSibling, 0)->line_no;
                }
                else caseEnd = endSwitch;
                if(getASTChild(caseIter, 3) != NULL)
                {
                    caseEnd =  getASTChild(getASTChild(caseIter, 3), 2)->line_no;
                }
                token switchVal = getASTChild(caseIter, 0)->val.t_val;
                if(switchVal == NUM ^ type1->type == INTEGER)
                {
                    if(printErrors)printf("\033[0;31mLINE [%d,%d] ERROR case statement value doesn't match typ3e\033[0m \n", caseStart, caseEnd-1);
                    node->type = -1;
                    node->line = type1->line;
                }
                else if ((switchVal == TRUE || switchVal == FALSE) ^ (type1->type == BOOLEAN))
                {
                    if(printErrors)printf("\033[0;31mLINE [%d,%d] ERROR case statement value doesn't match type\033[0m \n", caseStart, caseEnd-1);
                    node->type = -1;
                    node->line = type1->line;
                }
                else if(switchVal == REAL)
                {
                    if(printErrors)printf("\033[0;31mLINE [%d,%d] ERROR case statement value doesn't match type\033[0m \n", caseStart, caseEnd-1);
                    node->type = -1;
                    node->line = type1->line;
                }
                

                // MALLOC A NEW SYM TABLE, INITIALISE PARENT, CHILD, NESTING AND VALID 
                SYMTABLE childTable = (SYMTABLE) malloc(sizeof(struct SymTable)); 
                for(int i = 0; i<MODULO; i++) childTable->entries[i].valid = 0;
                childTable->nesting = currTable->nesting+1;
                childTable->symScope[0] = caseStart;
                childTable->symScope[1] = caseEnd-1;
                setSYMTABLEChild(currTable, childTable);
                strcpy(childTable->moduleName, currTable->moduleName);
                
                populateChildTable(getASTChild(caseIter, 1), childTable);
                caseIter = getASTChild(caseIter, 3);
            }
            if(type1->type == INTEGER && caseNode->nextSibling->firstChild->val.t_val == EPSILON) // no default statement
            {
                if(printErrors)printf("\033[0;31mLINE [%d,%d] ERROR default statement required\033[0m \n", startSwitch, endSwitch);
                node->type = -1;
                node->line = type1->line;
            }
            else if(type1->type == BOOLEAN && caseNode->nextSibling->firstChild->val.t_val != EPSILON)
            {
                if(printErrors)printf("\033[0;31mLINE [%d,%d] ERROR default statement not required - Invalid Syntax\033[0m \n", startSwitch, endSwitch);
                node->type = -1;
                node->line = type1->line;
            }
            else // default statement
            {
                SYMTABLE childTable = (SYMTABLE) malloc(sizeof(struct SymTable)); 
                for(int i = 0; i<MODULO; i++) childTable->entries[i].valid = 0;
                childTable->nesting = currTable->nesting+1;
                
                childTable->symScope[0] = getASTChild(caseNode->nextSibling, 0)->line_no;
                childTable->symScope[1] = endSwitch;

                setSYMTABLEChild(currTable, childTable);
                strcpy(childTable->moduleName, currTable->moduleName);
                populateChildTable(getASTChild(caseNode->nextSibling, 1), childTable);
            } 
        }
        populateChildTable(getASTChild(asTree, 4), currTable); // To continue after the SWITCH block is done
    }
    else if(currNT == whileOp)
    {
        RECURSESTRUCT type1 = populateChildTable(getASTChild(asTree, 0), currTable);
        // markAllID(getASTChild(currNode, 0), currTable);

        SYMTABLE childTable = (SYMTABLE) malloc(sizeof(struct SymTable)); // For the WHILE block
        for(int i = 0; i<MODULO; i++) childTable->entries[i].valid = 0;
        childTable->nesting = currTable->nesting+1;
        setSYMTABLEChild(currTable, childTable);
        strcpy(childTable->moduleName, currTable->moduleName);
        int start = getASTChild(currNode, 2)->line_no;
        int end = getASTChild(currNode, 3)->line_no;
        childTable->symScope[0] = start;
        childTable->symScope[1] = end;
        populateChildTable(getASTChild(currNode, 1), childTable);

        // int conditionVariableChanged = 0;
        // Check if condition variable got changed
        // for(int i = 0; i<MODULO; i++)
        // {
        //     if(currTable->entries[i].valid == 1)
        //     {
        //         if(currTable->entries[i].isWhile == 1 && currTable->entries[i].changed == 1) conditionVariableChanged = 1;
        //     }
        // }
        // SymTableEntry* IPList;
        // IPList = gSymTable[getHashIndexfromGlobal(gSymTable, currTable->moduleName, -1)].ip_list;
        // for(int i = 0; i<MODULO; i++)
        // {
        //     if(IPList[i].valid == 1)
        //     {
        //         if(IPList[i].isWhile == 1 && IPList[i].changed == 1) conditionVariableChanged = 1;
        //     }
        // }
        // if(conditionVariableChanged == 0)
        // {
        //     if(printErrors)printf("\033[0;31mLINE [%d,%d] ERROR condition variable not changed\033[0m \n", start, end);
        //     node->type = -1;
        // }
        populateChildTable(getASTChild(asTree, 4), currTable); // To continue after the WHILE block is done
    }
    else if(currNT == funCallOp)
    {
        ASTNODE optN = getASTChild(asTree, 0);
        if(optN->val.nt_val == optional)
        {
            ASTNODE outputPara = optN->firstChild;
            ASTNODE funcID = optN->nextSibling;
            if(strcmp(funcID->lexeme, currTable->moduleName) == 0)
            {
                if(printErrors)printf("\033[0;31mLINE %d ERROR Function cannot call itself\033[0m \n", funcID->line_no);
                node->line = funcID->line_no;
            }
            int index1 = getHashIndexfromGlobal(gSymTable, funcID->lexeme, -1);
            if(index1 == -1)
            {
                if(printErrors)printf("\033[0;31mLINE %d ERROR function not found\033[0m \n", funcID->line_no);
                node->line = funcID->line_no;
            }
            else
            {
                if(gSymTable[index1].isDec == 1) gSymTable[index1].isDec = 2; // If function is called after declaration then set to 2
                int error = 0;
                int currPara = 0;
                int saveLine = 0;
                while(outputPara != NULL)
                {
                    RECURSESTRUCT type1 = populateChildTable(outputPara, currTable);
                    if(type1->type == -1)
                    {
                        error = 1;
                        node->line = type1->line;
                        break;
                    } 
                    saveLine = type1->line;
                    SymTableEntry* OutputParaList = gSymTable[index1].op_list;
                    int paraIndex = -1;
                    for(int i = 0; i<gSymTable[index1].sizeOp; i++)
                    {
                        if(OutputParaList[i].valid == 1)
                        {
                            if(OutputParaList[i].paraNum == currPara) paraIndex = i;
                        }
                    }
                    if(paraIndex == -1)
                    {
                        error = 1;
                        node->line = type1->line;
                        if(printErrors)printf("\033[0;31mLINE %d ERROR output parameters don't match\033[0m \n", type1->line);
                        break;
                    }
                    if(type1->type != OutputParaList[paraIndex].type)
                    {
                        error = 1;
                        node->line = type1->line;
                        if(printErrors)printf("\033[0;31mLINE %d ERROR output parameters don't match\033[0m \n", type1->line);
                        break;
                    }
                    currPara++;
                    outputPara = outputPara->firstChild;
                }
                if(error == 0)
                {
                    if(gSymTable[index1].sizeOp != currPara)
                    {
                        if(printErrors)printf("\033[0;31mLINE %d ERROR number of output parameters don't match\033[0m \n", saveLine);
                    }
                    currPara = 0;
                    ASTNODE inputPara = funcID->nextSibling;
                    while(inputPara != NULL)
                    {
                        RECURSESTRUCT type1 = populateChildTable(getASTChild(inputPara, 1), currTable);
                        saveLine = type1->line;
                        SymTableEntry* InputParaList = gSymTable[index1].ip_list;
                        int paraIndex = -1;
                        for(int i = 0; i<gSymTable[index1].sizeIp; i++)
                        {
                            if(InputParaList[i].valid == 1)
                            {
                                if(InputParaList[i].paraNum == currPara) paraIndex = i;
                            }
                        }
                        if(paraIndex == -1)
                        {
                            error = 1;
                            node->line = type1->line;
                            if(printErrors)printf("\033[0;31mLINE %d ERROR input parameters don't match\033[0m \n", type1->line);
                            break;
                        }
                        if(type1->isArray != InputParaList[paraIndex].isArray ||type1->type != InputParaList[paraIndex].type)
                        {
                            error = 1;
                            node->line = type1->line;
                            if(printErrors)printf("\033[0;31mLINE %d ERROR input parameters don't match\033[0m \n", type1->line);
                            break;
                        }
                        currPara++;
                        inputPara = getASTChild(inputPara, 2);
                    }
                    if(error != 1 && gSymTable[index1].sizeIp != currPara)
                    {
                        if(printErrors)printf("\033[0;31mLINE %d ERROR number of input parameters don't match\033[0m \n", saveLine);
                        node->line = saveLine;
                    }
                }
            }
        }
        populateChildTable(optN->nextSibling->nextSibling->nextSibling, currTable);
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
            printParaList(htable[i].ip_list, htable[i].lexeme, htable[i].sizeIp);
            printParaList(htable[i].op_list, htable[i].lexeme, htable[i].sizeOp);
            printLocalTables(htable[i].firstChild);
        }
    }
}

void printParaList(SymTableEntry list[], char* modName, int size)
{
    for(int i = 0; i<size; i++)
    {
        if(list[i].valid == 1)
        {
            SymTableEntry currEntry = list[i];
            printf("%-10s ", currEntry.lexeme);
            printf("%-10s ", modName);
            printf("[%-3d,%-3d]  ", currEntry.scope[0], currEntry.scope[1]);
            if(currEntry.type == INTEGER) printf("%-10s ", "INTEGER"); // type
            else if(currEntry.type == REAL) printf("%-10s ", "REAL"); // type
            else if(currEntry.type == BOOLEAN) printf("%-10s ", "BOOLEAN"); // type
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
            printf("%-10d ", 0);
            printf("\n");
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
            printf("[%-3d,%-3d]  ", currEntry.scope[0], currEntry.scope[1]);
            if(currEntry.type == INTEGER) printf("%-10s ", "INTEGER"); // type
            else if(currEntry.type == REAL) printf("%-10s ", "REAL"); // type
            else if(currEntry.type == BOOLEAN) printf("%-10s ", "BOOLEAN"); // type
            
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
        if(recurseTable != NULL && strcmp(asTree->lexeme, e1.lexeme) == 0)
        {
            int entryIndex = getHashIndex(recurseTable, asTree->lexeme, -1);
            recurseTable->entries[entryIndex].isWhile = 1;
            recurseTable->entries[entryIndex].changed = 0;
        }
    }
    markAllID(asTree->nextSibling, currTable);
}

void  doOtherModules(ASTNODE searchOtherMod)
{
    
    while(searchOtherMod != NULL) 
    {
        searchOtherMod = searchOtherMod->firstChild;
        char modName[MAX_LEXEME];
        strcpy(modName, searchOtherMod->lexeme);
        SYMTABLE childTable;
        SymTableEntry ipParaTable[50];
        SymTableEntry OpParaTable[50];
        int start = searchFor(searchOtherMod, moduleDef)->firstChild->nextSibling->line_no;
        int end = searchFor(searchOtherMod, moduleDef)->firstChild->nextSibling->nextSibling->line_no;
        int store = insertGlobalSym(gSymTable, modName, -1);
        if(gSymTable[getHashIndexfromGlobal(gSymTable, modName, -1)].isDec == 2 || store != -1)
        {
            childTable = getfromGlobal(modName, gSymTable);
            if(childTable == NULL)
            {
                printf("Table not created\n");
                fflush(stdout);
            } 
            childTable->symScope[0] = start;
            childTable->symScope[1] = end;
            strcpy(childTable->moduleName, modName);
            strcpy(modName, searchOtherMod->lexeme);
        }
        else
        {
            if(gSymTable[getHashIndexfromGlobal(gSymTable, modName, -1)].isDec == 1) // Declared but not called
            {
                if(printErrors)printf("\033[0;31mLINE [%d,%d] ERROR %s MODULE DEFINITION BEFORE CALL \033[0m \n", start, end, modName);
            }
            else if(printErrors)printf("\033[0;31mLINE [%d,%d] ERROR %s MODULE NAME ALREADY EXISTS \033[0m \n", start, end, modName);
            searchOtherMod = searchFor(searchOtherMod, module);
            continue;
        }
        int hashIndex = getHashIndexfromGlobal(gSymTable, modName, -1);
        if(hashIndex == -1) printf("ERROR !!!!!!!!!!!!!!!!!!!\n");
        int sizeIP = 0; // To store the size of I/P
        ASTNODE ipNode = searchOtherMod->nextSibling;
        while(ipNode != NULL)
        {
            ASTNODE keyNode = ipNode->firstChild;
            int entryIndex = sizeIP;
            if(entryIndex != -1)
            {
                ASTNODE type = keyNode->nextSibling;
                int isArray = 0, width = 0, isStatic = 0, mult = 0;
                char* r1 = (char *) malloc(10*sizeof(char));
                char* r2 = (char *) malloc(10*sizeof(char));
                if(type->val.nt_val == arrType)
                {
                    isArray = 1;
                    ASTNODE num1 = type->firstChild->firstChild;
                    ASTNODE num2 = num1->nextSibling;
                    type = type->firstChild->nextSibling;
                    if(num1->firstChild->val.t_val == EPSILON) strcpy(r1, num1->firstChild->nextSibling->lexeme);
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

                
                ipParaTable[entryIndex].offset = gOffset;
                if(isArray == 1) // IF ARRAY TYPE
                {
                    width = (atoi(r2)-atoi(r1)+1)*mult + 1;
                    ipParaTable[entryIndex].width = width;
                    strcpy(ipParaTable[entryIndex].r1, r1);
                    strcpy(ipParaTable[entryIndex].r2, r2);
                    ipParaTable[entryIndex].isStatic = isStatic;
                    ipParaTable[entryIndex].isArray = 1;
                    
                }
                else // IF NOT ARRAY TYPE
                {
                    ipParaTable[entryIndex].width = mult;
                    ipParaTable[entryIndex].isStatic = 0;
                    ipParaTable[entryIndex].isArray = 0;
                }
                ipParaTable[entryIndex].scope[0] = start;
                ipParaTable[entryIndex].scope[1] = end;
                ipParaTable[entryIndex].paraNum = sizeIP;
                ipParaTable[entryIndex].valid = 1;
                ipParaTable[entryIndex].type = type->val.t_val;
                strcpy(ipParaTable[entryIndex].lexeme, keyNode->lexeme);
                gOffset += ipParaTable[entryIndex].width;
                sizeIP++;
            }
            else 
            {
                if(printErrors)printf("\033[0;31mERROR %s is being redeclared \033[0m \n", keyNode->lexeme); // If a variable is being redeclared
            }
            ipNode = getASTChild(ipNode, 2);
        }

        // CREATE O/P PARA LIST
        int sizeOP = 0; // To store the size of the o/p para list
        ASTNODE OpNode = searchOtherMod->nextSibling->nextSibling->firstChild;
        while(OpNode != NULL)
        {
            ASTNODE keyNode = OpNode->firstChild;
            int entryIndex = sizeOP;
            if(entryIndex != -1)
            {   
                ASTNODE type = keyNode->nextSibling;
                int isArray = 0, width = 0, isStatic = 0, mult = 0;
                char* r1 = (char *) malloc(10*sizeof(char));
                char* r2 = (char *) malloc(10*sizeof(char));
                if(type->val.nt_val == arrType)
                {
                    isArray = 1;
                    ASTNODE num1 = type->firstChild->firstChild;
                    ASTNODE num2 = num1->nextSibling;
                    type = type->firstChild->nextSibling;
                    if(num1->firstChild->val.t_val == EPSILON) strcpy(r1, num1->firstChild->nextSibling->lexeme);
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

                
                OpParaTable[entryIndex].offset = gOffset;
                if(isArray == 1) // IF ARRAY TYPE
                {
                    width = (atoi(r2)-atoi(r1)+1)*mult + 1;
                    OpParaTable[entryIndex].width = width;
                    strcpy(OpParaTable[entryIndex].r1, r1);
                    strcpy(OpParaTable[entryIndex].r2, r2);
                    OpParaTable[entryIndex].isStatic = isStatic;
                    OpParaTable[entryIndex].isArray = 1;
                    
                }
                else // IF NOT ARRAY TYPE
                {
                    OpParaTable[entryIndex].width = mult;
                    OpParaTable[entryIndex].isStatic = 0;
                    OpParaTable[entryIndex].isArray = 0;
                }
                OpParaTable[entryIndex].scope[0] = start;
                OpParaTable[entryIndex].scope[1] = end;
                OpParaTable[entryIndex].paraNum = sizeOP;
                OpParaTable[entryIndex].isOp = 1;
                OpParaTable[entryIndex].valid = 1;
                OpParaTable[entryIndex].type = type->val.t_val;
                strcpy(OpParaTable[entryIndex].lexeme, keyNode->lexeme);
                gOffset += OpParaTable[entryIndex].width;
                sizeOP++;
            }
            else 
            {
                if(printErrors)printf("\033[0;31mERROR %s is being redeclared \033[0m \n", keyNode->lexeme); // If a variable is being redeclared
            }
            OpNode = getASTChild(OpNode, 2);
        }
        memcpy(gSymTable[hashIndex].ip_list, ipParaTable, sizeof(ipParaTable));
        memcpy(gSymTable[hashIndex].op_list, OpParaTable, sizeof(OpParaTable));
        gSymTable[hashIndex].sizeIp = sizeIP;
        gSymTable[hashIndex].sizeOp = sizeOP;
        populateChildTable(searchOtherMod->nextSibling->nextSibling->nextSibling->firstChild, childTable);
        // TO CHECK IF OP PARAMETERS GOT CHANGED
        SymTableEntry* temporary = gSymTable[hashIndex].op_list;
        int outputVariableChanged = 0; // 0 is default, -1 is if output variable changed
        for(int i = 0; i<50; i++) 
        {
            if(temporary[i].valid == 1)
            {
                if(temporary[i].isOp == -1) outputVariableChanged = 1; // isOp will be set to -1 if that variable is changed
            }
        }
        if(outputVariableChanged != 1)
        {
            if(printErrors)printf("\033[0;31mLINE [%d,%d] ERROR Output variables unchanged \033[0m \n", start, end);
        }
        searchOtherMod = searchFor(searchOtherMod, module);
    }
}
