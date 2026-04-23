#include <stdio.h>
#include <string.h>

#define MAX 50

struct Code {
    char lhs[20];
    char rhs[20];
};

struct Code code[MAX];
int n;

/* Generate Target Code */
void generateTargetCode() {
    int i;
    int reg = 1;

    printf("\nTarget Code:\n");

    for (i = 0; i < n; i++) {
        printf("MOV R%d, %s\n", reg, code[i].rhs);
        printf("MOV %s, R%d\n", code[i].lhs, reg);
        reg++;

        if (reg > 4)
            reg = 1;   /* reuse registers */
    }
}

int main() {
    int i;

    printf("Enter number of statements: ");
    scanf("%d", &n);

    for (i = 0; i < n; i++) {
        printf("Enter lhs rhs for statement %d: ", i + 1);
        scanf("%s %s", code[i].lhs, code[i].rhs);
    }

    generateTargetCode();

    return 0;
}