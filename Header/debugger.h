#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <stdint.h>
#include "registers.h"
#include "timers.h"
#include "input_manager.h"

typedef struct Debugger
{
  bool render;

  uint16_t instruction;
  int* PC;
  Registers* registers;
  Timers* timers;
  InputManager* inputManager;
} Debugger;

#endif
