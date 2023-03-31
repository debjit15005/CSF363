/*
ID: 2020A7PS1214P	Name: Darshan Abhaykumar
ID: 2020A7PS0970P	Name: Debjit Kar
ID:2020A7PS0986P	Name: Nidhish Parekh
*/
#ifndef HTABLE_H
#define HTABLE_H
#define MODULO 30

int hashcode(char* key);
token getToken(char *lexeme, Htable htable);
void insertId(Htable htable, char* key, token sym, int hashed);
void initTable(Htable htable);

#endif