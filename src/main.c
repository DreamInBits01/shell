#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
enum StateMachine
{
    NORMAL,
    IN_WORD,
    IN_QUOTES,
    IN_SINGLE_QUOTES,
    ESCAPED,
    STATES_COUNT
};
enum Tokens
{
    PIPELINE,
    CMD,
    QUOTE,
    ARGUMENT,
    TOKENS_COUNT
};
typedef struct
{
    char *value;
    int type;
} Token;
void parse(char *line)
{
    size_t index = 0;
    int lastTokenType;
    Token tokens[10] = {0};
    char *token = strtok(line, " ");
    while (token != NULL && index < 10)
    {
        if (index == 0)
        {
            tokens[index] = (Token){token, CMD};
            lastTokenType = CMD;
        }
        else if (strncmp(token, "|", 1) == 0)
        {
            tokens[index] = (Token){token, PIPELINE};
            lastTokenType = PIPELINE;
        }
        else if (lastTokenType == PIPELINE)
        {
            tokens[index] = (Token){token, CMD};
            lastTokenType = CMD;
        }
        else if (lastTokenType == CMD && strncmp(token, "|", 1) != 0)
        {
            // args
            printf("Arg:%s\n", token);
            lastTokenType = ARGUMENT;
        }
        if (lastTokenType != ARGUMENT)
        {
            index++;
        }
        token = strtok(NULL, " ");
    }
    for (size_t i = 0; i < index; i++)
    {
        printf("Token:%s, type:%d\n", tokens[i].value, tokens[i].type);
        /* code */
    }
}
int main(char **argv, int argc)
{
    size_t len;
    char *line = NULL;
    while (1)
    {
        printf("shell> ");
        getline(&line, &len, stdin);
        parse(line);
    }
}