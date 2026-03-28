#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

char input[100];
int i = 0;

void declaration();
void type();
void id_list();
void id_tail();
void expr();
void skipSpaces();

void skipSpaces()
{
    while(input[i] == ' ')
        i++;
}

/* expr -> id | num */
void expr()
{
    skipSpaces();

    if(isalpha(input[i]))   // identifier
    {
        while(isalpha(input[i]) || isdigit(input[i]))
            i++;
    }
    else if(isdigit(input[i]))   // number
    {
        while(isdigit(input[i]))
            i++;
    }
    else
    {
        printf("Invalid expression\n");
        exit(0);
    }
}

/* id_tail -> , id id_tail | = expr id_tail | ε */
void id_tail()
{
    skipSpaces();

    if(input[i] == ',')
    {
        i++;
        expr();
        id_tail();
    }
    else if(input[i] == '=')
    {
        i++;
        expr();
        id_tail();
    }
}

/* id_list -> id id_tail */
void id_list()
{
    expr();
    id_tail();
}

/* type -> int | float | char */
void type()
{
    skipSpaces();

    if(strncmp(&input[i], "int", 3) == 0)
        i += 3;
    else if(strncmp(&input[i], "float", 5) == 0)
        i += 5;
    else if(strncmp(&input[i], "char", 4) == 0)
        i += 4;
    else
    {
        printf("Invalid type\n");
        exit(0);
    }
}

/* declaration -> type id_list ; */
void declaration()
{
    type();
    id_list();

    skipSpaces();

    if(input[i] == ';')
        i++;
    else
    {
        printf("Missing semicolon\n");
        exit(0);
    }
}

int main()
{
    printf("Enter declaration:\n");
    fgets(input, sizeof(input), stdin);

    declaration();

    skipSpaces();

    if(input[i] == '\0' || input[i] == '\n')
        printf("Parsing Successful\n");
    else
        printf("Parsing Failed\n");

    return 0;
}