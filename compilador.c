#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/*
 * Parser simples para +, -, *, / e parênteses
 */
const char *p;
int expr(), term(), factor();

int expr() {
    int v = term();
    while (*p=='+'||*p=='-') {
        char op = *p++;
        int v2 = term();
        v = (op=='+') ? v+v2 : v-v2;
    }
    return v;
}
int term() {
    int v = factor();
    while (*p=='*'||*p=='/') {
        char op = *p++;
        int v2 = factor();
        v = (op=='*') ? v*v2 : v/v2;
    }
    return v;
}
int factor() {
    if (*p=='(') {
        p++;
        int v = expr();
        if (*p==')') p++;
        return v;
    }
    if (!isdigit(*p)) {
        fprintf(stderr,"Erro de sintaxe em \"%s\"\n", p);
        exit(1);
    }
    int v = 0;
    while (isdigit(*p)) {
        v = v*10 + (*p - '0');
        p++;
    }
    return v;
}

// Gera Brainfuck para imprimir um byte (0–255) na célula `target_cell`.
void bf_print_byte(FILE *out, int *cur_cell, int target_cell, unsigned char byte) {
    // move
    int delta = target_cell - *cur_cell;
    if (delta>0) for(int i=0;i<delta;i++) fputc('>', out);
    else        for(int i=0;i<-delta;i++) fputc('<', out);
    *cur_cell = target_cell;
    // clear
    fputs("[-]", out);
    // set
    for(int i=0;i<byte;i++) fputc('+', out);
    // print
    fputc('.', out);
}

// Imprime string raw UTF-8, byte a byte
void bf_print_string(FILE *out, int *cur_cell, const char *s) {
    int cell = 0;
    for (size_t i = 0; i < strlen(s); ++i) {
        unsigned char c = (unsigned char)s[i];
        bf_print_byte(out, cur_cell, cell++, c);
    }
}

int main(int argc, char *argv[]) {
    if (argc!=2) {
        fprintf(stderr,"Uso: %s nome=\"expr\"\n", argv[0]);
        return 1;
    }
    // separa nome e expressão
    char *eq = strchr(argv[1], '=');
    if (!eq) {
        fprintf(stderr,"Formato inválido: nome=\"expr\"\n");
        return 1;
    }
    char varname[128];
    int n = eq - argv[1];
    if (n >= (int)sizeof(varname)) n = sizeof(varname)-1;
    strncpy(varname, argv[1], n);
    varname[n] = '\0';

    // expr
    char *expr_start = eq+1;
    // retira aspas se houver
    if (*expr_start=='"') {
        expr_start++;
        char *endq = strrchr(expr_start,'"');
        if (endq) *endq = '\0';
    }

    // avalia
    p = expr_start;
    int result = expr();

    // monta output text
    char output[256];
    snprintf(output, sizeof(output), "%s=%d", varname, result);

    // gera Brainfuck
    FILE *out = fopen("saida.bf","w");
    if (!out) { perror("saida.bf"); return 1; }

    int cur_cell = 0;
    bf_print_string(out, &cur_cell, output);
    fputc('\n', out);

    fclose(out);
    printf("Gerado Brainfuck em saida.bf  — ao executar, imprimirá: %s\n", output);
    return 0;
}
