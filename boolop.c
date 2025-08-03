#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void error(const char *msg) {
    fprintf(stderr, "Fatal error: %s\n", msg);
    exit(1);
}

const char *s;            // source ptr
int varnames[26], nvars;  // used variables
unsigned char expr[1000]; // compiled expression
unsigned char *wp = expr; // write pointer into expr
int varvalues[26];        // variable values during evaluation

void skipsp() {
    while (*s && strchr(" \r\t\n", *s)) s++;
}

#define MAXLEVEL 4

void compile_expr(int level) {
    skipsp();
    if (level == 0) {
        if (*s >= 'A' && *s <= 'Z') {
            int i = 0;
            while (i < nvars && varnames[i] != *s) i++;
            if (i == nvars) {
                varnames[nvars++] = *s;
            }
            s++;
            *wp++ = i;
            if (wp == expr+sizeof(expr)) error("Expression too complex");
        } else if (*s == '(') {
            s++;
            compile_expr(MAXLEVEL);
            skipsp();
            if (*s != ')') error("')' expected");
            s++;
        } else if (*s == '!') {
            s++; compile_expr(0);
            *wp++ = '!'; if (wp == expr+sizeof(expr)) error("Expression too complex");
        } else if (*s == 't' || *s == 'f') {
            *wp++ = *s++; if (wp == expr+sizeof(expr)) error("Expression too complex");
        } else error("Syntax error: expression expected");
    } else {
        compile_expr(level-1);
        for (;;) {
            skipsp();
            if (*s == '&' && level == 1) {
                s++;
                compile_expr(level-1);
                *wp++ = '&'; if (wp == expr+sizeof(expr)) error("Expression too complex");
            } else if (*s == '|' && level == 2) {
                s++;
                compile_expr(level-1);
                *wp++ = '|'; if (wp == expr+sizeof(expr)) error("Expression too complex");
            } else if (*s == '=' && s[1] == '>' && level == 3) {
                s+=2;
                compile_expr(level-1);
                *wp++ = '>'; if (wp == expr+sizeof(expr)) error("Expression too complex");
            } else if (*s == '<' && s[1] == '=' && s[2] == '>' && level == 4) {
                s+=3;
                compile_expr(level-1);
                *wp++ = '='; if (wp == expr+sizeof(expr)) error("Expression too complex");
            } else break;
        }
    }
}

int ev() {
    int stack[1000], *sp = stack;
    unsigned char *ip = expr;
    while (ip < wp) {
        int op = *ip++;
        if (op >= 0 && op < nvars) { *sp++ = varvalues[op]; }
        else if (op == 't' || op == 'f') { *sp++ = (op == 't'); }
        else if (op == '!') { sp[-1] = !sp[-1]; }
        else if (op == '&') { sp[-2] = (sp[-2] && sp[-1]); --sp; }
        else if (op == '|') { sp[-2] = (sp[-2] || sp[-1]); --sp; }
        else if (op == '>') { sp[-2] = (!sp[-2] || sp[-1]); --sp; }
        else if (op == '=') { sp[-2] = (sp[-2] == sp[-1]); --sp; }
        else error("Internal error: unknown opcode");
    }
    return sp[-1];
}

void print_table(int i, int explen) {
    if (i == nvars) {
        for (int j=0; j<nvars; j++) {
            printf("| %c ", varvalues[j] ? 't' : 'f');
        }
        printf("| %*s%c%*s |\n", (explen-1)/2, "", ev() ? 't' : 'f', (explen-1)-(explen-1)/2, "");
    } else {
        varvalues[i] = 0; print_table(i+1, explen);
        varvalues[i] = 1; print_table(i+1, explen);
    }
}

int main(int argc, const char *argv[]) {
    if (argc != 2) error("Expression expected");
    s = argv[1];
    int explen = strlen(s);
    compile_expr(MAXLEVEL);
    skipsp();
    if (*s) error("Extra characters at end of expression");
    for (int j=0; j<nvars; j++) {
        printf("+---");
    }
    putchar('+');
    for (int j=0; j<explen+2; j++) putchar('-');
    printf("+\n");
    for (int j=0; j<nvars; j++) {
        printf("| %c ", varnames[j]);
    }
    printf("| %s |\n", argv[1]);
    for (int j=0; j<nvars; j++) {
        printf("+---");
    }
    putchar('+');
    for (int j=0; j<explen+2; j++) putchar('-');
    printf("+\n");
    print_table(0, explen);
    for (int j=0; j<nvars; j++) {
        printf("+---");
    }
    putchar('+');
    for (int j=0; j<explen+2; j++) putchar('-');
    printf("+\n");
}