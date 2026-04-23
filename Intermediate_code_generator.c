/* ---------------------------------------------------------
   Phase 4 : Intermediate Code Generation (TAC)
   Supports BOTH:
   1) Assignment Input : a=b+c*d;
   2) Expression Only  : 7-6*5

   Examples:
   Input : a=b+c*d;
   Output:
       t1 = c * d
       t2 = b + t1
       a = t2

   Input : 7-6*5
   Output:
       t1 = 6 * 5
       t2 = 7 - t1
       Final Result = t2
--------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX 100

char operandStack[MAX][20];
char operatorStack[MAX];
int topOperand = -1, topOperator = -1;
int tempCount = 1;

/* ---------- Stack Functions ---------- */

void resetStacks() {
    topOperand = -1;
    topOperator = -1;
}

void pushOperand(char val[]) {
    strcpy(operandStack[++topOperand], val);
}

char* popOperand() {
    return operandStack[topOperand--];
}

void pushOperator(char op) {
    operatorStack[++topOperator] = op;
}

char popOperator() {
    return operatorStack[topOperator--];
}

/* ---------- Utility Functions ---------- */

int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

int isOperator(char ch) {
    return (ch == '+' || ch == '-' || ch == '*' || ch == '/');
}

char* newTemp() {
    static char temp[10];
    sprintf(temp, "t%d", tempCount++);
    return temp;
}

/* ---------- Generate TAC ---------- */

void generateCode() {
    char op = popOperator();

    char right[20], left[20];
    strcpy(right, popOperand());
    strcpy(left, popOperand());

    char temp[10];
    strcpy(temp, newTemp());

    printf("%s = %s %c %s\n", temp, left, op, right);

    pushOperand(temp);
}

/* ---------- Process Expression ---------- */

void processExpression(char expr[], char lhs[]) {
    int i = 0, j;

    resetStacks();

    while (expr[i] != '\0') {

        /* Operand */
        if (isalnum(expr[i])) {
            char operand[20];
            j = 0;

            while (isalnum(expr[i])) {
                operand[j++] = expr[i++];
            }
            operand[j] = '\0';

            pushOperand(operand);
            continue;
        }

        /* Operator */
        else if (isOperator(expr[i])) {
            while (topOperator >= 0 &&
                   precedence(operatorStack[topOperator]) >= precedence(expr[i])) {
                generateCode();
            }

            pushOperator(expr[i]);
        }

        i++;
    }

    /* Remaining operators */
    while (topOperator >= 0)
        generateCode();

    /* Final Result */
    if (strlen(lhs) > 0)
        printf("%s = %s\n", lhs, popOperand());
    else
        printf("Final Result = %s\n", popOperand());
}

/* ---------- Main ---------- */

int main() {
    char input[100], lhs[20] = "", expr[100] = "";
    int i, j = 0, hasAssignment = 0;

    printf("Enter Input: ");
    fgets(input, sizeof(input), stdin);

    /* Check if assignment exists */
    for (i = 0; input[i] != '\0'; i++) {
        if (input[i] == '=') {
            hasAssignment = 1;
            break;
        }
    }

    if (hasAssignment) {
        /* Extract LHS */
        i = 0;
        while (input[i] != '=') {
            if (input[i] != ' ')
                lhs[j++] = input[i];
            i++;
        }
        lhs[j] = '\0';

        i++; /* skip '=' */

        /* Extract RHS Expression */
        j = 0;
        while (input[i] != ';' && input[i] != '\0' && input[i] != '\n') {
            if (input[i] != ' ')
                expr[j++] = input[i];
            i++;
        }
        expr[j] = '\0';
    }
    else {
        /* Whole input is expression */
        j = 0;
        for (i = 0; input[i] != '\0' && input[i] != '\n'; i++) {
            if (input[i] != ' ')
                expr[j++] = input[i];
        }
        expr[j] = '\0';
    }

    printf("\nThree Address Code:\n");
    processExpression(expr, lhs);

    return 0;
}