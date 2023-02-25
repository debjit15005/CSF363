/* Definitions of data structures for lexer*/

#define MAX_LEXEME 100

typedef enum {

} token;

typedef struct {
    token token;
    int line;
    int val;
} tokenInfo;


