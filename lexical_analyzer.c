#include <stdio.h>
#include <ctype.h>
#include <string.h>

int isKeyword(char *word) {
    if(strcmp(word, "int") == 0 ||
       strcmp(word, "float") == 0 ||
       strcmp(word, "char") == 0)
        return 1;
    return 0;
}

int main() {
    char input[100];
    int i = 0;
    int valid = 1;

    printf("Enter declaration: ");
    fgets(input, sizeof(input), stdin);

    while(input[i] != '\0' && input[i] != '\n') {

        if(isspace(input[i])) {
            i++;
            continue;
        }

        if(isalpha(input[i])) {
            char buffer[50];
            int j = 0;

            while(isalnum(input[i])) {
                buffer[j++] = input[i++];
            }
            buffer[j] = '\0';

            if(isKeyword(buffer)) {
                printf("KEYWORD: %s\n", buffer);
            }
            else {
                printf("IDENTIFIER: %s\n", buffer);
            }
        }

        // Number
        else if(isdigit(input[i])) {
            char buffer[50];
            int j = 0;

            while(isdigit(input[i])) {
                buffer[j++] = input[i++];
            }
            buffer[j] = '\0';

            printf("NUMBER: %s\n", buffer);
        }

        // Operator
        else if(input[i] == '=') {
            printf("OPERATOR: =\n");
            i++;
        }

        // Separator
        else if(input[i] == ',' || input[i] == ';') {
            printf("SEPARATOR: %c\n", input[i]);
            i++;
        }

        // Invalid character
        else {
            valid = 0;
            break;
        }
    }

    if(valid == 0) {
        printf("\nInvalid input value error\n");
    }

    return 0;
}