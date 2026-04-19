#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

struct Symbol {
    char name[20];
    char type[10];
};

struct Symbol table[100];
int count = 0;

/* Check duplicate declaration */
int exists(char name[]) {
    for (int i = 0; i < count; i++) {
        if (strcmp(table[i].name, name) == 0)
            return 1;
    }
    return 0;
}

/* Insert into symbol table */
void insert(char name[], char type[]) {
    strcpy(table[count].name, name);
    strcpy(table[count].type, type);
    count++;
}

/* Check integer */
int isInt(char s[]) {
    for (int i = 0; s[i]; i++) {
        if (!isdigit(s[i]))
            return 0;
    }
    return 1;
}

/* Check float */
int isFloat(char s[]) {
    int dot = 0;
    for (int i = 0; s[i]; i++) {
        if (s[i] == '.')
            dot++;
        else if (!isdigit(s[i]))
            return 0;
    }
    return (dot == 1);
}

/* Check char constant */
int isChar(char s[]) {
    return (strlen(s) == 3 && s[0] == '\'' && s[2] == '\'');
}

/* Type checking */
int checkValue(char type[], char value[]) {
    if (strcmp(type, "int") == 0) {
        if (isInt(value))
            return 1;
    }

    else if (strcmp(type, "float") == 0) {
        if (isInt(value) || isFloat(value))
            return 1;
    }

    else if (strcmp(type, "char") == 0) {
        if (isChar(value))
            return 1;
    }

    return 0;
}

int main() {
    char dtype[10];
    char input[200];
    char *token;

    printf("Enter declaration: ");
    fgets(input, sizeof(input), stdin);

    token = strtok(input, " ,=;\n");

    if (token == NULL) {
        printf("Invalid Input\n");
        return 0;
    }

    strcpy(dtype, token);

    if (strcmp(dtype, "int") != 0 &&
        strcmp(dtype, "float") != 0 &&
        strcmp(dtype, "char") != 0) {
        printf("Invalid Data Type\n");
        return 0;
    }

    while ((token = strtok(NULL, " ,=;\n")) != NULL) {
        char id[20];
        strcpy(id, token);

        if (exists(id)) {
            printf("Semantic Error: Duplicate variable '%s'\n", id);
            return 0;
        }

        insert(id, dtype);

        char *next = strtok(NULL, " ,=;\n");

        if (next != NULL) {
            if (checkValue(dtype, next)) {
                continue;
            } else {
                printf("Type Error: Cannot assign '%s' to %s\n", next, dtype);
                return 0;
            }
        }
    }

    printf("Valid Declaration\n");

    printf("\nSymbol Table:\n");
    printf("Name\tType\n");

    for (int i = 0; i < count; i++) {
        printf("%s\t%s\n", table[i].name, table[i].type);
    }

    return 0;
}