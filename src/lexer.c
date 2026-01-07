#include "lexer.h"
/*
The lexer consists of two functions
a function that parse a token, and a function that calls that function until all tokens are parsed

The function that parse a token is called LexerState lexer_parse_next(char **input);
lexer_parse_all(char *input); calls lexer_parse_next() until TOKEN_EOF is returned

lexer_parse_next(char **input) logic:
-It starts with the state normal in each call
-It checks the current character to determine the next state of the next loop iterration
-It has a buffer to accumulate word tokens
-Based on the previous state, the function handles the case until the token is constructed and returned
-When a token is constructed, the current_character_pointer gets increment
and the string gets set to the current_character_pointer so that the next token can be constructed from there
-The state repeats from normal in the next call
-After all of the string is processes, the lexer lexer_parse_next is going to return TOKEN_EOF


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
            -increment the current_character_pointer
            -set the string to that pointer
            -return the token
-Fourth iterration NULL
    -The loop won't work
    -TOKEN_EOF is returned
    -lexer_parse_all loop is terminated

Example (ls || grep)
"ls" is parsed same as above, and the lexer_parse_next is called
-First iterration (state is normal)
    -'|' the state is either a pipeline or a logical OR
-Second itteration (IN_PIPELINE_OR_OR)
    -check if the character is '|', it's then we must construct a token and update the string
    -error handling also takes place
    -If it was anything else, then it's a pipeline and a pipeline token is constructed

*/
char look_ahead(char *input)
{
    if (input != NULL)
    {
        return *(input + 1);
    }
    return '\0';
}
LexerState lex_next_token(char **input)
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
        /*
        If the state is normal,
        -We should decide the current state based on the character,
        -Increment the current_character_pointer
        -The second iterration will take care of the token parsing
            -error handling
            -construction
            -accumulating
        */
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
void tokenize_all(char *input)
{
    while (lex_next_token(&input) != TOKEN_EOF)
        ;
}