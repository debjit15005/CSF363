/*
Group No. : 42
ID: 2020A7PS1214P	Name: Darshan Abhaykumar
ID: 2020A7PS0970P	Name: Debjit Kar
ID:2020A7PS0986P	Name: Nidhish Parekh
*/

/*
	This is the menu driven file. To begin executing the code 
	1> Type 'make'
	2> Type './runCompiler <input_file> <parseTreeOutput> size_of_buffer'
*/

#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "grammar.h"
#include "parser.h"
#include "tree.h"
#include "stack_ll.h"
#include "linkedlist.h"
#include "Nlinkedlist.h"
#include "hashtable.h"
#include "treeDef.h"
#include <time.h>
#include "symbolTable.h"

int main(int argc, char*argv[]){
 
	printf("---------- CAPABILITY OF THE COMPILER ---------- \n\n");
	printf(" --> Twin Buffer implemented\n");
	printf(" --> Implemented lexer module completely\n");
	printf(" --> Union struct used to save on memory\n");
	printf(" --> Parse Table implemented (Can be printed in grammar.c\n");
	printf(" --> Can construct and print N-ary tree\n");
	printf("-------------------------------------------------- \n\n");

	printf("---------- SHORTCOMINGS OF THE COMPILER ---------- \n\n");
	printf(" --> Parser doesn't work \n");
	printf(" --> Parse tree not complete (N-ary tree created) \n");
	printf(" --> First and Follow not automated\n");
	printf("-------------------------------------------------- \n\n");
	
	int option = 1;
    initializeLexerOnce();

	if(option > 0){
		printf("\n\n\n");
		printf("Press 0 to exit.\n");
		printf("Press 1 to invoke lexer only.\n");
		printf("Press 2 to invoke parser.\n");
        printf("Press 3 to print AST\n");
        printf("Press 4 to display the amount of allocated memory and number of nodes in parse tree and abstract syntax tree\n");
        printf("Press 5 to print the symbol table\n");
        printf("Press 6 to print total memory requirement for each function\n");
		printf("Press 7 to print the type expressions of array variables\n");
        printf("Press 8 to compile and verify syntactic and semantic correctness of input source code and report any syntax, type checking and semantic errors, and to measure total time taken by compiler\n");
        printf("Press 9 to generate assembly code\n");
		printf("\n");
		scanf("%d", &option);
		switch(option){
			case 0:{//done
				return 0;
			}
			case 1:{//done
				runLexer(argv[1], atoi(argv[3]));
				break;
			}
			case 2:{//done
				buildGrammar();
				automaticFirsts();

                automaticFollows();
                createParseTable();
                int** parseTable = getParseTable();
                ASTNODE asTree = parseInputSourceCode(argv[1], parseTable, 0);
                printTree(t1,0);
                
				break;
			}
            case 3:{//done
				buildGrammar();
				automaticFirsts();
                automaticFollows();
                createParseTable();
                int** parseTable = getParseTable();
                ASTNODE asTree = parseInputSourceCode(argv[1], parseTable, 0);
                printAST(asTree,0);
				break;
			}
            case 4:{//done
				buildGrammar();
				automaticFirsts();
                automaticFollows();
                createParseTable();
                int** parseTable = getParseTable();
                ASTNODE asTree = parseInputSourceCode(argv[1], parseTable, 0);
                int count = countNodes(asTree);
                printf("Parse tree number of nodes = %d, Allocated Memory = %ld bytes\n", parseTreeNodeCount, sizeof(struct TreeNode)*parseTreeNodeCount);
                printf("Abstract Syntax tree number of nodes = %d, Allocated Memory = %ld bytes\n", count, (long int) (sizeof(struct ASTNode)*count));
                float fract = ((parseTreeNodeCount - 1.0*count)/parseTreeNodeCount)*100;
                printf("Compression Percentage = %f\n", fract);

				break;
			}
            case 5:{//done
				buildGrammar();
				automaticFirsts();
                automaticFollows();
                createParseTable();
                int** parseTable = getParseTable();

                ASTNODE asTree = parseInputSourceCode(argv[1], parseTable, 0);

                initSymTable();
                createSymTable(asTree);
                printGlobalTable(gSymTable);

				break;
			}
            case 6:{
				buildGrammar();
				automaticFirsts();
                automaticFollows();
                createParseTable();
                int** parseTable = getParseTable();
                ASTNODE asTree = parseInputSourceCode(argv[1], parseTable, 0);
                initSymTable();
                createSymTable(asTree);
                // printActivationRecord(gSymTable);

				break;
			}
            case 7:{//done
				buildGrammar();
				automaticFirsts();
                automaticFollows();
                createParseTable();
                int** parseTable = getParseTable();
                ASTNODE asTree = parseInputSourceCode(argv[1], parseTable, 0);
                initSymTable();
                createSymTable(asTree);
                // printArrays(gSymTable);

				break;
			}
           

			case 8:{
				clock_t start_time, end_time;
				double total_CPU_time, total_CPU_time_in_seconds;

				start_time = clock();

				buildGrammar();
				automaticFirsts();
                automaticFollows();
                createParseTable();
                int** parseTable = getParseTable();
                ASTNODE asTree = parseInputSourceCode(argv[1], parseTable, 1);
                if(syntacticError == 0)
                {
                    initSymTable();
                    createSymTable(asTree);
                }

				end_time = clock();

				total_CPU_time = (double) (end_time - start_time);
				total_CPU_time_in_seconds = total_CPU_time /CLOCKS_PER_SEC;

				printf("total_CPU_time = %f \n",total_CPU_time);
				printf("total_CPU_time_in_seconds = %f seconds \n",total_CPU_time_in_seconds);
				break;
			}
		}
	}
}