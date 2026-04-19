#include <stdio.h>
#include <string.h>
#include <ctype.h>

char tokens[100][20];
int pos = 0, total = 0;

/* Function declarations */
int declaration();
int type();
int id_list();
int id_tail();
int expr();

/* Match expected token */
int match(char expected[]) {
    if (pos < total && strcmp(tokens[pos], expected) == 0) {
        pos++;
        return 1;
    }
    return 0;
}

/* Check identifier */
int isIdentifier(char s[]) {
    if (strcmp(s, "int") == 0 || strcmp(s, "float") == 0 ||
        strcmp(s, "char") == 0 || strcmp(s, ",") == 0 ||
        strcmp(s, "=") == 0 || strcmp(s, ";") == 0)
        return 0;
    return 1;
}

/* Check number */
int isNumber(char s[]) {
    int i;
    for (i = 0; s[i] != '\0'; i++) {
        if (!isdigit(s[i]))
            return 0;
    }
    return 1;
}

/* <declaration> ::= <type> <id_list> ; */
int declaration() {
    if (type()) {
        if (id_list()) {
            if (match(";"))
                return 1;
        }
    }
    return 0;
}

/* <type> ::= int | float | char */
int type() {
    if (match("int") || match("float") || match("char"))
        return 1;
    return 0;
}

/* <id_list> ::= id <id_tail> */
int id_list() {
    if (pos < total && isIdentifier(tokens[pos])) {
        pos++;
        return id_tail();
    }
    return 0;
}

/* <id_tail> ::= , id <id_tail>
                 | = <expr> <id_tail>
                 | ε */
int id_tail() {
    if (match(",")) {
        if (pos < total && isIdentifier(tokens[pos])) {
            pos++;
            return id_tail();
        }
        return 0;
    }
    else if (match("=")) {
        if (expr())
            return id_tail();
        return 0;
    }
    return 1; /* epsilon */
}

/* <expr> ::= id | num */
int expr() {
    if (pos < total &&
       (isIdentifier(tokens[pos]) || isNumber(tokens[pos]))) {
        pos++;
        return 1;
    }
    return 0;
}

/* Main function */
int main() {
    char input[200];
    char temp[20];
    int i, j = 0;

    printf("Enter declaration: ");
    fgets(input, sizeof(input), stdin);

    /* Tokenization */
    for (i = 0; input[i] != '\0'; i++) {
        if (input[i] == ' ' || input[i] == ',' ||
            input[i] == '=' || input[i] == ';' ||
            input[i] == '\n') {

            if (j > 0) {
                temp[j] = '\0';
                strcpy(tokens[total++], temp);
                j = 0;
            }

            if (input[i] == ',' || input[i] == '=' || input[i] == ';') {
                tokens[total][0] = input[i];
                tokens[total][1] = '\0';
                total++;
            }
        }
        else {
            temp[j++] = input[i];
        }
    }

    /* Parsing */
    if (declaration() && pos == total)
        printf("Valid Declaration\n");
    else
        printf("Syntax Error\n");

    return 0;
}