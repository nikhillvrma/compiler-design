#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX 50

struct Code {
    char lhs[20];
    char rhs[20];
};

struct Code code[MAX];
int n = 0;

/* Check if number */
int isNumber(char str[]) {
    int i, dot = 0;
    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] == '.')
            dot++;
        else if (!isdigit(str[i]))
            return 0;
    }
    return 1;
}

/* Print Code */
void printCode() {
    int i;
    for (i = 0; i < n; i++) {
        printf("%s = %s\n", code[i].lhs, code[i].rhs);
    }
}

/* Constant / Copy Propagation */
void propagate() {
    int i, j;

    for (i = 0; i < n; i++) {
        for (j = i + 1; j < n; j++) {

            if (strcmp(code[j].rhs, code[i].lhs) == 0) {
                strcpy(code[j].rhs, code[i].rhs);
            }
        }
    }
}

/* Remove Duplicate Assignments */
void removeRedundant() {
    int i, j, k;

    for (i = 0; i < n - 1; i++) {
        if (strcmp(code[i].lhs, code[i + 1].lhs) == 0 &&
            strcmp(code[i].rhs, code[i + 1].rhs) == 0) {

            for (k = i + 1; k < n - 1; k++)
                code[k] = code[k + 1];

            n--;
            i--;
        }
    }
}

int main() {
    int i;

    printf("Enter number of TAC statements: ");
    scanf("%d", &n);

    for (i = 0; i < n; i++) {
        printf("Enter lhs rhs for statement %d: ", i + 1);
        scanf("%s %s", code[i].lhs, code[i].rhs);
    }

    printf("\nOriginal Code:\n");
    printCode();

    propagate();
    removeRedundant();

    printf("\nOptimized Code:\n");
    printCode();

    return 0;
}