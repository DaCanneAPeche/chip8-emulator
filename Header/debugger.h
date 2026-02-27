#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <stdint.h>
#include "registers.h"

typedef struct Debugger
{
  uint16_t instruction;
  int* PC;
  Registers* registers;
} Debugger;

#endif
