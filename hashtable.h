#ifndef HTABLE_H
#define HTABLE_H
#define MODULO 35
#include "lexerDef.h"

int hashcode(char* key);
token getToken(char *lexeme, Htable htable);
void insertId(Htable htable, char* key, token sym, int hashed);
void initTable(Htable htable);

#endif