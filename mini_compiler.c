/*
 * Mini Compiler — supports grammar:
 *
 * <declaration> ::= <type> <id_list> ";"
 * <type>        ::= "int" | "float" | "char"
 * <id_list>     ::= id <id_tail>
 * <id_tail>     ::= "," id <id_tail>
 *                 | "=" <expr> <id_tail>
 *                 | ε
 * <expr>        ::= id | num | char_literal
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/* ── Limits ──────────────────────────────────────────────────────────── */
#define MAXTOK 200
#define MAXSYM 100
#define MAXCODE 200

/* ── Data structures ─────────────────────────────────────────────────── */
typedef struct
{
    char lex[50];
    char type[20];
} Token;
typedef struct
{
    char name[30];
    char dtype[10];
} Symbol;
typedef struct
{
    char lhs[20];
    char rhs[100];
} Code;

/* ── Pending entry: filled by parser, consumed by semantic phase ─────── */
typedef struct
{
    char name[30];
    char val[50];
    int hasVal;
} Pending;

/* ── Globals ─────────────────────────────────────────────────────────── */
Token tokens[MAXTOK];
int total = 0, pos = 0;
Symbol sym[MAXSYM];
int sc = 0;
Code code[MAXCODE];
int cc = 0;
char currentType[10];
Pending pending[MAXSYM];
int pc = 0; /* collected by parser, checked later */

/* ════════════════════════════════════════════════════════════════════════
 * PHASE 1 — LEXICAL ANALYSIS
 * ════════════════════════════════════════════════════════════════════════ */

int isKeyword(char *s)
{
    return !strcmp(s, "int") || !strcmp(s, "float") || !strcmp(s, "char");
}

void addTok(char *lex, char *type)
{
    strcpy(tokens[total].lex, lex);
    strcpy(tokens[total].type, type);
    total++;
}

void lexer(char in[])
{
    int i = 0;
    total = 0;

    while (in[i] && in[i] != '\n')
    {

        /* skip whitespace */
        if (isspace(in[i]))
        {
            i++;
            continue;
        }

        /* identifier or keyword */
        if (isalpha(in[i]) || in[i] == '_')
        {
            char buf[50];
            int j = 0;
            while (isalnum(in[i]) || in[i] == '_')
                buf[j++] = in[i++];
            buf[j] = '\0';
            addTok(buf, isKeyword(buf) ? "KEYWORD" : "ID");
            continue;
        }

        /* numeric literal */
        if (isdigit(in[i]))
        {
            char buf[50];
            int j = 0;
            while (isdigit(in[i]) || in[i] == '.')
                buf[j++] = in[i++];
            buf[j] = '\0';
            addTok(buf, "NUM");
            continue;
        }

        /* character literal  'x' */
        if (in[i] == '\'' && in[i + 2] == '\'')
        {
            char buf[4] = {in[i], in[i + 1], in[i + 2], '\0'};
            addTok(buf, "CHAR");
            i += 3;
            continue;
        }

        /* single-character symbols */
        {
            char buf[2] = {in[i], '\0'};
            if (strchr(",;", in[i]))
                addTok(buf, "SYM");
            else if (in[i] == '=')
                addTok(buf, "ASSIGN OP");
            else
                addTok(buf, "INVALID");
            i++;
        }
    }
}

/* ════════════════════════════════════════════════════════════════════════
 * PHASE 2 — SYNTAX ANALYSIS  (recursive-descent parser)
 * ════════════════════════════════════════════════════════════════════════ */

/* Try to consume token with lexeme s; return 1 on success. */
int match(char *s)
{
    if (pos < total && !strcmp(tokens[pos].lex, s))
    {
        pos++;
        return 1;
    }
    return 0;
}

int isIdTok(void)
{
    return pos < total && !strcmp(tokens[pos].type, "ID");
}

/* ════════════════════════════════════════════════════════════════════════
 * SEMANTIC HELPERS  (used by Phase 3)
 * ════════════════════════════════════════════════════════════════════════ */

int exists(char *name)
{
    for (int i = 0; i < sc; i++)
        if (!strcmp(sym[i].name, name))
            return 1;
    return 0;
}

void insertSym(char *name, char *type)
{
    strcpy(sym[sc].name, name);
    strcpy(sym[sc].dtype, type);
    sc++;
}

int isIntLit(char *s)
{
    for (int i = 0; s[i]; i++)
        if (!isdigit(s[i]))
            return 0;
    return 1;
}

int isFloatLit(char *s)
{
    int dots = 0;
    for (int i = 0; s[i]; i++)
    {
        if (s[i] == '.')
            dots++;
        else if (!isdigit(s[i]))
            return 0;
    }
    return dots == 1;
}

int isCharLit(char *s)
{
    return strlen(s) == 3 && s[0] == '\'' && s[2] == '\'';
}

/* Returns 1 if value v is type-compatible with declared type t. */
int typeOk(char *t, char *v)
{
    if (!strcmp(t, "int"))
        return isIntLit(v);
    if (!strcmp(t, "float"))
        return isIntLit(v) || isFloatLit(v);
    if (!strcmp(t, "char"))
        return isCharLit(v);
    return 0;
}

/* forward declaration — emit() is defined in Phase 4 */
void emit(char *lhs, char *rhs);

/* ════════════════════════════════════════════════════════════════════════
 * PHASE 3 — SEMANTIC ANALYSIS
 * Runs after syntax succeeds; walks the pending[] list built by the parser.
 * ════════════════════════════════════════════════════════════════════════ */

/*
 * Returns 1 if all pending entries pass duplicate + type checks,
 * and populates the symbol table on success.
 */
int semantic(void)
{
    int ok = 1;

    for (int i = 0; i < pc; i++)
    {
        char *name = pending[i].name;

        /* duplicate variable check */
        if (exists(name))
        {
            printf("Semantic Error: Duplicate variable '%s'\n", name);
            ok = 0;
            continue; /* keep checking remaining variables */
        }
        insertSym(name, currentType);

        /* type compatibility check (only for initialised variables) */
        if (pending[i].hasVal)
        {
            if (!typeOk(currentType, pending[i].val))
            {
                printf("Type Error: cannot assign '%s' to '%s' variable '%s'\n",
                       pending[i].val, currentType, name);
                ok = 0;
            }
            else
            {
                emit(name, pending[i].val);
            }
        }
    }

    return ok;
}

/* ════════════════════════════════════════════════════════════════════════
 * PHASE 4 — INTERMEDIATE CODE GENERATION
 * ════════════════════════════════════════════════════════════════════════ */

void emit(char *lhs, char *rhs)
{
    strcpy(code[cc].lhs, lhs);
    strcpy(code[cc].rhs, rhs);
    cc++;
}

/* <expr> ::= id | num | char_literal */
int expr(char *out)
{
    if (pos < total &&
        (!strcmp(tokens[pos].type, "ID") ||
         !strcmp(tokens[pos].type, "NUM") ||
         !strcmp(tokens[pos].type, "CHAR")))
    {
        strcpy(out, tokens[pos++].lex);
        return 1;
    }
    return 0;
}

/* Forward declaration */
int id_tail(void);

/*
 * <id_list> ::= id <id_tail>
 *
 * The parser ONLY checks structure and records (name, value) pairs.
 * No semantic work (duplicate check, type check) is done here.
 */
int id_list(void)
{
    if (!isIdTok())
        return 0;

    /* record identifier */
    Pending *p = &pending[pc++];
    strcpy(p->name, tokens[pos++].lex);
    p->hasVal = 0;
    p->val[0] = '\0';

    /* optional initialiser — structural check only */
    if (match("="))
    {
        if (!expr(p->val))
        {
            printf("Syntax Error: expected expression after '='\n");
            return 0;
        }
        p->hasVal = 1;
    }

    return id_tail();
}

/*
 * <id_tail> ::= "," <id_list>   |   ε
 */
int id_tail(void)
{
    if (match(","))
        return id_list();
    return 1; /* ε */
}

/*
 * <declaration> ::= <type> <id_list> ";"
 */
int declaration(void)
{
    if (pos >= total || strcmp(tokens[pos].type, "KEYWORD") != 0)
        return 0;

    strcpy(currentType, tokens[pos++].lex);

    if (!id_list())
        return 0;
    if (!match(";"))
    {
        printf("Syntax Error: expected ';'\n");
        return 0;
    }
    if (pos != total)
    {
        printf("Syntax Error: unexpected tokens after ';'\n");
        return 0;
    }

    return 1;
}

/* ════════════════════════════════════════════════════════════════════════
 * PHASE 5 — CODE OPTIMISATION  (copy propagation)
 * ════════════════════════════════════════════════════════════════════════ */

void optimize(void)
{
    for (int i = 0; i < cc; i++)
        for (int j = i + 1; j < cc; j++)
            if (!strcmp(code[j].rhs, code[i].lhs))
                strcpy(code[j].rhs, code[i].rhs);
}

/* ════════════════════════════════════════════════════════════════════════
 * PHASE 6 — TARGET CODE GENERATION
 * ════════════════════════════════════════════════════════════════════════ */

void target(void)
{
    int r = 1;
    printf("\nPhase 6: Target Code Generation\n");
    printf("--------------------------------\n");
    for (int i = 0; i < cc; i++)
    {
        printf("MOV R%d, %s\n", r, code[i].rhs);
        printf("MOV %s, R%d\n", code[i].lhs, r);
        r = r % 4 + 1;
    }
}

/* ════════════════════════════════════════════════════════════════════════
 * MAIN
 * ════════════════════════════════════════════════════════════════════════ */

int main(void)
{
    char input[300];

    printf("Enter declaration: ");
    fgets(input, sizeof(input), stdin);

    /* ── Phase 1: Lexical Analysis ─────────────────────────────────── */
    lexer(input);

    printf("\nPhase 1: Lexical Analysis\n");
    printf("-------------------------\n");
    for (int i = 0; i < total; i++)
    {
        if (!strcmp(tokens[i].type, "INVALID"))
        {
            printf("Lexical Error: invalid token '%s'\n", tokens[i].lex);
            return 1;
        }
        printf("%-15s -> %s\n", tokens[i].lex, tokens[i].type);
    }

    /* ── Phase 2: Syntax Analysis ──────────────────────────────────── */
    pos = 0;
    printf("\nPhase 2: Syntax Analysis\n");
    printf("------------------------\n");
    if (!declaration())
    {
        printf("Syntax Error: invalid declaration\n");
        return 1;
    }
    printf("Valid Declaration\n");

    /* ── Phase 3: Semantic Analysis ────────────────────────────────── */
    printf("\nPhase 3: Semantic Analysis\n");
    printf("--------------------------\n");
    if (!semantic())
    {
        printf("Semantic Analysis failed\n");
        return 1;
    }
    printf("No Semantic Errors\n\n");
    printf("Symbol Table:\n");
    printf("%-20s %s\n", "Name", "Type");
    printf("%-20s %s\n", "----", "----");
    for (int i = 0; i < sc; i++)
        printf("%-20s %s\n", sym[i].name, sym[i].dtype);

    /* ── Phase 4: Intermediate Code ────────────────────────────────── */
    printf("\nPhase 4: Intermediate Code Generation\n");
    printf("-------------------------------------\n");
    if (cc == 0)
    {
        printf("(no assignments — nothing to generate)\n");
    }
    else
    {
        for (int i = 0; i < cc; i++)
            printf("%s = %s\n", code[i].lhs, code[i].rhs);
    }

    /* ── Phase 5: Optimisation ─────────────────────────────────────── */
    optimize();
    printf("\nPhase 5: Code Optimisation\n");
    printf("--------------------------\n");
    if (cc == 0)
    {
        printf("(nothing to optimise)\n");
    }
    else
    {
        for (int i = 0; i < cc; i++)
            printf("%s = %s\n", code[i].lhs, code[i].rhs);
    }

    /* ── Phase 6: Target Code ──────────────────────────────────────── */
    target();

    return 0;
}