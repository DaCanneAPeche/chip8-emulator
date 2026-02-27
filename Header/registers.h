#ifndef REGISTERS_H
#define REGISTERS_H

#include <stdint.h>

typedef struct Registers
{
  char V[16];
  uint16_t I;
} Registers;

#endif
