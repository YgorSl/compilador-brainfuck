// bfexec.c — Executor Brainfuck com suporte a UTF-8 no Windows
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
  #include <windows.h>
#endif

#define TAPE_SIZE   30000
#define PROG_SIZE  131072

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s arquivo.bf\n", argv[0]);
        return 1;
    }

  #ifdef _WIN32
    // Ativa saída UTF-8 no console do Windows
    SetConsoleOutputCP(CP_UTF8);
  #endif

    // Carrega o programa Brainfuck
    FILE *f = fopen(argv[1], "r");
    if (!f) { perror("Erro ao abrir .bf"); return 1; }
    char *program = malloc(PROG_SIZE);
    size_t prog_len = fread(program, 1, PROG_SIZE-1, f);
    program[prog_len] = '\0';
    fclose(f);

    // Prepara fita e ponteiros
    unsigned char *tape = calloc(TAPE_SIZE, 1);
    size_t pc = 0, dp = 0;
    size_t *stack = malloc(PROG_SIZE * sizeof(size_t));
    size_t sp = 0;

    while (pc < prog_len) {
        switch (program[pc]) {
            case '>': dp = (dp+1)%TAPE_SIZE; break;
            case '<': dp = (dp==0? TAPE_SIZE-1: dp-1); break;
            case '+': tape[dp]++; break;
            case '-': tape[dp]--; break;
            case '.': 
                // imprime raw byte — isto preserva UTF-8
                putchar(tape[dp]);
                break;
            case ',': {
                int c = getchar();
                tape[dp] = (c==EOF? 0: (unsigned char)c);
                break;
            }
            case '[':
                if (tape[dp]) {
                    stack[sp++] = pc;
                } else {
                    int nest = 1;
                    while (nest && ++pc < prog_len) {
                        if (program[pc]=='[') nest++;
                        else if (program[pc]==']') nest--;
                    }
                }
                break;
            case ']':
                if (sp==0) {
                    fprintf(stderr,"']' sem '[' correspondente em pc=%zu\n", pc);
                    goto cleanup;
                }
                if (tape[dp]) {
                    pc = stack[sp-1];
                } else {
                    sp--;
                }
                break;
            default:
                // ignora outros caracteres
                break;
        }
        pc++;
    }

cleanup:
    free(program);
    free(tape);
    free(stack);
    return 0;
}
