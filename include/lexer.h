#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
typedef enum
{
    NORMAL,
    IN_WORD,
    IN_QUOTES,
    IN_REDIRECTION,
    IN_AND,
    IN_PIPELINE_OR_OR,
    IN_SINGLE_QUOTES,
    TOKEN_EOF,
    STATES_COUNT
} LexerState;
typedef enum
{
    PIPELINE,
    OR,
    AND,
    CMD,
    ARGUMENT,
    REDIRECTION,
    REDIRECTION_APPEND,
    TOKENS_COUNT
} Tokens;
typedef struct
{
    char *value;
    int type;
} Token;
LexerState lex_next_token(char **input);
void tokenize_all(char *input);
char look_ahead(char *input);
#endif