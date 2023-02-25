# include <stdio.h>
# include "lexerDef.h"


FILE *getStream(FILE *fp);
tokenInfo getNextToken();
void removeComments(char *testcaseFile, char *cleanFile);

int line;
char[]