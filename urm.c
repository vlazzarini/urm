/******************************************************************************
 * Unlimited register machine interpreter
 * 
 * Victor Lazzarini, 2017
 *
 ******************************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INIT_SIZE 32

enum { Z = 1, S, T, J };

/** 
   one instruction in machine code  
*/
typedef struct code_line { unsigned long instruction[4]; } CODELINE;

/** 
   unlimited register 
*/
typedef struct _register_ {
  int *data;
  size_t size;
} REGISTER;


/**
  Allocate extra space for the register, reset the new locations
  to zero.
*/
REGISTER *expand_register(REGISTER *reg) {
  size_t end = reg->size;
  reg->size += INIT_SIZE;
  reg->data = realloc(reg->data, reg->size * sizeof(int));
  memset(reg->data + end, 0, sizeof(int) * (reg->size - end));
  return reg;
}

/**
 Execute one instruction given by the machine code
 located in pgm; update the program counter cnt and
 register *p
*/
unsigned int execute_instruction(unsigned int cnt, CODELINE pgm,
                                 REGISTER **p) {
  unsigned int opc = pgm.instruction[0], op1 = pgm.instruction[1],
               op2 = pgm.instruction[2], op3 = pgm.instruction[3];

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

/** 
    Read one instruction in the string code and
    return the corresponding machine code
*/
CODELINE read_instruction(char *code) {
  char *nxt;
  CODELINE pgm = { .instruction = {0L, 0L, 0L, 0L} };
  unsigned int opc = strtoul(code, &nxt, 0);
  switch (opc) {
  case Z:
    pgm.instruction[1] = strtoul(nxt, 0, 0);
    break;
  case S:
    pgm.instruction[1] = strtoul(nxt, 0, 0);
    break;
  case T:
    pgm.instruction[1] = strtoul(nxt, &nxt, 0);
    pgm.instruction[2] = strtoul(nxt, 0, 0);
    break;
  case J:
    pgm.instruction[1] = strtoul(nxt, &nxt, 0);
    pgm.instruction[2] = strtoul(nxt, &nxt, 0);
    pgm.instruction[3] = strtoul(nxt, 0, 0);
    break;
  DEFAULT:
    fprintf(stderr, "illegal instruction\n");
  }
  return pgm;
}

/** 
   Initialise the URM
*/
void init_urm(CODELINE **pgm, REGISTER **reg) {
   *pgm = (CODELINE *)calloc(sizeof(CODELINE), INIT_SIZE);
   *reg = (REGISTER *)calloc(sizeof(REGISTER), INIT_SIZE);

}

/** 
  Free the URM memory
*/
void free_urm(CODELINE *pgm, REGISTER *reg) {
  free(pgm);
  free(reg);
}

int main(int argc, const char **argv) {

  FILE *fp = stdin;
  char *line = (char *) calloc(sizeof(char), INIT_SIZE);
  size_t cap = INIT_SIZE,pgm_size = 0, max_pgm_size =
    INIT_SIZE, pgm_cnt;
  CODELINE *pgm;
  REGISTER *reg;

  if (argc > 1)
    fp = fopen(argv[1], "r");

  init_urm(&pgm, &reg);
  
  if (getline(&line, &cap, fp) > 0) {
    if (pgm_size > max_pgm_size) {
      max_pgm_size += INIT_SIZE;
      pgm = (CODELINE *)realloc(pgm, max_pgm_size * sizeof(CODELINE));
    }
    pgm[pgm_size++] = read_instruction(line);
  }

  while (pgm_cnt < pgm_size)
    pgm_cnt = execute_instruction(pgm_cnt, pgm[pgm_cnt], &reg);

  printf("%d", reg->data[0]);

  free_urm(pgm, reg);
  return 0;
}
