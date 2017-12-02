#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INIT_SIZE 32

enum { Z = 1, S, T, J };

typedef struct code_line { unsigned long instruction[4]; } CODELINE;

typedef struct _register_ {
  int *data;
  size_t size;
} REGISTER;

REGISTER *expand_register(REGISTER *reg) {
  size_t end = reg->size;
  reg->size += INIT_SIZE;
  reg->data = realloc(reg->data, reg->size * sizeof(int));
  memset(reg->data + end, 0, sizeof(int) * (reg->size - end));
  return reg;
}

unsigned int execute_instruction(unsigned int cnt, CODELINE *pgm,
                                 REGISTER **p) {
  unsigned int opc = pgm->instruction[0], op1 = pgm->instruction[1],
               op2 = pgm->instruction[2], op3 = pgm->instruction[3];

  REGISTER *reg = *p;
  switch (opc) {
  case Z:
    if (op1 > reg->size)
      *p = reg = expand_register(reg);
    reg->data[op1] = 0;
    cnt++;
    break;
  case S:
    if (op1 > reg->size)
      *p = reg = expand_register(reg);
    reg->data[op1]++;
    cnt++;
    break;
  case T:
    if (op1 > reg->size || op2 > reg->size)
      *p = reg = expand_register(reg);
    reg->data[op1] = reg->data[op2];
    cnt++;
    break;
  case J:
    if (op1 > reg->size || op2 > reg->size)
      *p = reg = expand_register(reg);
    if (reg->data[op1] == reg->data[op2])
      cnt = reg->data[op3];
    else
      cnt++;
    break;
  DEFAULT:
    cnt++;
    fprintf(stderr, "illegal instruction\n");
  }
  return cnt;
}

void read_instruction(char *code, CODELINE *pgm) {
  char *nxt;
  unsigned int opc = strtoul(code, &nxt, 0);
  switch (opc) {
  case Z:
    pgm->instruction[1] = strtoul(nxt, 0, 0);
    break;
  case S:
    pgm->instruction[1] = strtoul(nxt, 0, 0);
    break;
  case T:
    pgm->instruction[1] = strtoul(nxt, &nxt, 0);
    pgm->instruction[2] = strtoul(nxt, 0, 0);
    break;
  case J:
    pgm->instruction[1] = strtoul(nxt, &nxt, 0);
    pgm->instruction[2] = strtoul(nxt, &nxt, 0);
    pgm->instruction[3] = strtoul(nxt, 0, 0);
    break;
  DEFAULT:
    fprintf(stderr, "illegal instruction\n");
  }
}

int main(int argc, const char **argv) {

  char *line = (char *)calloc(sizeof(char), INIT_SIZE);
  size_t cap = INIT_SIZE;
  ssize_t len;
  CODELINE *pgm = (CODELINE *)calloc(sizeof(CODELINE), INIT_SIZE);
  size_t pgm_size = 0, max_pgm_size = INIT_SIZE, pgm_cnt;
  REGISTER *reg = (REGISTER *)calloc(sizeof(REGISTER), INIT_SIZE);
  FILE *fp;

  if (argc > 1)
    fp = fopen(argv[1], "r");
  else
    fp = stdin;

  if ((len = getline(&line, &cap, fp)) > 0) {
    if (pgm_size > max_pgm_size) {
      max_pgm_size += INIT_SIZE;
      pgm = (CODELINE *)realloc(pgm, max_pgm_size * sizeof(CODELINE));
    }
    read_instruction(line, &pgm[pgm_size++]);
  }

  while (pgm_cnt < pgm_size)
    pgm_cnt = execute_instruction(pgm_cnt, pgm, &reg);

  printf("%d", reg->data[0]);

  free(reg);
  free(pgm);
  free(line);

  return 0;
}
