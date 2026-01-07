#include "lexer.h"
int main(char **argv, int argc)
{
    size_t len;
    char *line = NULL;
    while (1)
    {
        printf("shell> ");
        getline(&line, &len, stdin);
        tokenize_all(line);
    }
}