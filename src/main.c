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
char look_ahead(char *input)
{
    if (input != NULL)
    {
        return *(input + 1);
    }
    return '\0';
}
/*
The lexer consists of two functions
a function that parse a token, and a function that calls that function until all tokens are parsed

The function that parse a token is called LexerState lexer_parse_next(char **input);
The function that calls lexer_parse_next() is called void lexer_parse_all(char *input);

lexer_parse_next(char **input) logic:
-It starts with the state normal in each call
-It checks the current character to determine the next state of the next loop iterration
-Based on the previous state, the function handles the case until the token is constructed and returned
-It has a buffer to accumulate word tokens

Example ("ls")
-First iterration 'l'
    -Normal state code runs
        -'l' is a character meaning we are in a word,
            -Accumulate the letter
            -advance the current_character_pointer
            -set the state to be IN_WORD
-Second iterration 's'
    -Word state runs
        -'s' is a character
            -Accumulate 'l'
            -advance the current_character_pointer
            -keep the state the same
-Third iterration null-terminator
    -Word state runs
        -'\0'
            -Doesn't staisfy the letters to accumulated condition
            -Terminate the buffer
            -return the token
-Fourth iterration NULL
    -The loop won't work
    -TOKEN_EOF is returned
    -lexer_parse_all loop is terminated

-It returns we a token is constructed
-It returns a TOKEN_EOF when there's no more tokens to parse to stop the loop in
*/
LexerState lexer_parse_next(char **input)
{
    // The job of this function is to return a token and advance the string to the last consumed character
    // Must be used until an TOKEN_EOF is reached (the input can't staisfy any of the cases to create a token)
    LexerState state = NORMAL;
    ssize_t buffer_index = 0;
    char buffer[1024];
    char *current_character_pointer = *input;
    while (*current_character_pointer)
    {
        char character = *current_character_pointer;
        switch ((state))
        {
        case NORMAL:
            if (character == ' ' || character == '\t' || character == '\n')
            {
                current_character_pointer++;
            }
            if (character == '|')
            {
                current_character_pointer++;
                state = IN_PIPELINE_OR_OR;
            }
            if (character == '&')
            {
                state = IN_AND;
                current_character_pointer++;
            }
            if (character == '"')
            {
                state = IN_QUOTES;
                current_character_pointer++;
            }
            if (character == '\'')
            {
                state = IN_SINGLE_QUOTES;
                current_character_pointer++;
            }
            if (isalnum(character) || character == '-' || character == '/')
            {
                state = IN_WORD;
                buffer[buffer_index] = character;
                buffer_index++;
                current_character_pointer++;
            }
            break;
        case IN_PIPELINE_OR_OR:
            if (character == '|')
            {
                char next_character = look_ahead(current_character_pointer);
                if (next_character == '\0')
                {
                    printf("OR MUST BE FOLLOWED BY SOMETHING\n");
                    exit(EXIT_FAILURE);
                }
                else if (next_character != ' ')
                {
                    printf("OR MUST BE FOLLOWED BY A SPACE\n");
                    exit(EXIT_FAILURE);
                }
                current_character_pointer++;
                *input = current_character_pointer;
                printf("OR:||\n");
                return OR;
            }
            else
            {
                current_character_pointer++;
                *input = current_character_pointer;
                printf("PIPELINE:|\n");
                return PIPELINE;
            }
            break;
        case IN_AND:
            if (character == '&')
            {
                char next_character = look_ahead(current_character_pointer);
                if (next_character == '\0')
                {
                    printf("AND MUST BE FOLLOWED BY SOMETHING\n");
                    exit(EXIT_FAILURE);
                }
                else if (next_character != ' ')
                {
                    printf("AND MUST BE FOLLOWED BY A SPACE\n");
                    exit(EXIT_FAILURE);
                }
                printf("AND:&&\n");
                current_character_pointer++;
                *input = current_character_pointer;
                return AND;
            }
            else
            {
                printf("& MUST BE FOLLOWED BY &\n");
                exit(EXIT_FAILURE);
            }
            break;
        case IN_WORD:

            if (isalnum(character) || character == '-' || character == '/')
            {
                buffer[buffer_index] = character;
                buffer_index++;
                current_character_pointer++;
            }
            else
            {
                buffer[buffer_index] = '\0';
                current_character_pointer++;
                *input = current_character_pointer;
                printf("WORD:%s\n", buffer);
                return IN_WORD;
            }
            break;
        case IN_QUOTES:
            if (character == '\'')
            {
                // ERROR:END OF A DOUBLE QUOTE WITH A SINGLE QUOTE
                printf("MISMATCHED QUOTES:DOUBLE THEN SINGLE");
                exit(EXIT_FAILURE);
            }
            if (character == '"')
            {
                // END OF A DOUBLE QUOTE
                current_character_pointer++;
                *input = current_character_pointer;
                buffer[buffer_index] = '\0';
                printf("WORD:%s\n", buffer);
                return IN_WORD;
            }
            else
            {
                // Accumlate
                buffer[buffer_index] = character;
                buffer_index++;
                current_character_pointer++;
            }
            break;
        case IN_SINGLE_QUOTES:
            if (character == '"')
            {
                // ERROR:END OF A DOUBLE QUOTE WITH A SINGLE QUOTE
                printf("MISMATCHED QUOTES:SINGLE THEN DOUBLE");
                exit(EXIT_FAILURE);
            }
            if (character == '\'')
            {
                // END OF A SINGLE QUOTE
                current_character_pointer++;
                *input = current_character_pointer;
                buffer[buffer_index] = '\0';
                printf("WORD:%s\n", buffer);
                return IN_WORD;
            }
            else
            {
                // Accumlate
                buffer[buffer_index] = character;
                buffer_index++;
                current_character_pointer++;
            }
        }
    }
    return TOKEN_EOF;
}
void lexer_parse_all(char *input)
{
    while (lexer_parse_next(&input) != TOKEN_EOF)
        ;
}
int main(char **argv, int argc)
{
    size_t len;
    char *line = NULL;
    while (1)
    {
        printf("shell> ");
        getline(&line, &len, stdin);
        lexer_parse_all(line);
    }
}